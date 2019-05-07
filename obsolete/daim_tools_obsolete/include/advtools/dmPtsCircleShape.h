//--------------------------------------------------------
// Copyright (c) 2004 David Marteau
// 
// File         : dmPtsCircleShape.h
// Date         : 10/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
// THIS FILE IS SUBJECT TO CHANGE WITHOUT NOTICE !!
//--------------------------------------------------------
#ifndef dmPtsCircleShape_h
#define dmPtsCircleShape_h

class dmPtsCircleShape : public dmShape
{
  private:
    mutable bool _valid;

  protected:
    virtual void DoFrame( dmGraphics& , int _index, int drawFlags ) const;

  public:
   dmPtsCircleShape();
   dmPtsCircleShape( const dmPoint& p1, const dmPoint& p2, const dmPoint& p3 );

   explicit dmPtsCircleShape( const dmPtsCircleShape& );

   virtual ~dmPtsCircleShape() {}
   
   void SetPoints(  const dmPoint& p1, const dmPoint& p2, const dmPoint& p3 );
   void BuildRegion( dmRegion& ) const;

   operator dmCircle() const;

   bool IsValid() const { return _valid;      }

   virtual ECoordinatesType Type() const { return eCircleType; }
   virtual void Accept( __dmShapeVisitor& _v) const { _v.Visit( *this); }  
};

#endif // dmPtsCircleShape_h
