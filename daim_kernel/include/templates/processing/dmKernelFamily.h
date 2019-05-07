#ifndef dmKernelFamily_h
#define dmKernelFamily_h

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
// File         : dmKernelFamily.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

class dmMaskDescription
{
  protected:
    dm_int mOx,mOy;         // set the anchor for the mask
    size_t mWidth,mHeight;  // size of that mask

    void Init(dm_int _ox, dm_int _oy, size_t _width, size_t _height);

  public:
    dmMaskDescription();
    dmMaskDescription(const dmMaskDescription& _aMask );
    dmMaskDescription( dm_int _ox, dm_int _oy, size_t _width, size_t _height);
    dmMaskDescription( dm_int _ox, dm_int _oy, size_t _width );
    dmMaskDescription( size_t _width, size_t _height);
    dmMaskDescription( size_t _width );
    virtual ~dmMaskDescription() {}

    dmMaskDescription& operator=(const dmMaskDescription&);

    size_t Size()   const { return mWidth*mHeight; }
    size_t Width()  const { return mWidth;  }
    size_t Height() const { return mHeight; }
    dm_int Ox()     const { return mOx; }
    dm_int Oy()     const { return mOy; }

    dmRect GetMask( const dmRect& _rect ) const; // Rectangle utile calcul� � partir de _rect
    dmRect GetRect( const dmRect& _rect ) const; // Inverse de mask(...) => (mask o rect) == ident.

    void SetOrigin(dm_int _ox,dm_int _oy);
};
//-----------------------------------------------------------------------
class dmKernelDescription : public dmMaskDescription
{
  public:
    typedef dm_int32 value_type;

  protected:
    value_type  mNorm;
    value_type* mData;

  public:
    dmKernelDescription();
    dmKernelDescription(const dmKernelDescription& _kernel);
    dmKernelDescription(
       dm_int _ox,
       dm_int _oy,
	     size_t _width,
	     size_t _height,
	     const value_type* _data=NULL);

    virtual ~dmKernelDescription();

    void SetDescription( dm_int _ox,dm_int _oy,size_t _width,size_t _height,
                         const value_type* _data=NULL);

    const value_type* Data()       const { return mData;  }
    int operator[](unsigned int i) const { return mData[i]; }

    value_type Norm() const;  // Cumulated sum

    void SetNorm( value_type value ) { mNorm=value; }

    void Complement(); // Compute the complementary as a structuring element
    void Transpose();  // Compute the tranposed     as a structuring element

    dmKernelDescription& operator=( const dmKernelDescription& );

};

// Define as a structuring element
typedef dmKernelDescription dmStructuringElement;

//--------------------------------------------------------
// Define a family of convolution kernel/structuring element
//--------------------------------------------------------
class dmKernelFamily
{
  public:
    typedef dmVector<dmKernelDescription>  family_t;
    typedef family_t::iterator iterator;

  private:
    int      _mode;
    family_t _family;

  public:
    dmKernelFamily();
    dmKernelFamily( const dmKernelFamily& );

    void Complement();
    void Transpose();

    iterator Begin() { return _family.Begin(); }
    iterator End()   { return _family.End();   }

    void Add(const dmKernelDescription& _k) {
      _family.Push_Back(_k);
    }

    iterator Erase (iterator it)  { return _family.Erase(it); }
    iterator Insert(iterator it, const dmKernelDescription& _k = dmKernelDescription()) {
      return _family.Insert(it,_k);
    }

    void   Clear() {  _family.Clear();  }

    bool   Empty() const { return _family.Empty();  }
    size_t Size()  const { return _family.Size();   }

          dmKernelDescription& operator[]( int i )       {  return _family.At(i); }
    const dmKernelDescription& operator[]( int i ) const {  return _family.At(i); }

    void GetMaskDescription( dmMaskDescription& ) const;

    int  GetMode() const { return _mode; }
    void SetMode( int mode ) { _mode = mode; }

    dmKernelFamily& operator=( const dmKernelFamily& );
    
    DM_DECL_ARENA_OPERATOR_NEW(dmKernelFamily)
};
//-----------------------------------------------------------------------
// Inline members
//-----------------------------------------------------------------
inline dmMaskDescription::dmMaskDescription( dm_int _ox,dm_int _oy,size_t _width,size_t _height)  {
  Init(_ox,_oy,_width,_height);
}
//-----------------------------------------------------------------
inline dmMaskDescription::dmMaskDescription( dm_int _ox, dm_int _oy, size_t _width ) {
  Init(_ox,_oy,_width,_width);
}
//-----------------------------------------------------------------
inline dmMaskDescription::dmMaskDescription( size_t _width, size_t _height) {
  Init(_width/2,_height/2,_width,_height);
}
//-----------------------------------------------------------------
inline dmMaskDescription::dmMaskDescription( size_t _width ) {
	Init(_width/2,_width/2,_width,_width);
}
//-----------------------------------------------------------------

#endif // dmKernelFamily_h
