
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

#ifndef daim_info_h
#define daim_info_h

#define dmUseKernelImage
#define dmUseKernelBuffer
#include "daim_kernel.h"

//----------------------------------------------------
// Definition of image document :
//
// - Masks
// - Displayable image
// - LUT
//----------------------------------------------------
class daim_info;
//----------------------------------------------------
// daim_document is a proxy used by daim_info to delegate
// notifications/actions/events to the associated document object
//----------------------------------------------------

class daim_document
{
  public:
    daim_info* mInfo; // Owner of this daim_document

    daim_document(daim_info* _info) : mInfo(_info) {};
    virtual ~daim_document() {}

    virtual void OnSetTitle     ( dm_bool bStatus ) {};
    virtual void OnSetMask      ( dm_bool bStatus ) {};
    virtual void OnSetModified  ( dm_bool bStatus ) {};
    virtual void OnSetActiveInfo( dm_bool bStatus ) {};
    virtual void OnRecalculate  ( dm_bool bStatus ) {};

    virtual void OnRefresh();
    virtual void OnUpdateObject();

    virtual void UpdateMask(bool) {}

    virtual void OnNewDocument(  dm_bool bStatus ) {};

    // For implementors
    virtual void* This() { return NULL; }
};

//----------------------------------------------------

class daim_info
{
protected:
  static daim_info* _pInfoActive;

protected:
  bool mbFromDisk;
  bool mbModified;

  dm_double mMinRange;
  dm_double mMaxRange;

  int mDocAttrs;
  int mLastDocError;

  dmLink<dmImage> mImage;
  dmImageBuffer   mImageBuffer;

 
  dmRegion mMask;
  dmRegion mExtMask;

  dmString mTitle;
  dmString mPath;

  dmRect   mImageRect;

protected:
  void SetImageMask ( dmRegion&, const dmRegion&,int _op );
  void UpdateMask   ( bool bUpdate  );

public:
   daim_info();
  ~daim_info();

  daim_document* mDocument;
 
  //---------------------
  // Accesssors
  //---------------------
  const dmString& GetTitle() const { return mTitle; }
  const dmString& GetPath()  const { return mPath;  }

  dmLink<dmImage>& GetImage() { return mImage; }

  int GetDocAttrs()     const { return mDocAttrs;     }
  int GetLastDocError() const { return mLastDocError; }

  bool IsModified()         const { return mbModified;      }
  bool IsEmptyView()        const { return mImage.IsNull(); }
  bool IsGrayScaleImage()   const;
  bool IsRGBImage()         const;

  EPixelFormat GetImageFormat() const;
  dm_uint      NumChannels()    const;

  const dmRect& GetImageRect() const { return mImageRect; }

  void SetDensityRange( dm_double  rmin, dm_double  rmax, bool _refresh=true );
  void GetDensityRange( dm_double& rmin, dm_double& rmax ) const; 

  dm_double MaxRange() const { return mMaxRange; }
  dm_double MinRange() const { return mMinRange; }

  dmImageBuffer* GetBuffer() { return &mImageBuffer; }

  //---------------------
  // Masks 
  //---------------------
  void SetExtMask( const dmRegion& _roi );
  
  dmRegion&       GetMask()       { return mMask; }
  const dmRegion& GetMask() const { return mMask; }

  dmRegion&       GetExtMask()         { return mExtMask; }
  const dmRegion& GetExtMask()   const { return mExtMask; }
  dm_uint         GetMaskSize()  const { return mMask.Area();  }

  //------------------------
  // ROI Cache
  //------------------------
  dmRegion mROI;

  //------------------------
  // Images transformations 
  //------------------------

  dmLink<dmImage> ConvertTo( EPixelFormat );
  dmLink<dmImage> ConvertToRGB  ();
  dmLink<dmImage> ConvertTo8Bits();
  dmLink<dmImage> RotateImage( int way );
  dmLink<dmImage> FlipImage  ( int way );

  bool RevertToSaved();
  bool FromDisk()  const { return mbFromDisk;}

  //------------------------------------------------
  // Buffer opérations
  //------------------------------------------------
  bool RestoreFromBuffer();
  bool SaveToBuffer();
  bool SaveRegion( const dmString::E* _path, const dmRegion& );
  //------------------------------------------------
  void SetImage( dmLink<dmImage>& );
  void SetTitle( const dmString& _title );

  void SetMask ( const dmRegion& _roi, int operation=0 );
  void ResetMask();
  void KillMask();

  void ClearImage();
  bool IsValidView() const;
  void SetModified(bool);
  void RecalcInfo();

  void ClearDocument();
  bool SaveDocument( const dmString::E* _path, bool bReplace );
  bool OpenDocument( const dmString::E* _path, bool bReplace );

  bool       IsActiveInfo() const;
  daim_info* SetActiveInfo( bool bactive );

  void Refresh();
  void UpdateObject();

  static  daim_info* GetInfo();

};
//------------------------------------------------
// Inlines
//------------------------------------------------
inline EPixelFormat daim_info::GetImageFormat() const
{
  return (IsEmptyView()?dmPixelFormatUndefined:mImage->PixelFormat());
}
//------------------------------------------------
inline dm_uint daim_info::NumChannels() const
{
  EPixelFormat _Format = GetImageFormat();
  return (dmIsPixelFormatScalar(_Format) ? 1:
         (_Format==dmPixelFormat24bppRGB ? 3 : 0)); 
}
//------------------------------------------------
#endif // daim_info_h
