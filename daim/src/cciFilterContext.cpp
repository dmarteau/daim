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

#define dmUseKernelImage
#include "cciFilterContext.h"
#include "cciIImageContainer.h"
#include "cciIRegionContainer.h"
#include "cciImageUtils.h"

/* Implementation file */
CCI_IMPL_ISUPPORTS2(cciFilterContext, cciIFilterContext,cciIInterfaceRequestor)

cciFilterContext::cciFilterContext()
: mMinRange(0)
, mMaxRange(0)
{
  mBuffer = dmAutoLink<dmImageBuffer>(new dmImageBuffer);
}

cciFilterContext::~cciFilterContext()
{
}

/* void createBuffer (in cciImage image, in cciRegion rgn, in dm_uint32 padding, in dm_int32 padding_mode); */
CCI_IMETHODIMP cciFilterContext::CreateBuffer(cciImage image, cciRegion rgn,
                                              dm_uint32 padX, dm_uint32 padY,
                                              dm_int32 limit_type)
{
  CCI_ENSURE_ARG_POINTER(image);

  dmImage* nativeImage = image->GetNative();
  CCI_PRECONDITION(nativeImage,"null image");

  dmRegion roi = rgn ? *rgn->GetNative() : nativeImage->Rect();
  if(padX > 0||padY > 0)
  {
    dmMaskDescription  mask(padX,padY);
    mBuffer->CreateBuffer(*nativeImage,roi,mask);
    mBuffer->SetBufferLimit(nativeImage->Rect(),static_cast<dmEBndryLimitType>(limit_type));
  } else
    mBuffer->CreateBuffer(*nativeImage,roi);

  return CCI_OK;
}

/* void restoreBuffer (in cciImage image); */
CCI_IMETHODIMP cciFilterContext::RestoreBuffer(cciImage image)
{
  CCI_ENSURE_ARG_POINTER(image);
  mBuffer->BufferToImage(*image->GetNative());
  return CCI_OK;
}

/* void clearBuffer (); */
CCI_IMETHODIMP cciFilterContext::ClearBuffer()
{
  mBuffer->Clear();
  return CCI_OK;
}

/* [noscript,notxpcom] dmImageBufferPtr nativeBuffer (); */
CCI_IMETHODIMP_(dmImageBuffer *) cciFilterContext::NativeBuffer()
{
  return mBuffer;
}


/* attribute dm_real minRange; */
CCI_IMETHODIMP cciFilterContext::GetMinRange(dm_real *aMinRange)
{
  *aMinRange = mMinRange;
  return CCI_OK;
}

/* attribute dm_real maxRange; */
CCI_IMETHODIMP cciFilterContext::GetMaxRange(dm_real *aMaxRange)
{
  *aMaxRange = mMaxRange;
  return CCI_OK;
}

/* void setRange (in dm_real rmin, in dm_real rmax); */
CCI_IMETHODIMP cciFilterContext::SetRange(dm_real rmin, dm_real rmax)
{
  mMinRange = rmin;
  mMaxRange = rmax;
  return CCI_OK;
}


//=========================================
// cciIInterfaceRequestor
//=========================================

/* void getInterface (in dmIIDRef uuid, [iid_is (uuid), retval] out nsQIResult result); */
CCI_IMETHODIMP cciFilterContext::GetInterface(const dmIID & uuid, void * *result)
{
  return QueryInterface(uuid, result);;
}


CCI_COM_GLUE
cci_result CCI_NewFilterContext( cciIFilterContext** _result )
{
  cciFilterContext* context = new cciFilterContext();
  if(!context)
     return CCI_ERROR_OUT_OF_MEMORY;

  CCI_ADDREF(*_result = context);

  return CCI_OK;
}




