//--------------------------------------------------------
// Copyright (c) 2004 David Marteau
// 
// File         : dmSector.h
// Date         : 10/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
// THIS FILE IS SUBJECT TO CHANGE WITHOUT NOTICE !!
//--------------------------------------------------------
#ifndef dmSector_h
#define dmSector_h

class dmSector : public dmPolyEdit
{
  public:
    dmSector( dmTool* _parent = NULL );
    dmSector( const dmSector& _toCopy, dmTool* _parent = NULL );
   ~dmSector() {}
 
  public:
    virtual void    OnDisplay( dmGraphics& ) const;
    virtual dmTool* Clone( dmTool* _parent = NULL ) const;

    virtual bool OnMouseUp(const dmPoint&,dm_uint _button,dm_uint _keyQual); 

  public:
    dmDeclareClassInfo( dmSector );
};

#endif // dmSector_h
