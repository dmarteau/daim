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

#define dmUseKernelImageTemplates
#include "cciCOMPtr.h"
#include "cciLFilter.h"
#include "cciInterfaceRequestorUtils.h"
#include "cciIFilterContext.h"
#include "cciIRegionContainer.h"
#include "cciIImageContainer.h"

#include "common/dmUserLib.h"

/* Implementation file */
CCI_IMPL_ISUPPORTS2(cciLFilter, cciILFilter,
                                cciIImageFilter)


cciLFilter::cciLFilter()
{
  /* member initializers and constructor code */
}

cciLFilter::~cciLFilter()
{
  /* destructor code */
}

//=========================================
// cciILFilter
//=========================================

/* attribute cciIMatrix coeffs; */
CCI_IMETHODIMP cciLFilter::GetCoeffs(cciIMatrix * *aCoeffs)
{
  CCI_ENSURE_ARG_POINTER(aCoeffs);

  CCI_ADDREF(*aCoeffs = mCoeffs);
  return CCI_OK;
}
CCI_IMETHODIMP cciLFilter::SetCoeffs(cciIMatrix *aCoeffs)
{
  mCoeffs = aCoeffs;
  if(mCoeffs) {
     mMask = dmMaskDescription(mCoeffs->Columns(),mCoeffs->Rows());
  } else
     mMask = dmMaskDescription();

  return CCI_OK;
}

/* void setMaskOrigin (in dm_uint32 x, in dm_uint32 y); */
CCI_IMETHODIMP cciLFilter::SetMaskOrigin(dm_uint32 x, dm_uint32 y)
{
  if(y > mCoeffs->Rows() || x > mCoeffs->Columns())
     return CCI_ERROR_INVALID_ARG;

  mMask.SetOrigin(x,y);
  return CCI_OK;
}

/* [noscript] void doFilter (in dmImagePtr image, in dmRegionPtr rgn, in cciIMatrix coeffs, in dmMaskDescriptionRef mask, in cciISupports context); */
CCI_IMETHODIMP cciLFilter::DoFilter(dmImage *image, dmRegion *rgn,
                                    cciIMatrix *coeffs,
                                    dmMaskDescription & mask,
                                    cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(coeffs);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  dmRegion roi = rgn ? *rgn : image->Rect();

  if(coeffs->Size() < mask.Size())
     return CCI_ERROR_ILLEGAL_VALUE;

  dmLFilter _Filter(mask,coeffs->Data());

  return dmApplyFilter(_Filter,*filterCtxt->NativeBuffer(),*image,roi,true)?
         CCI_OK : CCI_ERROR_FAILURE;
}

//=========================================
// cciIImageFilter
//=========================================

/* attribute cciMaskDescription mask; */
CCI_IMETHODIMP cciLFilter::GetMask(dmMaskDescription& aMask)
{
  aMask = mMask;
  return CCI_OK;
}
CCI_IMETHODIMP cciLFilter::SetMask(dmMaskDescription& aMask)
{
  return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void applyFilter (in cciImage image, in cciRegion roi, in cciISupports context); */
CCI_IMETHODIMP cciLFilter::ApplyFilter(cciImage image, cciRegion roi, cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);

  dmRegion* nativeRgn = roi ? CCI_NATIVE(roi) : dm_null;

  return DoFilter(CCI_NATIVE(image),nativeRgn,mCoeffs,mMask,filterCtxt);
}
