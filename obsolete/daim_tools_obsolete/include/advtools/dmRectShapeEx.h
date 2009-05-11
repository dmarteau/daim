//--------------------------------------------------------
// Copyright (c) 2004 David Marteau
// 
// File         : dmRectShapeEx.h
// Date         : 10/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
// THIS FILE IS SUBJECT TO CHANGE WITHOUT NOTICE !!
//--------------------------------------------------------
#ifndef dmRectShapeEx_h
#define dmRectShapeEx_h


class dmRectShapeEx : public dmShape
{
  protected: 
    virtual void DoMove     ( int _index, int dx, int dy );
    virtual void DoMoveIndex( int _index, int dx, int dy );

   void DoUpdate();

  private:
   dmPoly    mShape;
   dmRect    mRect;
   dm_double mAngle;
   bool      mFixedsize;

  public:
    virtual void DoFrame( dmGraphics& , int _index, int drawFlags ) const;

  public:
   dmRectShapeEx();
   dmRectShapeEx( const dmRect&, dm_real _angle );
   explicit dmRectShapeEx( const dmRectShapeEx& );

   virtual ~dmRectShapeEx() {}

   virtual int Initialize( const dmPoint& p );
  
   void BuildRegion( dmRegion& ) const;

   void SetRect ( const dmRect&, bool _update =true);
   void SetShape( const dmRect&, dm_real _angle, bool _update =true );
   void SetOrientation( dm_double _angle  , bool _update =true);
   void SetFixedSize( bool _bfixed ) { mFixedsize = _bfixed; }

   bool          IsFixedSize()    const { return mFixedsize; }
   const dmRect& GetRect()        const { return mRect;  }
   dm_double     GetOrientation() const { return mAngle; }

   dmPoint Center() const { return mRect.Center(); }

   virtual ECoordinatesType Type() const { return eRectangleType; }

   virtual void Accept( __dmShapeVisitor& _v ) const { _v.Visit(mRect); }
}; 

#endif // dmRectShapeEx_h
