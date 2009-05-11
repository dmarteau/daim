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
#include "cciInterfaceRequestorUtils.h"
#include "cciIFilterContext.h"
#include "cciRFilter.h"
#include "cciIRegionContainer.h"
#include "cciIImageContainer.h"

#include "common/dmUserLib.h"

/* Implementation file */
CCI_IMPL_ISUPPORTS2(cciRFilter, cciIRFilter,
                                cciIImageFilter)


cciRFilter::cciRFilter()
: mRank(0)
, mMask(3,3)
{
  /* member initializers and constructor code */
}

cciRFilter::~cciRFilter()
{
  /* destructor code */
}

//=========================================
// cciRFilter implementation
//=========================================

/* attribute unsigned long rank; */
CCI_IMETHODIMP cciRFilter::GetRank(dm_int32 *aRank)
{
  *aRank = mRank;
  return CCI_OK;
}
CCI_IMETHODIMP cciRFilter::SetRank(dm_int32 aRank)
{
  mRank = aRank;
  return CCI_OK;
}


cci_result cciRFilter::ApplyMFilter(dmImage* image,dmRegion* rgn,long type,
                                    dmMaskDescription& mask,
                                    dmImageBuffer* buffer)
{
  dmFilterType _ftype;
  switch(type) {
    case Filter_Min   : _ftype = dmFilterMin    ; break;
    case Filter_Max   : _ftype = dmFilterMax    ; break;
    case Filter_Middle: _ftype = dmFilterMiddle ; break;
    case Filter_Mean  : _ftype = dmFilterMean   ; break;
    case Filter_Median: _ftype = dmFilterMedian ; break;

    default :
       dmLOG_ERROR("Invalid filter type !");
       return CCI_ERROR_ILLEGAL_VALUE;
   }

  dmMFilter _MFilter(mask,_ftype);

  return dmApplyFilter(_MFilter,*buffer,*image,*rgn,true)?
         CCI_OK : CCI_ERROR_FAILURE;
}

cci_result cciRFilter::ApplyRFilter(dmImage* image,dmRegion* rgn,dm_uint32 rank,
                                    dmMaskDescription& mask,
                                    dmImageBuffer* buffer)
{
  if(rank>=mask.Size())
    return CCI_ERROR_ILLEGAL_VALUE;

  dmRFilter _Filter(mask,rank);

  return dmApplyFilter(_Filter,*buffer,*image,*rgn,true)?
           CCI_OK : CCI_ERROR_FAILURE;
}

//=========================================
// cciIRFilter
//=========================================


/* [noscript] void doFilter (in cciImage image, in cciRegion rgn, in long rank, in cciMaskDescription mask, cciIFilterContext filterCtxt); */
CCI_IMETHODIMP cciRFilter::DoFilter(dmImage *image, dmRegion *rgn, dm_int32 rank,
                                    dmMaskDescription & mask,
                                    cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);
  CCI_ENSURE_ARG_POINTER(filterCtxt);

  dmRegion roi = rgn ? *rgn : image->Rect();

  if(rank<0)
     return ApplyMFilter(image,&roi,rank,mask,filterCtxt->NativeBuffer());
  else
     return ApplyRFilter(image,&roi,rank,mask,filterCtxt->NativeBuffer());
}

//=========================================
// cciIImageFilter
//=========================================

/* attribute cciMaskDescription mask; */
CCI_IMETHODIMP cciRFilter::GetMask(dmMaskDescription& aMask)
{
  aMask = mMask;
  return CCI_OK;
}
CCI_IMETHODIMP cciRFilter::SetMask(dmMaskDescription& aMask)
{
  mMask = aMask;
  return CCI_OK;
}

/* void applyFilter (in cciImage image, in cciRegion roi, in cciIFilterContext filterCtxt); */
CCI_IMETHODIMP cciRFilter::ApplyFilter(cciImage image, cciRegion roi, cciIFilterContext *filterCtxt)
{
  CCI_ENSURE_ARG_POINTER(image);

  dmRegion* nativeRgn = roi ? CCI_NATIVE(roi) : dm_null;

  return DoFilter(CCI_NATIVE(image),nativeRgn,mRank,mMask,filterCtxt);
}
