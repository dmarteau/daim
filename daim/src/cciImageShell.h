#ifndef cciImageShell_h
#define cciImageShell_h
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
// File         : cciImageShell.h
// Date         : 15 déc. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "cciCOMPtr.h"
#include "cciIFilterContext.h"
#include "cciIImageContainer.h"
#include "cciIImageShell.h"
#include "cciIImageMath.h"
#include "cciIColorTable.h"

#include "daim_kernel.h"

class cciISurfaceDriver;

/* Header file */
class cciImageShell : public cciIImageShell,
                      public cciIImageContainer
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IIMAGESHELL
  CCI_DECL_IIMAGECONTAINER

  cciImageShell();
  cciImageShell(const cciImageShell* aSrc);

private:
  ~cciImageShell();

  cci_result EnsureFilterContext();
  cci_result EnsureImageMath();

  void ClearOpenedResources();

protected:
  dm_bool mLock;
  dm_bool mLockAlpha;
  dm_bool mPreserveMetaData;

  dmLink<dmImage> mImage;
  dmLink<dmImage> mAlpha;

  cci_Ptr<cciIImageMath>      mImageMath;
  cci_Ptr<cciIFilterContext>  mFilterContext;
  cci_Ptr<cciISurface>        mCurrentSurface;
  cci_Ptr<cciIColorTable>     mColorTable;

  cci_Ptr<cciISurfaceDriver>  mDriverCache;
};


#endif /* cciImageShell_h */
