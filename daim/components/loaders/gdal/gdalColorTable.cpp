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
// File         : gdalColorTable.cpp
// Date         : 4 févr. 2009
// Author       : David Marteau
//--------------------------------------------------------

#include "cciCOMPtr.h"
#include "ccidaim.h"
#include "cciIColorTable.h"

#include "gdaldrv.h"
#include "daim_kernel.h"

#include "gdalColorTable.h"

CCI_DEFINE_STATIC_IID_ACCESSOR(gdalColorTable, CCI_COLORTABLE_PRIVATE_IID)

/* Implementation file */
CCI_IMPL_ISUPPORTS2(gdalColorTable, gdalColorTable, cciIColorTable)

gdalColorTable::gdalColorTable( GDALColorTableH colorTable )
: mColorTable(colorTable)
{
}

gdalColorTable::~gdalColorTable()
{
  GDALDestroyColorTable(mColorTable);
}

cci_result gdalColorTable::copyColorTable( cciIColorTable* src, GDALColorTableH &dst )
{
   cci_Ptr<gdalColorTable> clrtble = do_QueryInterface(src);
   if(clrtble) {
     dst = GDALCloneColorTable(clrtble->mColorTable);
     CCI_ENSURE_TRUE(dst,CCI_ERROR_OUT_OF_MEMORY);
   }
   else
   {
     dst = GDALCreateColorTable(GPI_RGB);
     CCI_ENSURE_TRUE(dst,CCI_ERROR_OUT_OF_MEMORY);
     
     int dstCount = GDALGetColorEntryCount(dst);
   
     int srcCount = 0;
     cci_result rv = src->GetColorEntryCount(&srcCount);
     CCI_ENSURE_SUCCESS(rv,rv);
     
     if(srcCount > dstCount)
        srcCount = dstCount;
     
     dm_uint32 value;
     GDALColorEntry entry;
     
     for(int i=0;i<srcCount;++i) {
       rv = src->GetColorEntry(i,&value);
       if(CCI_SUCCEEDED(rv)) {
         entry.c1 = DM_GETRVALUE( value );
         entry.c2 = DM_GETGVALUE( value );
         entry.c3 = DM_GETBVALUE( value );
         entry.c4 = DM_GETAVALUE( value );
         GDALSetColorEntry(dst,i,&entry);
       }
     }
   }
   return CCI_OK;
}

/* long getColorEntryCount (); */
CCI_IMETHODIMP gdalColorTable::GetColorEntryCount(dm_int32 *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(_retval);

  *_retval = GDALGetColorEntryCount(mColorTable);
  return CCI_OK;
}

/* void setColorEntry (in dm_int32 index, in dm_uint32 c); */
CCI_IMETHODIMP gdalColorTable::SetColorEntry(dm_int32 index, dm_uint32 c)
{
  GDALColorEntry entry = {
    DM_GETRVALUE( c ),
    DM_GETGVALUE( c ),
    DM_GETBVALUE( c ),
    DM_GETAVALUE( c ),
  };

  GDALSetColorEntry(mColorTable,index,&entry);
  return CCI_OK;
}

/* dm_uint32 getColorEntry (in dm_int32 index); */
CCI_IMETHODIMP gdalColorTable::GetColorEntry(dm_int32 index, dm_uint32 *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(_retval);

  const GDALColorEntry* entry = GDALGetColorEntry(mColorTable,index);
  CCI_ENSURE_TRUE(entry,CCI_ERROR_FAILURE);

  *_retval = DM_ARGB(entry->c4,entry->c1,entry->c2,entry->c3);
  return CCI_OK;
}

/* void getColorEntries ([array, size_is (count)] inout dm_uint32 data, in unsigned long count); */
CCI_IMETHODIMP gdalColorTable::GetColorEntries(dm_uint32 **data CCI_INOUTPARAM, dm_uint32 count)
{
  CCI_ENSURE_ARG_POINTER(data);
  if(!*data)
     return CCI_ERROR_INVALID_ARG;

  dm_uint32 entryCount = GDALGetColorEntryCount(mColorTable);
  if(count > entryCount)
     count = entryCount;

  const GDALColorEntry* entry;
  for(dm_uint32 i=0;i<count;++i)
  {
    entry = GDALGetColorEntry(mColorTable,i);
    (*data)[i] = DM_ARGB(entry->c4,entry->c1,entry->c2,entry->c3);
  }

  return CCI_OK;
}

/* cciIColorTable clone (); */
CCI_IMETHODIMP gdalColorTable::Clone(cciIColorTable * *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(_retval);

  GDALColorTableH hCloned = GDALCloneColorTable(mColorTable);
  CCI_ENSURE_TRUE(hCloned,CCI_ERROR_OUT_OF_MEMORY);

  gdalColorTable* cloned = new gdalColorTable(hCloned);
  CCI_ENSURE_TRUE(cloned,CCI_ERROR_OUT_OF_MEMORY);

  CCI_ADDREF(*_retval = cloned);
  return CCI_OK;
}


//=====================================
// Utilities
//=====================================
cci_result CCI_NewGDALColorTable(GDALColorTableH aColorTable, cciIColorTable** result)
{
  CCI_ENSURE_ARG_POINTER(result);
  CCI_ENSURE_ARG_POINTER(aColorTable);

  gdalColorTable* colorTable = new gdalColorTable(aColorTable);
  CCI_ENSURE_TRUE(colorTable,CCI_ERROR_OUT_OF_MEMORY);

  CCI_ADDREF(*result = colorTable);
  return CCI_OK;
}



