//--------------------------------------------------------
// Copyright (c) 2004 David Marteau
// 
// File         : dmCircleEdit.h
// Date         : 10/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
// THIS FILE IS SUBJECT TO CHANGE WITHOUT NOTICE !!
//--------------------------------------------------------
#ifndef dmCircleEdit_h
#define dmCircleEdit_h


class dmCircleEdit : public dmAreaEdit 
{
  private:
    dmCircleShape mCircle;
    bool          mbShowCenter;
    bool          mbShowDiameter;

  public:
    dmCircleEdit( dmTool* _parent = NULL );	 
    dmCircleEdit( const dmCircle&, dmTool* _parent = NULL );	 
    dmCircleEdit( const dmCircleEdit& _toCopy, dmTool* _parent = NULL );
   ~dmCircleEdit();
  
  protected:
    virtual void OnUpdateRegion( dmRegion& );

    const dmCircleShape& GetCircle()     { return mCircle; }
    void  SetCircle( const dmCircle& c ) { mCircle = c;    }

  public:
    void SetCoordinates( const dmCircle& c );
 
    dmCircleEdit& operator=( const dmCircle& c) { 
      SetCoordinates(c); 
      return *this; 
    }

    operator dmCircle() const { return  mCircle; }         
 
    void ShowCenter  ( bool );
    void ShowDiameter( bool );

    virtual void OnDisplay( dmGraphics& ) const;
    virtual dmTool* Clone( dmTool* _parent = NULL ) const;

  public:
    dmDeclareClassInfo( dmCircleEdit );

};

#endif // dmCircleEdit_h

