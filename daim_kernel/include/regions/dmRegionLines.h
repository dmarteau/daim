#ifndef dmRegionLines_h
#define dmRegionLines_h

/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2005 David Marteau
 *
 * This file is part of the DAIM Image Processing Library (DAIM library)
 *
 * The DAIM library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License (LGPL)
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * The DAIM library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the DAIM library; see the file LICENSE.  If
 * not, write to the Free Software Foundation, Inc., 59 Temple Place -
 * Suite 330, Boston, MA 02111-1307, USA.
 *
 *  ::: END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : dmRegionLines.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

// Next is for region definition

typedef struct {
  long x1,x2;
} dmRgnPair;

// variable length structure
typedef struct {
  long       rl_npair;
  dmRgnPair  rl_xpair[1];
} dmRegionLine;

#define PairWidth(xp) ((xp).x2-(xp).x1+1)
#define PairEqual(xp1,xp2) ( ((xp1).x1==(xp2).x1) && ((xp1).x2==(xp2).x2) )

typedef dmRegionLine* dmRgnLinePtr;

#define dmRegionLineSize(n)( sizeof(dmRegionLine) + (n)*sizeof(dmRgnPair) )
#define dmSizeofRgnLine(l) ( dmRegionLineSize( (l).rl_npair) )
#define DISTANCE(l1,l2)    ((dm_byte*)(l1)-(dm_byte*)(l2))

class dmRgnHandle;


//------------------------------------------------------------
class dmRgnLineArray
{
  friend class dmRgnHandle;

  private:
    dmRgnLinePtr ptr;
    size_t       buffsize;

  public:
    enum {
      MaxDfltArraySize = DM_CONFIG_DFLT_RGNARRAYSIZE,
      SecurityBuffer   = 512*sizeof(dmRgnPair)
    };

   __dmKernel void    Dealloc();
   __dmKernel dm_size Realloc( dm_size size = MaxDfltArraySize );

   dm_size Inflate( dm_size size ) {
      return Realloc(size+MaxDfltArraySize);
   }

   dmRgnLinePtr operator()() const { return (ptr); }
   dmRgnLineArray() : ptr(NULL), buffsize(0) {}
};
//------------------------------------------------------------
// Definition d'un iterateur associe, unidirectionel
//------------------------------------------------------------
class rgnline_iterator {
  friend class dmRgnHandle;
  private:
    dmRgnLinePtr line;
  public:
    rgnline_iterator(dmRgnLinePtr l) : line(l) {};
    bool operator==(const rgnline_iterator& it) const { return line == it.line; }
    bool operator!=(const rgnline_iterator& it) const { return line != it.line; }

    dmRgnLinePtr operator*() const { return line; }

    rgnline_iterator& operator++() {
      line = (dmRgnLinePtr)( (dm_byte*)line + dmSizeofRgnLine(*line) );
      return *this;
    }
    rgnline_iterator operator++(int) {
      rgnline_iterator tmp = *this;
      ++*this;
      return tmp;
    }

    rgnline_iterator operator+(dm_uint offset) { return (dmRgnLinePtr)((dm_byte*)line+offset ); }

    dmRgnPair* begin() { return line->rl_xpair; }
    dmRgnPair* end()   { return line->rl_xpair + line->rl_npair; }

    const dmRgnPair* begin() const { return line->rl_xpair; }
    const dmRgnPair* end()   const { return line->rl_xpair + line->rl_npair; }
};

inline rgnline_iterator dmGetRegionLine(rgnline_iterator l,dm_uint n)
{
  for(dm_uint i=0;i<n;i++)
    ++l;

   return (l);
}

//------------------------------------------------------------
// Class dmRgnHandle
//------------------------------------------------------------
class  dmRgnEditor;
class  dmRegionLineBuffer;
struct dmRegionBuffer;

class dmRgnHandle {

  friend class dmRgnEditor;
  friend class dmRegionLineBuffer;

  public:
   static __dmKernel dmRgnHandle* SubRegionData( const dmRgnHandle*,const dmRgnHandle*, dmRgnHandle* );
   static __dmKernel dmRgnHandle* AddRegionData( const dmRgnHandle*,const dmRgnHandle*, dmRgnHandle* );
   static __dmKernel dmRgnHandle* XorRegionData( const dmRgnHandle*,const dmRgnHandle*, dmRgnHandle* );
   static __dmKernel dmRgnHandle* AndRegionData( const dmRgnHandle*,const dmRgnHandle*, dmRgnHandle* );

   static __dmKernel dmRgnHandle* OuterBoundaryRgn( dmRgnHandle*, const dmRgnHandle* );
   static __dmKernel dmRgnHandle* InnerBoundaryRgn( dmRgnHandle*, const dmRgnHandle* );

   static __dmKernel dmRgnHandle* FlipVerRgn( dmRgnHandle*, const dmRgnHandle* );
   static __dmKernel dmRgnHandle* FlipHorRgn( dmRgnHandle*, const dmRgnHandle* );

   static __dmKernel dmRgnHandle* CreateRegion();
   static __dmKernel dmRgnHandle* CreateRegion( const dmCircle*  );
   static __dmKernel dmRgnHandle* CreateRegion( const dmRect*    );
   static __dmKernel dmRgnHandle* CreateRegion( const dmLine*    );
   static __dmKernel dmRgnHandle* CreateRegion( const dmEllipse* );
   static __dmKernel dmRgnHandle* CreateRegion( const dmPoly*    );

   static __dmKernel void DisposeRegion( dmRgnHandle* );

  public:
    typedef rgnline_iterator iterator;

  private:
    dmRect          rgn_box;
    dm_size         rgn_size;
    dmRgnLineArray  rgn_data;

    mutable dmRegionBuffer* rgn_opbuf;

    __dmKernel void Construct(const dmRgnHandle&);
    __dmKernel void Construct(const dmCircle&);
    __dmKernel void Construct(const dmRect&);
    __dmKernel void Construct(const dmLine&);
    __dmKernel void Construct(const dmEllipse&);
    __dmKernel void Construct(const dmPoly&);
    __dmKernel void Destroy();

  public:
    dmRgnHandle() : rgn_box(0,0,0,0),rgn_size(0), rgn_opbuf(NULL) {}

    dm_size Size()  const { return (rgn_size);  }
    bool    Empty() const { return (rgn_size==0); }

    // Reserve space for buffering (set region to empty)
    __dmKernel iterator Reserve(const dmRect& rect,dm_size size = dmRgnLineArray::MaxDfltArraySize);

    __dmKernel void FreeRegion();

    // Adjust the BBox by removing empty lines at the beginning and
    // at the end of the region definition (RegionArray) and compute
    // the new resulting BBox.
    __dmKernel dmRgnHandle* AdjustBoundingBox();

    dmRgnHandle(const dmRgnHandle& rgn) { Construct(rgn); }

    // Region constructors from a shape definition
    dmRgnHandle(const dmCircle& c)  { Construct(c); }
    dmRgnHandle(const dmRect& r)    { Construct(r); }
    dmRgnHandle(const dmLine& l)    { Construct(l); }
    dmRgnHandle(const dmEllipse& e) { Construct(e); }
    dmRgnHandle(const dmPoly& p)    { Construct(p); }

    ~dmRgnHandle() { Destroy(); }

	// Operateur de copie
    __dmKernel dmRgnHandle& operator=(const dmRgnHandle& rgn);

    iterator Begin() const {  return (rgn_data()); }
    iterator End()   const {  return (Begin() + rgn_size); }

    const dmRect&  Box()       const { return (rgn_box); }
    const dmRect&  Rectangle() const { return (rgn_box); }

    __dmKernel void Translate(long dx,long dy);
    __dmKernel dmRgnHandle* ClipToRect(const dm_rect& );

    __dmKernel bool PointInRegion( long x, long y ) const;
    __dmKernel bool Intersect( const dm_rect& r) const;         // return true if intersects the region
    __dmKernel bool Intersect( const dmRgnHandle& _rgn ) const; // return true if both regions intersect
        
    __dmKernel bool    IsRectRgn() const;  // test si la region est rectangulaire
    __dmKernel dm_uint Area()      const;  // retourne l'"Aire" de la region

    __dmKernel bool operator==(const dmRgnHandle& ) const;

    bool PointInRegion( const dm_point& p ) const { return PointInRegion(p.x,p.y); }

}; // Class dmRegionHandle
///
//------------------------------------------------------------
// dmRgnEditor : Use this class to handle creation of new
// region
//------------------------------------------------------------
class dmRgnEditor
{
 private:
  dmRgnHandle       s_rgn;
  rgnline_iterator  s_iter;
  dmRgnPair*        s_pair;
  dm_size           s_size;

 public:
  __dmKernel void Initialize(const dmRect& r,dm_size size = dmRgnLineArray::MaxDfltArraySize);
  __dmKernel void EnlargeBuffer(dm_size _size);

  __dmKernel const dmRgnHandle& Validate();

  dmRgnEditor() : s_iter(0),s_pair(0),s_size(0) {}
  dmRgnEditor(const dmRect& r,dm_size size = dmRgnLineArray::MaxDfltArraySize)
  : s_iter(0),s_pair(0),s_size(0) {  Initialize(r,size); }

  virtual ~dmRgnEditor() {}

  const dmRgnHandle& Region() const { return s_rgn; }

  //---------------------------------------------
  // must be used between Initialize/Validate calls
  //---------------------------------------------

  __dmKernel void PushLine();

  void AddSegment(long x1,long x2) {
    (*s_pair).x1 = x1;
    (*s_pair).x2 = x2;
    ++s_pair;
    ++(*s_iter)->rl_npair;
  }
};
//------------------------------------------------------------

#endif // dmRegionLines_h

