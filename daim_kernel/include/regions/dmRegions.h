#ifndef dmRegions_h
#define dmRegions_h

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
// File         : dmRegions.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------


class dmRegion  {
private:
   typedef enum { NoRoi, RectRoi, RgnRoi } RoiTypeType;

   dmRect        RoiRect; // Rect Region Of Interest
   dmRgnHandle   RoiRgn;  // ROI as implemented in Region.h
   RoiTypeType   RoiType;

public:
   // Region Setting
   void SetRgnRoi(const dmRgnHandle& rgn) {
     if(!rgn.Empty()) { RoiType = RgnRoi; RoiRgn = rgn; RoiRect = rgn.Box(); }
     else { RoiRgn.FreeRegion(); RoiType = NoRoi; }
   }

public:
   dmRect&      Rectangle() { return RoiRect; }
   dmRgnHandle& Region()    { return RoiRgn;  }

   // const versions
   const dmRect&      Rectangle() const { return RoiRect; }
   const dmRgnHandle& Region()    const { return RoiRgn;  }

   // Rectangle Roi settings
   void SetRectRoi(const dmRect& r) {
     RoiRect = r; RoiType = RectRoi;
     if(!RoiRgn.Empty()) RoiRgn.FreeRegion();
   }

   // Use a abstract representation for coordinates
   template<class dmCoordinates>
   void SetRoi(const dmCoordinates& coords) {
  	 if(coords.TypeOfCoords()==eRectangleType) SetRectRoi( coords.BoundingBox() );
	   else {
	     dmRgnHandle* rgn = dmRgnHandle::CreateRegion(&coords);
	     if(rgn) SetRgnRoi(*rgn);
	     dmRgnHandle::DisposeRegion(rgn);
	   }
   }
/*
   template<class dmCoordinates>
   dmRegion& operator=(const dmCoordinates& coords) { 
       SetRoi(coords);  
       return *this;
   }
*/
   // Copy operator
   dmRegion& operator=(const dmRegion& roi)
   {
	   if(&roi!=this) {
		   RoiRgn  = roi.RoiRgn;
		   RoiRect = roi.RoiRect;
		   RoiType = roi.RoiType;
	   }
	   return *this;
   }
  // Arithmetics operations from dmRgnHandles
   __dmKernel void AddRegion(const dmRgnHandle& );
   __dmKernel void SubRegion(const dmRgnHandle& );
   __dmKernel void XorRegion(const dmRgnHandle& );
   __dmKernel void AndRegion(const dmRgnHandle& );

   // Arithmetics operations from dmRegion
   dmRegion& AddRoi( const dmRegion& roi );
   dmRegion& SubRoi( const dmRegion& roi );
   dmRegion& XorRoi( const dmRegion& roi );
   dmRegion& AndRoi( const dmRegion& roi );
 
   // Arithmetics operations from generic coordinate
   template<class dmCoordinates>
   void AddCoordinates( const dmCoordinates& coords );

   template<class dmCoordinates>
   void SubCoordinates( const dmCoordinates& coords );

   template<class dmCoordinates>
   void XorCoordinates( const dmCoordinates& coords );

   template<class dmCoordinates>
   void AndCoordinates( const dmCoordinates& coords );

   // Constructeurs
   dmRegion() : RoiRgn(), RoiType(NoRoi) {}
   dmRegion(const dmRgnHandle& rgn) : RoiRgn(),RoiType(NoRoi)  { SetRgnRoi(rgn); }
   dmRegion(const dmRegion& roi   ) : RoiRect(roi.RoiRect),RoiRgn(roi.RoiRgn), RoiType(roi.RoiType) {}
/*
   template<class dmCoordinates>
   dmRegion(const dmCoordinates& coords) : RoiRgn(),RoiType(NoRoi)  {
     SetRoi(coords); 
   }
*/
   // Constructors for regular type of coordinates

   dmRegion(const dmRect&    coords) : RoiRgn(),RoiType(NoRoi) { SetRoi(coords); }
   dmRegion(const dmCircle&  coords) : RoiRgn(),RoiType(NoRoi) { SetRoi(coords); }
   dmRegion(const dmLine&    coords) : RoiRgn(),RoiType(NoRoi) { SetRoi(coords); }
   dmRegion(const dmEllipse& coords) : RoiRgn(),RoiType(NoRoi) { SetRoi(coords); }
   dmRegion(const dmPoly&    coords) : RoiRgn(),RoiType(NoRoi) { SetRoi(coords); }

  ~dmRegion() {}

   // Return region outline in 8-connectivity
   __dmKernel dmRegion& GetOutLine( dmRegion& aDest, bool _inner = true) const;

   dmRegion& GetInnerBoundary( dmRegion& aDest ) const { return GetOutLine(aDest,true);  }
   dmRegion& GetOuterBoundary( dmRegion& aDest ) const { return GetOutLine(aDest,false); } 
   
   __dmKernel  dmRegion& FlipRoi( dmRegion& aDest, bool bHor,bool bVer ) const;

   void KillRoi() { RoiRgn.FreeRegion(); RoiType = NoRoi;}

   void ClipToRect( const dmRect& clip) {
	  if(RoiType==RectRoi) { if(!dmClipRectangle( RoiRect, clip)) KillRoi(); }
	  else if(RoiType==RgnRoi)  { 
         RoiRgn.ClipToRect(clip); 
         SetRgnRoi(RoiRgn);
      }
   }
   // const functions
   bool IsEmptyRoi() const { return (RoiType==NoRoi);   }
   bool IsRectRoi()  const { return (RoiType==RectRoi); }
   bool IsValidRoi() const { return (RoiType==RgnRoi) ? !RoiRgn.Empty() : true ; }

   bool PointInRoi( long x, long y ) const {
	  if(RoiType==RectRoi) return (dmPointInRectangle(x,y,RoiRect));
	  if(RoiType==RgnRoi)  return (RoiRgn.PointInRegion(x,y));
	  return false;
   }

   bool PointInRoi(const dmPoint& p) const { return PointInRoi(p.x,p.y); }

   // Return the first point in the region
   dmPoint RegionStart() const {
     if(RoiType == RgnRoi) 
       return dmPoint((*Region().Begin())->rl_xpair->x1,Rectangle().Top());
     else
       return Rectangle().TopLeft();
   }

   dm_uint Area() const;
   bool    Intersect( const dm_rect& r    ) const;
   bool    Intersect( const dmRegion& rgn ) const;

   void Translate( long dx,long dy);
   void Translate( const dmPoint& p ) { Translate(p.x,p.y); }

   void OffsetRoi( const dmPoint& p ) { Translate(-p.x,-p.y); } 
   void OffsetRoi() { OffsetRoi(Rectangle().TopLeft()); }

   // fonctions virtuelle definies dans dmCoordinates
   ECoordinatesType TypeOfCoords() const { return eRegionType; }
   unsigned long BoxWidth()        const { return Rectangle().BoxWidth(); }
   unsigned long BoxHeight()       const { return Rectangle().BoxHeight(); }
   dm_point Loc()                  const { return Rectangle().Loc(); }
   dm_rect  BoundingBox()          const { return Rectangle().BoundingBox(); }

   //dmRgnHandle*   CreateRegion()   const { 
   //  return ( IsRectRoi() ? Rectangle().CreateRegion() : new dmRgnHandle(Region())); 
   //}

   __dmKernel bool operator==(const dmRegion& ) const; // comparaison rapide de deux regions

}; // dmRegion


//---------------------------------------------------------
/// Inline member functions
//----------------------------------------------------------
inline dm_uint dmRegion::Area() const 
{ 
  return (IsRectRoi() 
  ? RoiRect.BoxWidth()*RoiRect.BoxHeight() 
  : (IsValidRoi() ? RoiRgn.Area() : 0) );
} 
//----------------------------------------------------------
inline bool dmRegion::Intersect( const dm_rect& r) const 
{     
   dmRect _rect(r);  
   return (IsRectRoi()
    ? dmClipRectangle( _rect, RoiRect )
    : (IsValidRoi() ? RoiRgn.Intersect(r) : false) ); 
}
//----------------------------------------------------------
inline bool dmRegion::Intersect( const dmRegion& roi ) const 
{
  return (IsRectRoi()
     ? roi.Intersect(RoiRect)
     : (IsValidRoi() ? RoiRgn.Intersect(roi.RoiRgn) : false) ); 
} 
//----------------------------------------------------------
inline void dmRegion::Translate(long dx,long dy) 
{
  if(!IsEmptyRoi()) {
    RoiRect.Translate(dx,dy);
    if(IsValidRoi()) RoiRgn.Translate(dx,dy);
  }
}
//----------------------------------------------------------
inline dmRegion& dmRegion::AddRoi( const dmRegion& roi)
{
  if(roi.RoiType==RectRoi)   AddCoordinates(roi.RoiRect);
  else AddRegion(roi.RoiRgn);
  return *this;
}
//----------------------------------------------------------
inline dmRegion& dmRegion::SubRoi( const dmRegion& roi)
{
  if(roi.RoiType==RectRoi)   SubCoordinates(roi.RoiRect);
  else if(!roi.IsEmptyRoi()) SubRegion(roi.RoiRgn);
  return *this;
}
//----------------------------------------------------------
inline dmRegion& dmRegion::XorRoi( const dmRegion& roi)
{
  if(roi.RoiType==RectRoi)   XorCoordinates(roi.RoiRect);
  else XorRegion(roi.RoiRgn);
  return *this;
}
//----------------------------------------------------------
inline dmRegion& dmRegion::AndRoi( const dmRegion& roi)
{
  if(roi.RoiType==RectRoi)  AndCoordinates(roi.RoiRect);
  else AndRegion(roi.RoiRgn);
  return *this;
}

//----------------------------------------------------------
/// Regions Operations from coordinates
//----------------------------------------------------------
template<class dmCoordinates>
void dmRegion::AddCoordinates( const dmCoordinates& coords )
{
   if(RoiType==NoRoi) SetRoi(coords);
   else {
     dmRgnHandle* rgn = dmRgnHandle::CreateRegion(&coords);
	 if(rgn) {
       AddRegion(*rgn);
       dmRgnHandle::DisposeRegion(rgn);
     }
   }
}
//----------------------------------------------------------
template<class dmCoordinates>
void dmRegion::SubCoordinates( const dmCoordinates& coords )
{
   if(RoiType==NoRoi) return;
   else {
     dmRgnHandle* rgn = dmRgnHandle::CreateRegion(&coords);
     if(rgn) {
       SubRegion(*rgn);
       dmRgnHandle::DisposeRegion(rgn);
     }
  }
}
//----------------------------------------------------------
template<class dmCoordinates>
void dmRegion::XorCoordinates( const dmCoordinates& coords )
{
   if(RoiType==NoRoi) SetRoi(coords);
   else {
     dmRgnHandle* rgn = dmRgnHandle::CreateRegion(&coords);
     if(rgn) {
       XorRegion(*rgn);
       dmRgnHandle::DisposeRegion(rgn);
     }
   }
}
//----------------------------------------------------------
template<class dmCoordinates>
void dmRegion::AndCoordinates( const dmCoordinates& coords )
{
   if(RoiType==NoRoi) return;
   else if(coords.TypeOfCoords()==eRectangleType) {
     dmRect r = coords.BoundingBox();
     ClipToRect(r);
   } else {
     dmRgnHandle* rgn = dmRgnHandle::CreateRegion(&coords);
      if(rgn) {
        AndRegion(*rgn);
        dmRgnHandle::DisposeRegion(rgn);
      }
   }
}
//----------------------------------------------------------
#endif // dmRegions_h

