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
// File         : cciImageListImage.cpp
// Date         : 15 févr. 2011
// Author       : David Marteau
//--------------------------------------------------------

#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"
#include "cciImageList.h"
#include "cciImageListImage.h"

/* Implementation file */
CCI_IMPL_ISUPPORTS_INHERITED1(cciImageListImage,cciScriptableImage,
                              cciIImageListImage)

cciImageListImage::cciImageListImage()
{
  /* member initializers and constructor code */
}

cciImageListImage::~cciImageListImage()
{
  /* destructor code */
}

/* void init (in cciIImageList imagelist, in unsigned long index); */
CCI_IMETHODIMP cciImageListImage::Init(cciIImageList *imagelist, dm_uint32 index)
{
  CCI_ENSURE_FALSE(mImageList,CCI_ERROR_ALREADY_INITIALIZED);
  CCI_ENSURE_ARG_POINTER(imagelist);
 
  cci_result rv;
  
  mImageList = do_QueryInterface(imagelist,&rv);
  if(CCI_FAILED(rv))
     return rv;
  
  return Select(index);
}

/* void select (in unsigned long index); */
CCI_IMETHODIMP cciImageListImage::Select(dm_uint32 index)
{
  CCI_ENSURE_TRUE(mImageList,CCI_ERROR_NOT_INITIALIZED);
  CCI_ENSURE_FALSE(mLock,CCI_ERROR_FAILURE);
  
  return mImageList->GetImageLink(mImage,index);
}

/* readonly attribute cciIImageList imageList; */
CCI_IMETHODIMP cciImageListImage::GetImageList(cciIImageList * *aImageList)
{
  cci_Ptr<cciIImageList> list = do_QueryInterface(mImageList);
  if(!list)
     return CCI_ERROR_FAILURE;
  
  list.forget(aImageList);
  return CCI_OK;
}

