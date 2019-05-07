#ifndef dmRegionEditProto_h
#define dmRegionEditProto_h

/* :::BEGIN LICENSE BLOCK:::
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
 *  :::END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : dmRegionEditProto.h
// Date         : 7/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

class dmRegionEditProto : public dmAreaEdit
{  
  private:
    const dmRegion* mTmpRegion;
    bool            mDeleteTmp;
    int             mMergeMode;
 
  public:  
    typedef enum {
      rgnCOPY,rgnADD,rgnSUB,rgnAND,rgnXOR,rgnKILL
    } MergeMode;


  protected:
   virtual void OnUpdateRegion( dmRegion& );
   void ClearTmpRoi();

  public:
   dmRegionEditProto( dmTool* _parent = NULL );
   dmRegionEditProto( const dmRegion&, dmTool* _parent = NULL );
   dmRegionEditProto( const dmRegionEditProto& _toCopy, dmTool* _parent = NULL );
 
   virtual ~dmRegionEditProto();

   virtual void SetRoi  ( const dmRegion& );
   virtual void MergeRoi( const dmRegion&, MergeMode ); 
   virtual void KillRoi();

   virtual dmTool* Clone( dmTool* _parent = NULL ) const;
        
  public:
   dmDeclareClassInfo( dmRegionEditProto );
};
//--------------------------------------------------------
#endif // dmRegionEditProto_h
