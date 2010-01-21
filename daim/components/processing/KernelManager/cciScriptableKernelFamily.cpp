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

#include "cciScriptableKernelFamily.h"

//-----------------------------------------------
#define _TESTCODE( s, t, n )  if(s == t) return n
//-----------------------------------------------
int static __GetOperator( const dmACString& s )
{
  if(!s.IsEmpty()) {
   _TESTCODE(s,"ADD" ,dmTk::Math::AddPixels);
   _TESTCODE(s,"SUB" ,dmTk::Math::SubPixels);
   _TESTCODE(s,"OR"  ,dmTk::Math::OrPixels);
   _TESTCODE(s,"XOR" ,dmTk::Math::XorPixels);
   _TESTCODE(s,"AND" ,dmTk::Math::AndPixels);
   _TESTCODE(s,"MIN" ,dmTk::Math::MinPixels);
   _TESTCODE(s,"MAX" ,dmTk::Math::MaxPixels);
   _TESTCODE(s,"NSUB",dmTk::Math::NSubPixels);
   _TESTCODE(s,"DIFF",dmTk::Math::DiffPixels);
   _TESTCODE(s,"ABS" ,dmTk::Math::AbsPixels);
  }
  return dmTk::None;
};


//-----------------------------------------------
// cciScriptableKernelFamily implementation
//-----------------------------------------------

CCI_IMPL_ISUPPORTS3(cciScriptableKernelFamily,
                    cciIScriptableKernelFamily,
                    cciIKernelFamily,
                    cciIKernelFamilyContainer)

cciScriptableKernelFamily::cciScriptableKernelFamily()
{
  kX = kY = kWidth = kHeight = 0;
}

cciScriptableKernelFamily::~cciScriptableKernelFamily()
{

}

//=================================
// cciIScriptableKernelFamily
//=================================

/* void newDescription (in long x, in long y, in long width, in long height, in string mode); */
CCI_IMETHODIMP cciScriptableKernelFamily::NewDescription(dm_int32 x, dm_int32 y, dm_int32 width, dm_int32 height, const char * mode)
{
  kX = kY = kWidth = kHeight = 0;
  kX               = x;
  kY               = y;
  kWidth           = width;
  kHeight          = height;

  if(kWidth <= 0 || kHeight <= 0 )
     return CCI_ERROR_INVALID_ARG;

  mMode.Assign(mode);

  mFamily.Clear();
  mFamily.SetMode(__GetOperator(mMode));
  return CCI_OK;
}

/* void addKernel (in string data); */
CCI_IMETHODIMP cciScriptableKernelFamily::AddKernel(const char * data)
{
  return AddKernelDescription(data,kX,kY,kWidth,kHeight);
}


/* void addKernelDescription (in string data, in long x, in long y, in long width, in long height); */
CCI_IMETHODIMP cciScriptableKernelFamily::AddKernelDescription(const char * data, dm_int32 x, dm_int32 y, dm_int32 width, dm_int32 height)
{
  CCI_ENSURE_TRUE(data && data[0]!='\0',CCI_ERROR_INVALID_ARG);

  rElement.SetDescription(x,y,width  > 0 ? width  : kWidth,
                              height > 0 ? height : kHeight);

  if(rElement.Width() <= 0 || rElement.Height() <= 0 )
     return CCI_ERROR_INVALID_ARG;

  cci_result rv;

  dmCString val,sc(data);
  sc.ReplaceChars("\t\n,;",' '); // remove all white spaces
  int* pData = (int*)rElement.Data();
  size_t i,pos;

  for(i=0,pos = 0;pos!=dmCString::npos && i<rElement.Size();++i)
  {
    pos = sc.Split(val,pos,NULL);
    pData[i] = val.ToInt();
  }

  if(i!=rElement.Size())
  {
    dmTRACE("Incomplete kernel element...skipping");
    rv = CCI_ERROR_FAILURE;
  }
  else
  {
    #ifdef _DEBUG
      dmLOG("Adding Kernel element to family\n");
      dmLOG("x=%ld y=%ld w=%ld h=%ld\n",
          rElement.Ox(),rElement.Oy(),
          rElement.Width(),rElement.Height());

      size_t i;
      for(i=0;i<rElement.Size();++i)
      {
        dmLOG("\t%d",rElement[i]);
        if(((i+1)%rElement.Height())==0) dmLOG("\n");
      }
    #endif // _DEBUG

    mFamily.Add(rElement);
    rv = CCI_OK;
  }

  return rv;

}

/* void clear (); */
CCI_IMETHODIMP cciScriptableKernelFamily::Clear()
{
  mFamily.Clear();
  return CCI_OK;
}

/* cciIKernelFamily clone (); */
CCI_IMETHODIMP cciScriptableKernelFamily::Clone(cciIKernelFamily * *_retval CCI_OUTPARAM)
{
  cciScriptableKernelFamily* newFamily = new cciScriptableKernelFamily();

  newFamily->mFamily = mFamily;

  CCI_ADDREF(*_retval = newFamily);
  return CCI_OK;
}


//-----------------------------------------------
// cciIKernelFamily implementation
//-----------------------------------------------

/* [noscript] void getMaskDescription (in dmMaskDescriptionRef aMask); */
CCI_IMETHODIMP cciScriptableKernelFamily::GetMaskDescription(dmMaskDescription & aMask)
{
  mFamily.GetMaskDescription(aMask);
  return CCI_OK;
}

//-----------------------------------------------
// cciKernelFamilyContainer implementation
//-----------------------------------------------

/* [noscript,notxpcom] dmKernelFamilyPtr family (); */
CCI_IMETHODIMP_(dmKernelFamily *) cciScriptableKernelFamily::GetNative()
{
  return &mFamily;
}

