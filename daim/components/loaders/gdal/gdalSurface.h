#ifndef gdalSurface_h
#define gdalSurface_h
/* :::BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2010 David Marteau
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
// File         : gdalSurface.h
// Date         : 15 janv. 2010
// Author       : David Marteau
//--------------------------------------------------------

#include "cciIRemoteSurface.h"
#include "cciIMetaDataContainer.h"

class gdalSurface : public cciIRemoteSurface,
                    public cciIScriptableSurface,
                    public cciIMetaDataContainer
{
friend class cciGDALDriver;
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_ISURFACE
  CCI_DECL_ISCRIPTABLESURFACE
  CCI_DECL_IREMOTESURFACE
  CCI_DECL_IMETADATACONTAINER

   gdalSurface();
  ~gdalSurface();
  
   static cci_result CreateInMemorySurface(const dmImageData &, 
                                           const dm_uint8* alphaBits, 
                                           dm_int32 alphaStride ,
                                           GDALDatasetH& hDS );

   static cci_result Create(GDALDatasetH aDataset, dm_uint32 ioFlags, GDALDriverH tmpDriver,
                            gdalSurface** result);

   static cci_result Create(GDALDriverH aDriver,const char * location,
                            dm_uint32 width, dm_uint32 height,
                            EPixelFormat format, dm_bool hasAlpha,
                            const char *options,
                            gdalSurface** result);

   static cci_result Create(GDALDriverH aDriver,const char * location,
                            dmImageData& imData, dm_uint8* alphaBits, dm_int32 alphaStride,
                            const char *options,
                            gdalSurface** result);

protected:
   
   cci_result Init(GDALDatasetH aDS, dm_uint32 ioFlags, GDALDriverH tmpDriver );

   cci_result Init(GDALDriverH aDriver,const char * location,
                   dmImageData&, dm_uint8* alphaBits, dm_int32 alphaStride,
                   const char *options);

   cci_result Init(GDALDriverH aDriver,const char * location,
                   dm_uint32 width, dm_uint32 height,
                   EPixelFormat format, dm_bool hasAlpha,
                   const char *options);

  cci_result CreateUserBuffer(dmRect&, EPixelFormat, dmImageData&,dm_bool wantAlpha);
  cci_result ReadData(EPixelFormat, dmImageData&, dm_bool wantAlpha, dm_uint32 lockModes);

  GDALDatasetH mDS;
  GDALDriverH  mOutputDriver;
  GDALDriverH  mTmpDriver;
  GDALAccess   mAccess;

  int          mAlphaBand;
  EPixelFormat mFormat;
  dm_bool      mRequireUserBuffer;


  char* *mCreateOpts;

  dm_uint32    mLock;
  dmRect       mLockRect;

  dmCString    mLocation;

  dmLink<dmImage> mImageBuffer;
  dmLink<dmImage> mAlphaBuffer;

  cci_Ptr<cciIInterfaceRequestor> mCallBacks;

  GDALDriverH GetGDALDriver()
  {
    GDALDriverH hDrv = mOutputDriver ? mOutputDriver : mTmpDriver;
    if(!hDrv)
        hDrv = GDALGetDatasetDriver(mDS);

    return hDrv;
  }

};


#endif /* gdalSurface_h */
