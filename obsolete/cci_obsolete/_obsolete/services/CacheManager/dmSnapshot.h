#ifndef dmSnapshot_h
#define dmSnapshot_h

/* ::: BEGIN LICENSE BLOCK:::
 * Copyright (c) 2005 David Marteau
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

//--------------------------------------------------------------------
// File         : dmSnapshot.h
// Date         : 29/06/2005
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------------------
class dmSnapshot
{
  private:
    dmString m_BaseImagePath;
    dmString m_CacheImagePath;
    dmString m_CacheName;

    dm_real      m_Magnification;
    dmPoint      m_BaseSize;   
    EPixelFormat m_BaseFormat;
   
    dm_uint          mStatus;
    dmGenericLoader* mDecoder;
    dmGenericLoader* mEncoder;

    dmString mFileExt;

  public:
    dmSnapshot();
   ~dmSnapshot();

    typedef enum { eAttemptOK, eNeedUpdateCache, eAttemptFailed } EAttemptResult;
    EAttemptResult AttemptLoadFromCache();                 // Check cache infos

    bool Save();

    void SetImagePath( const char_t* _path );

    dmLink<dmImage> GetSourceImage();

    const dmString& GetCacheName()         const { return m_CacheName;     }  
    const dmString& GetOriginalImagePath() const { return m_BaseImagePath; }
    const dmString& GetCachedImagePath()   const { return m_CacheImagePath;}
    const dmPoint&  ImageSize()            const { return m_BaseSize;      }

    dm_uint ImageWidth()  const { return m_BaseSize.x; }
    dm_uint ImageHeight() const { return m_BaseSize.y; }
    dm_uint ImageFormat() const { return m_BaseFormat; }

    bool LoadCacheInfos();
    bool UpdateImageCache(); // create/update cache
    void ClearImageCache();

    dmGenericLoader* SetEncoder( dmGenericLoader* );
    dmGenericLoader* SetDecoder( dmGenericLoader* );

    dmGenericLoader* GetEncoder() { return mEncoder; }
    dmGenericLoader* GetDecoder() { return mDecoder; }
};
//--------------------------------------------------------
#endif // dmSnapshot_h
