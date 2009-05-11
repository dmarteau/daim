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
// File         : cciKernelService.cpp
// Date         : 19 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "cciKernelService.h"
#include "cciIKernelFamily.h"
#include "dmCrt.h"

//------------------------------------------------

struct kernelEntry
{
  dm_int32    x;
  dm_int32    y;
  dm_uint32   width;
  dm_uint32   height;
  dmKernelDescription::value_type          norm;
  const dmKernelDescription::value_type   *data;
};

#define BEGIN_KERNEL(fname,kname)            \
static const dmKernelDescription::value_type \
fname##_##kname##_data[] = {

#define END_KERNEL \
};


#define KERNEL_ENTRY(fname,kname,_x,_y,_width,_height,norm) \
  { _x,_y,_width,_height,norm,fname##_##kname##_data },

#define BEGIN_FAMILY(fname) \
static const kernelEntry fname##_kernels[] = {

#define END_FAMILY \
};

#define ADD_FAMILY_ENTRY(group,family,name,mode) \
  AddFamilyEntry(group::family##_kernels,DM_ARRAY_LENGTH(group::family##_kernels),name,mode)

#include "kernels/Edges.h"
#include "kernels/LowPass.h"
#include "kernels/Morpho4c.h"
#include "kernels/Morpho8c.h"


//-----------------------------------------------
#define _TESTCODE( s, t, n )  if(strcmp(s,t)==0) return n
//-----------------------------------------------
int static __GetOperator( const char* s )
{
  if(s) {
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
//------------------------------------------------


/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciKernelService, cciIKernelService)

cciKernelService::cciKernelService()
{
  LoadStaticKernels();
}

cciKernelService::~cciKernelService()
{
  Shutdown();
}

void cciKernelService::AddFamilyEntry( const kernelEntry* entries, size_t size,
                                       const char* name,
                                       const char* mode )
{
  #ifdef DEBUG
    dmLOG("Kernel service : adding family entry %s",name);
  #endif

  dmKernelFamily* family = new dmKernelFamily();
  dmKernelDescription element;

  family->SetMode(__GetOperator(mode));

  const kernelEntry* entry = entries;
  for(unsigned int i=0;i<size;++i)
  {
    element.SetDescription(entry->x,
                           entry->y,
                           entry->width,
                           entry->height,
                           entry->data);

    element.SetNorm(entry->norm);
    family->Add(element);

    ++entry;
  }

  mFamilies[dmCString(name)] = family;
}

void cciKernelService::LoadStaticKernels()
{
  ADD_FAMILY_ENTRY(LowPass,Gauss_8__Isotrope,"LowPass/Gauss(8)-Isotrope",dm_null);
  ADD_FAMILY_ENTRY(LowPass,Gauss_8,"LowPass/Gauss(8)",dm_null);
  ADD_FAMILY_ENTRY(LowPass,Gauss_4,"LowPass/Gauss(4)",dm_null);
  ADD_FAMILY_ENTRY(LowPass,Gauss5x5,"LowPass/Gauss5x5",dm_null);
  ADD_FAMILY_ENTRY(LowPass,Gauss5x5_Separable,"LowPass/Gauss5x5-Separable",dm_null);

  ADD_FAMILY_ENTRY(Edges,Sobel,"Edges/Sobel","ABS");
  ADD_FAMILY_ENTRY(Edges,Sobel_X,"Edges/Sobel-X",dm_null);
  ADD_FAMILY_ENTRY(Edges,Sobel_Y,"Edges/Sobel-Y",dm_null);
  ADD_FAMILY_ENTRY(Edges,Harris,"Edges/Harris","ABS");
  ADD_FAMILY_ENTRY(Edges,Prewitt,"Edges/Prewitt","ABS");
  ADD_FAMILY_ENTRY(Edges,Robinson,"Edges/Robinson","ABS");
  ADD_FAMILY_ENTRY(Edges,Kirsh,"Edges/Kirsh","ABS");
  ADD_FAMILY_ENTRY(Edges,Gradient,"Edges/Gradient","ABS");
  ADD_FAMILY_ENTRY(Edges,Laplace_8,"Edges/Laplace(8)",dm_null);
  ADD_FAMILY_ENTRY(Edges,Laplace_4,"Edges/Laplace(4)",dm_null);
  ADD_FAMILY_ENTRY(Edges,Mexican_Hat,"Edges/Mexican Hat",dm_null);

  ADD_FAMILY_ENTRY(Edges,x_backward_diff,"Edges/x-backward-diff",dm_null);
  ADD_FAMILY_ENTRY(Edges,y_backward_diff,"Edges/y-backward-diff",dm_null);
  ADD_FAMILY_ENTRY(Edges,x_forward_diff ,"Edges/x-forward-diff",dm_null);
  ADD_FAMILY_ENTRY(Edges,y_forward_diff ,"Edges/y-forward-diff",dm_null);
  ADD_FAMILY_ENTRY(Edges,x_centered_diff,"Edges/x-centered-diff",dm_null);
  ADD_FAMILY_ENTRY(Edges,y_centered_diff,"Edges/y-centered-diff",dm_null);

  ADD_FAMILY_ENTRY(Morpho8c,L_8,"Morpho8c/L(8)",dm_null);
  ADD_FAMILY_ENTRY(Morpho8c,D_8,"Morpho8c/D(8)",dm_null);
  ADD_FAMILY_ENTRY(Morpho8c,C_8,"Morpho8c/C(8)",dm_null);
  ADD_FAMILY_ENTRY(Morpho8c,E_8,"Morpho8c/E(8)",dm_null);
  ADD_FAMILY_ENTRY(Morpho8c,E_4,"Morpho8c/E(4)",dm_null);
  ADD_FAMILY_ENTRY(Morpho8c,I_8,"Morpho8c/I(8)",dm_null);
  ADD_FAMILY_ENTRY(Morpho8c,H_8,"Morpho8c/H(8)",dm_null);
  ADD_FAMILY_ENTRY(Morpho8c,B_8,"Morpho8c/B(8)",dm_null);

  ADD_FAMILY_ENTRY(Morpho4c,L_4,"Morpho4c/L(4)",dm_null);
  ADD_FAMILY_ENTRY(Morpho4c,I_4,"Morpho4c/I(4)",dm_null);
  ADD_FAMILY_ENTRY(Morpho4c,E_4,"Morpho4c/E(4)",dm_null);
  ADD_FAMILY_ENTRY(Morpho4c,H_4,"Morpho4c/H(4)",dm_null);
}


void cciKernelService::Shutdown()
{
  // Clear factory entries
  dmKernelDict::iterator it  = mFamilies.begin();
  dmKernelDict::iterator end = mFamilies.end();
  while(it!=end)
  {
    dmKernelFamily* fm = (*it++).second;
    if(fm)
       delete fm;
  }

  mFamilies.clear();
}


class cciKernelFamilyImpl : public cciIKernelFamily
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IKERNELFAMILYCONTAINER
  CCI_DECL_IKERNELFAMILY

  cciKernelFamilyImpl( const dmKernelFamily& aFamily )
  : mFamily(aFamily) {}

private:
  ~cciKernelFamilyImpl() {}

protected:
   dmKernelFamily mFamily;
};

/* Implementation file */
CCI_IMPL_ISUPPORTS2(cciKernelFamilyImpl, cciIKernelFamily, cciIKernelFamilyContainer)

/* readonly attribute cciMaskDescription mask; */
CCI_IMETHODIMP cciKernelFamilyImpl::GetMask(dmMaskDescription & aMask)
{
  return CCI_ERROR_NOT_IMPLEMENTED;
}

/* [noscript,notxpcom] dmKernelFamilyPtr getNative (); */
CCI_IMETHODIMP_(dmKernelFamily *) cciKernelFamilyImpl::GetNative()
{
  return &mFamily;
}


/* cciKernelFamily GetFamily (in string name); */
CCI_IMETHODIMP cciKernelService::GetFamily(const char * name, cciKernelFamily* _retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(_retval);

  dmKernelFamily* family = GetNativeFamily(name);
  if(family)
  {
    *_retval = new cciKernelFamilyImpl(*family);
    if(!*_retval)
       return CCI_ERROR_OUT_OF_MEMORY;

    CCI_ADDREF(*_retval);
    return CCI_OK;
  }
  return CCI_ERROR_NOT_AVAILABLE;
}

/* void registerFamily (in string name, in cciKernelFamily family); */
CCI_IMETHODIMP cciKernelService::RegisterFamily(const char * name, cciKernelFamily family)
{
  CCI_ENSURE_ARG_POINTER(family);

  dmKernelFamily* fm = new dmKernelFamily(*CCI_NATIVE(family));
  mFamilies[dmCString(name)] = fm;
  return CCI_OK;
}

/* void unregisterFamily (in string name); */
CCI_IMETHODIMP cciKernelService::UnregisterFamily(const char * name)
{
  dmKernelFamily* family = GetNativeFamily(name);
  if(family) {
    mFamilies.erase(dmCString(name));
    delete family;
  }
  return CCI_OK;
}

/* [noscript,notxpcom] dmKernelFamilyPtr GetNativeFamily (in string name); */
CCI_IMETHODIMP_(dmKernelFamily *) cciKernelService::GetNativeFamily(const char * name)
{
  dmKernelDict::iterator it = mFamilies.find(dmCString(name));
  if(it!=mFamilies.end())
     return (*it).second;

  return dm_null;
}

/* [noscript] dmKernelFamilyRef getCopyOfNativeFamily (in string name); */
CCI_IMETHODIMP cciKernelService::GetCopyOfNativeFamily(const char * name, dmKernelFamily & _retval CCI_OUTPARAM)
{
  dmKernelFamily* family = GetNativeFamily(name);
  if(family)
  {
    _retval = *family;
    return CCI_OK;
  }
  return CCI_ERROR_NOT_AVAILABLE;

}


/* [noscript] cciKernelFamily NewFamilyFromNative (in dmKernelFamilyRef aNativefamily); */
CCI_IMETHODIMP cciKernelService::NewFamilyFromNative(dmKernelFamily & aNativefamily, cciKernelFamily *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(_retval);

  *_retval = new cciKernelFamilyImpl(aNativefamily);
  if(!*_retval)
     return CCI_ERROR_OUT_OF_MEMORY;

  CCI_ADDREF(*_retval);
  return CCI_OK;
}

