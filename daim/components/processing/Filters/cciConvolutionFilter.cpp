/*
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
#
# The Original Code is Daim code.
#
# The Initial Developer of the Original Code is
#   David Marteau
# Portions created by the Initial Developer are Copyright (C) 2008
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#   David Marteau (dhmarteau@gmail.com)
#
# Alternatively, the contents of this file may be used under the terms of
# either of the GNU General Public License Version 2 or later (the "GPL"),
# or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
# in which case the provisions of the GPL or the LGPL are applicable instead
# of those above. If you wish to allow use of your version of this file only
# under the terms of either the GPL or the LGPL, and not to allow others to
# use your version of this file under the terms of the MPL, indicate your
# decision by deleting the provisions above and replace them with the notice
# and other provisions required by the GPL or the LGPL. If you do not delete
# the provisions above, a recipient may use your version of this file under
# the terms of any one of the MPL, the GPL or the LGPL.
#
# ***** END LICENSE BLOCK *****
*/


#include "cciCOMPtr.h"
#include "cciConvolutionFilter.h"
#include "cciInterfaceRequestorUtils.h"
#include "cciIFilterContext.h"
#include "cciIKernelService.h"
#include "cciIKernelFamily.h"
#include "cciIRegionContainer.h"
#include "cciIImageContainer.h"

#include "common/dmUserLib.h"

/* Implementation file */
CCI_IMPL_ISUPPORTS2(cciConvolutionFilter, cciIConvolutionFilter,
                                          cciIImageFilter)
//=========================================
// cciConvolutionFilter
//=========================================

cciConvolutionFilter::cciConvolutionFilter()
: mOperation(0)
, mIterations(1)
{
  /* member initializers and constructor code */
}

cciConvolutionFilter::~cciConvolutionFilter()
{
  /* destructor code */
}

//=========================================
// cciIConvolutionFilter
//=========================================

/* attribute cciKernelFamily kernels; */
CCI_IMETHODIMP cciConvolutionFilter::GetKernels(cciKernelFamily* aKernels)
{
  return CCI_NewKernelFamily(mFamily,aKernels);
}
CCI_IMETHODIMP cciConvolutionFilter::SetKernels(cciKernelFamily aKernels)
{
  mFamily = *CCI_NATIVE(aKernels);
  return CCI_OK;
}

/* attribute unsigned long iterations; */
CCI_IMETHODIMP cciConvolutionFilter::GetIterations(dm_uint32 *aIterations)
{
  *aIterations = mIterations;
  return CCI_OK;
}
CCI_IMETHODIMP cciConvolutionFilter::SetIterations(dm_uint32 aIterations)
{
  mIterations = aIterations;
  return CCI_OK;
}

/* attribute unsigned long operation; */
CCI_IMETHODIMP cciConvolutionFilter::GetOperation(dm_uint32 *aOperation)
{
  *aOperation = mOperation;
  return CCI_OK;
}
CCI_IMETHODIMP cciConvolutionFilter::SetOperation(dm_uint32 aOperation)
{
  mOperation = aOperation;
  return CCI_OK;
}

//---------------------------------------------------------------------
// Method DoConvolution
//---------------------------------------------------------------------
// Defined in daim_kernel library
bool __dmKernel _dmDoImageMath2( dmBufferParameters& , dm_int );
//---------------------------------------------------------------------
class dmConvolutionOperator : public dmConvolution
{
public:
  int _Math;

  dmConvolutionOperator(  dmKernelFamily& aFamily, int Iter, int Math )
  : dmConvolution(aFamily,Iter),_Math(Math) {}

  bool Apply( dmBufferParameters& params ) {
    if(dmConvolution::Apply(params)) {
      if(_Math) _dmDoImageMath2(params,_Math);
      return true;
    }
    return false;
  }
};
//---------------------------------------------------------------------
/* [noscript] void doConvolution (in dmImagePtr image, in dmRegionPtr rgn, in dmKernelFamilyRef family, in unsigned long iter, in unsigned long oper, in cciIFilterContext filterCtxt); */
CCI_IMETHODIMP cciConvolutionFilter::DoConvolution(dmImage *image, dmRegion *rgn,
                                                   dmKernelFamily & family,
                                                   dm_uint32 iter,
                                                   dm_uint32 oper,
                                                   cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  dmRegion roi = rgn ? *rgn : image->Rect();
  dmConvolutionOperator _Convolution(family,iter,oper);

  return dmApplyFilter(_Convolution,*filterCtxt->NativeBuffer(),*image,roi,true)?
         CCI_OK : CCI_ERROR_FAILURE;
}


/* void loadKernel (in string name); */
CCI_IMETHODIMP cciConvolutionFilter::LoadKernel(const char * name)
{
  CCI_ENSURE_ARG_POINTER(name);
  return CCI_GetKernelFamily(name,mFamily);
}

//=========================================
// cciIImageFilter
//=========================================

/* attribute cciMaskDescription mask; */
CCI_IMETHODIMP cciConvolutionFilter::GetMask(dmMaskDescription& aMask)
{
  mFamily.GetMaskDescription(aMask);
  return CCI_OK;
}
CCI_IMETHODIMP cciConvolutionFilter::SetMask(dmMaskDescription& aMask)
{
  return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void applyFilter (in cciImage image, in cciRegion roi, in cciIFilterContext filterCtxt); */
CCI_IMETHODIMP cciConvolutionFilter::ApplyFilter(cciImage image, cciRegion roi, cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);

  dmRegion* nativeRgn = roi ? CCI_NATIVE(roi) : dm_null;

  return DoConvolution(CCI_NATIVE(image),nativeRgn,mFamily,mIterations,mOperation,filterCtxt);
}
