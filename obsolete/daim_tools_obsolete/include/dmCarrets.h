#ifndef dmCarrets_h
#define dmCarrets_h

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
// File         : dmCarrets.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

//----------------------------------------------------------------------
// visitor pattern pour les formes standarts 
//----------------------------------------------------------------------
class __dmShapeVisitor
{
  public:
    virtual ~__dmShapeVisitor() {}
    virtual  void Visit( const dmLine& )    {}
    virtual  void Visit( const dmRect& )    {}
    virtual  void Visit( const dmPoly& )    {}
    virtual  void Visit( const dmEllipse& ) {}
    virtual  void Visit( const dmCircle& )  {}
};
//----------------------------------------------------------------------
// class dmShape
//----------------------------------------------------------------------
class __dmTools dmShape 
{ 
  public :
     typedef void (*drawfunc)( dmGraphics& , int, const dmShape& );
  private :
    typedef enum {    
      csVisible,
      csRigid,
      csDrawFrame,
      csNoFrameMove,
      csMoving,
      csHoldGraphics,
    } _CarretsStates;
 
    friend class dmSetOf<_CarretsStates>;

    drawfunc        mDrawCarret;
    dmPenAttributs* mPenAttrs;
    dmPenAttributs* mRoiAttrs;

    dmSetOf<_CarretsStates> mStates;
    int                     mCarretSize;
 
  protected:
    dmPoly  mCarrets;

  public:
    typedef dmPoly::iterator       iterator;
    typedef dmPoly::const_iterator const_iterator;

  protected:
    dmShape( size_t n ); // protected constructor

    iterator  Begin()   { return mCarrets.Begin(); } 
    iterator  End( )    { return mCarrets.End  (); }

    virtual void DoMove     ( int _index, int dx, int dy );
    virtual void DoMoveIndex( int _index, int dx, int dy );
    virtual void DoFrame    ( dmGraphics& , int _index, int drawFlags ) const {}

  public: 
      dmShape();
      explicit dmShape( const dmPoly& );
      explicit dmShape( const dmShape& );
      
      virtual ~dmShape();
      //----------------------------------------------------------------------
      drawfunc        SetDrawFunc( drawfunc );
      dmPenAttributs* SetPenAttrs ( dmPenAttributs* );
      dmPenAttributs* SetLineAttrs( dmPenAttributs* );
      dmPenAttributs* GetPenAttrs()  { return mPenAttrs; }
      dmPenAttributs* GetLineAttrs() { return mRoiAttrs; }
      //----------------------------------------------------------------------
      void MoveCarret(dmGraphics*, int _index, int dx, int dy );
      void MoveCarret(dmGraphics* gr, int _index, const dmPoint& _delta ) {
        MoveCarret(gr,_index,_delta.x,_delta.y);
      } 

      void EndMove(dmGraphics* gr, int _index, int dx, int dy );
      void EndMove(dmGraphics* gr, int _index, const dmPoint& _delta ) {
        EndMove(gr,_index,_delta.x,_delta.y);
      }

      void RedrawFrame(dmGraphics*) const;

      //----------------------------------------------------------------------
      virtual int Initialize( const dmPoint& p );
      //----------------------------------------------------------------------

      void SetMoveState( bool _st ) { mStates.Set(csMoving,_st);         }
      bool IsMoving() const         { return mStates.Contains(csMoving); }

      size_t CarretSize() const { return mCarretSize; }

      size_t ScaledCarretSize( dmGraphics* ) const;

      void   SetCarretSize( size_t sz ) { mCarretSize = sz;   }
      //----------------------------------------------------------------------
      void DisplayCarrets(dmGraphics&, int _index=-1)  const; // Display carret index
      //----------------------------------------------------------------------
      // Display the carrets or no
      //----------------------------------------------------------------------
      void Visible( bool _st) { mStates.Set(csVisible,_st);         }
      bool Visible() const    { return mStates.Contains(csVisible); }
      //----------------------------------------------------------------------
      // Prevent carrets moving relatives to each others
      //----------------------------------------------------------------------
      void RigidMode( bool _st) { mStates.Set(csRigid,_st);         }
      bool RigidMode() const    { return mStates.Contains(csRigid); }
      //----------------------------------------------------------------------
      // Prevent drawing carrets frame on moves
      //----------------------------------------------------------------------
      void FrameMove( bool _st) { mStates.Set(csNoFrameMove,!_st);         }
      bool FrameMove() const    { return !mStates.Contains(csNoFrameMove); }
      //----------------------------------------------------------------------
      const dmPoly&  Carrets() const { return mCarrets; } 
      //---------------------------------------------------------------------- 
      const_iterator  Begin() const { return mCarrets.Begin(); } 
      const_iterator  End( )  const { return mCarrets.End  (); }
      //----------------------------------------------------------------------
      const dm_point& operator[](size_t i) const { return mCarrets[i]; }
      const dm_point& Carret( size_t i )   const { return mCarrets[i]; }
      //----------------------------------------------------------------------
      size_t Size() const { return mCarrets.Size(); }
      //----------------------------------------------------------------------
      int  CarretIndex( const dmPoint& pWhere, int hitTest ) const;
      //----------------------------------------------------------------------
      virtual void BuildRegion( dmRegion& ) const {}
      virtual ECoordinatesType Type() const { return eUnknowType; }

      virtual void Accept( __dmShapeVisitor& ) const {}
};
//----------------------------------------------------------------------
class __dmTools dmRectShape : public dmShape 
{
   protected:
     virtual void DoMoveIndex( int _index, int dx, int dy );
     virtual void DoFrame( dmGraphics&, int _index, int drawFlags ) const;

   public:
     dmRectShape();
     dmRectShape( const dmRect& );
     explicit dmRectShape( const dmRectShape& );

     virtual ~dmRectShape() {}

     dmRectShape& operator=( const dmRect& );

     void BuildRegion( dmRegion& ) const;

     long Top()    const { return daim::min(mCarrets[0].y,mCarrets[2].y); }
     long Left()   const { return daim::min(mCarrets[0].x,mCarrets[2].x); }
     long Right()  const { return daim::max(mCarrets[0].x,mCarrets[2].x); }
     long Bottom() const { return daim::max(mCarrets[0].y,mCarrets[2].y); }

     dmPoint TopLeft()     const { return dmPoint(Left() ,Top())   ; }
     dmPoint TopRight()    const { return dmPoint(Right(),Top())   ; }
     dmPoint BottomLeft()  const { return dmPoint(Left() ,Bottom()); }
     dmPoint BottomRight() const { return dmPoint(Right(),Bottom()); }

     dmPoint Center() const {
       return dmPoint((mCarrets[0].x+mCarrets[2].x)/2,
                      (mCarrets[0].y+mCarrets[2].y)/2);
     }
     
     long Width()  const { return Right()  - Left() + 1; }
     long Height() const { return Bottom() - Top()  + 1;  }

     operator dmRect() const { return dmRect(mCarrets[0],mCarrets[2]); }

     virtual ECoordinatesType Type() const { return eRectangleType; }
     virtual void Accept( __dmShapeVisitor& _v) const { _v.Visit( *this); }
};
//----------------------------------------------------------------------
class __dmTools dmLineShape : public dmShape 
{
  protected:
    virtual void DoFrame( dmGraphics&, int _index, int drawFlags ) const;

  public:
    dmLineShape();
    dmLineShape( const dmLine& );
    explicit dmLineShape( const dmLineShape& );

    virtual ~dmLineShape() {}
    //----------------------------------------
    virtual int Initialize( const dmPoint& p );
    //----------------------------------------
    
    dmLineShape& operator=( const dmLine& );
    void BuildRegion( dmRegion& ) const;

    const dm_point& Start() const { return mCarrets[0]; }
    const dm_point& End()   const { return mCarrets[1]; }

    void Start(const dm_point& _p) { mCarrets[0] = _p; }
    void End  (const dm_point& _p) { mCarrets[1] = _p; }

    operator dmLine() const;

    virtual ECoordinatesType Type() const { return eLineType; }
    virtual void Accept( __dmShapeVisitor& _v) const { _v.Visit( *this); }
};
//----------------------------------------------------------------------
class __dmTools dmCircleShape : public dmShape 
{
  protected:
   virtual void DoMoveIndex( int _index  , int dx, int dy );
   virtual void DoFrame    ( dmGraphics& , int _index, int drawFlags ) const;

  public:
   dmCircleShape();
   dmCircleShape( const dmCircle& );
   explicit dmCircleShape( const dmCircleShape& );

   virtual ~dmCircleShape() {}

   dmCircleShape& operator=( const dmCircle& );
   
   void BuildRegion( dmRegion& ) const;

   dmPoint       Center() const;
   unsigned long Radius() const;

   operator dmCircle() const;

   virtual ECoordinatesType Type() const { return eCircleType; }
   virtual void Accept( __dmShapeVisitor& _v) const { _v.Visit( *this); }
};
//----------------------------------------------------------------------
class __dmTools dmEllipseShape : public dmShape 
{
  protected:
    virtual void DoMoveIndex( int _index , int dx, int dy );
    virtual void DoFrame    ( dmGraphics&, int _index, int drawFlags ) const;

  public:
    dmEllipseShape();
    dmEllipseShape( const dmEllipse& );
    explicit dmEllipseShape( const dmEllipseShape& );

    virtual ~dmEllipseShape() {}

    dmEllipseShape& operator=( const dmEllipse& );

    void Display(dmGraphics&)     const;
    void BuildRegion( dmRegion& ) const;

    dmPoint   Center() const;
    unsigned long Rx() const;
    unsigned long Ry() const;

    operator dmEllipse() const;

    virtual ECoordinatesType Type() const { return eEllipseType; }
    virtual void Accept( __dmShapeVisitor& _v) const { _v.Visit( *this); }
};
//----------------------------------------------------------------------
class __dmTools dmPolyShape : public dmShape 
{ 
  protected:
   virtual void DoMoveIndex( int _index , int dx, int dy );
   virtual void DoFrame    ( dmGraphics&, int _index, int drawFlags ) const;

  public:
   dmPolyShape();
   explicit dmPolyShape( const dmPoly& );
   explicit dmPolyShape( const dmPolyShape& );

   virtual ~dmPolyShape() {}

   dmPolyShape& operator=( const dmPoly& );

   void BuildRegion( dmRegion& ) const;
  
   typedef dmPoly::iterator iterator;
   typedef dmPoly::const_iterator const_iterator;

   iterator Begin() { return mCarrets.Begin(); }
   iterator End()   { return mCarrets.End()  ; }
   dm_point&  Back()  { return mCarrets.Back() ; }

   const_iterator Begin() const { return mCarrets.Begin(); }
   const_iterator End()   const { return mCarrets.End()  ; }
   const dm_point&  Back()  const { return mCarrets.Back() ; }

   void Clear() { mCarrets.Clear(); }

   void Push_Back (const dm_point& p) { mCarrets.Push_Back(p); }
   void Pop_Back  ()                { mCarrets.Pop_Back();   }

   void Insert( iterator it, const dm_point& p ) { mCarrets.Insert(it,p);  }
   void Insert( int index  , const dm_point& p ) { mCarrets.Insert(index,p); }

   void Remove( iterator it  ) { mCarrets.Remove(it); }
   void Remove( int index    ) { mCarrets.Remove(index); }

   void Resize( size_t _newsize ) { mCarrets.Resize(_newsize); }

   dm_point&       operator[]( int i )       { return mCarrets[i]; }
   const dm_point& operator[]( int i ) const { return mCarrets[i]; }

   operator const dmPoly&() const { return mCarrets; }

   bool Closed() const { return mCarrets.Closed(); }

   virtual ECoordinatesType Type() const { return ePolygoneType; }
   virtual void Accept( __dmShapeVisitor& _v) const { _v.Visit( *this); }
};
//----------------------------------------------------------------------
#endif // dmCarrets_h
