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
#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"
#include "cciImageUtils.h"
#include "cciIStorageImage.h"
#include "cciISurface.h"
#include "cciIColorTable.h"
#include "cciIMetaDataContainer.h"
#include "cciISurfaceDriver.h"
#include "cciDaimCID.h"

#include <string.h> // for strncat

#define LOADER_SERVICE_CONTRACTID \
  "@daim.org/contrib/loader-service;1"

#ifdef DAIM_GLUE
CCI_USE_NS

DAIM_GLUE_EXPORT
cci_result CCI_NewImageList( cciIImageList** _result )
{
  cci_result rv;
  cci_Ptr<cciIImageList> imagelist = do_CreateInstance(CCI_IMAGELIST_CONTRACTID,&rv);
  if(CCI_SUCCEEDED(rv))
    imagelist.forget(_result);
 
  return rv;
}

DAIM_GLUE_EXPORT 
cci_result CCI_NewImage(dm_uint32 width, dm_uint32 height, EPixelFormat format, cciIImage* *_result)
{
  cci_result rv;
  cci_Ptr<cciIStorageImage> image = do_CreateInstance(CCI_STORAGEIMAGE_CONTRACTID,&rv);
  CCI_ENSURE_SUCCESS(rv,rv);
  
  rv = image->Create(width,height,format);
  if(CCI_SUCCEEDED(rv))
     CCI_ADDREF(*_result = image);
 
  return rv;
}

DAIM_GLUE_EXPORT 
cci_result CCI_NewImage(dmImageData & data, cciIImage* *_result)
{
  cci_result rv;
  cci_Ptr<cciIStorageImage> image = do_CreateInstance(CCI_STORAGEIMAGE_CONTRACTID,&rv);
  CCI_ENSURE_SUCCESS(rv,rv);
  
  rv = image->ShareImageData(data);
  if(CCI_SUCCEEDED(rv))
     CCI_ADDREF(*_result = image);

  return rv;
}


#else // !DAIM_GLUE

#include "cciImageList.h"
#include "cciScriptableImage.h"

DAIM_GLUE_EXPORT
cci_result CCI_NewImageList( cciIImageList** _result )
{
  cciIImageList* newList = new cciImageList();
  CCI_ADDREF(*_result = newList);
  return CCI_OK;
}

DAIM_GLUE_EXPORT 
cci_result CCI_NewImage(dm_uint32 width, dm_uint32 height, EPixelFormat format, cciIImage* *_result)
{
  cciScriptableImage* newImage = new cciScriptableImage(width,height,format);
  CCI_IF_ADDREF(newImage);
  
  if(newImage && newImage->IsValid()) 
  {
    *_result = newImage;
    return CCI_OK;
  }
  
  CCI_IF_RELEASE(newImage);
  return CCI_ERROR_OUT_OF_MEMORY;
}

DAIM_GLUE_EXPORT 
cci_result CCI_NewImage(dmImageData & data, cciIImage* *_result)
{
  cciScriptableImage* newImage = new cciScriptableImage(data);
  CCI_IF_ADDREF(newImage);
  
  if(newImage && newImage->IsValid()) 
  {
    *_result = newImage;
    return CCI_OK;
  }
  
  CCI_IF_RELEASE(newImage);
  return CCI_ERROR_OUT_OF_MEMORY;
}

#endif // DAIM_GLUE

#define COLORSPACE_MAX_CHAR 10

DAIM_GLUE_EXPORT
cci_result CCI_NewColorSpace( const char* colorSpace, cciIColorSpace** _result )
{
  char
  contractid[sizeof(CCI_COLORSPACE_CONTRACTID_)+COLORSPACE_MAX_CHAR+1] = CCI_COLORSPACE_CONTRACTID_;

  strncat(contractid,colorSpace,COLORSPACE_MAX_CHAR);

  cci_result rv;
  cci_Ptr<cciIColorSpace> buffer = do_CreateInstance(contractid,&rv);
  if(CCI_SUCCEEDED(rv))
     buffer.forget(_result);

  return rv;
}

DAIM_GLUE_EXPORT
cci_result CCI_GetLoader( cciILoaderService* srvc, const char* type, bool createCaps, cciISurfaceDriver** _result )
{
  cci_result rv;
  cci_Ptr<cciILoaderService> ldrSrvc = srvc; 
  if(!ldrSrvc)
  {
    ldrSrvc = do_GetService(LOADER_SERVICE_CONTRACTID,&rv);
    if(CCI_FAILED(rv))
       return rv;
  }
  
  // Get the corresponding driver
  cci_Ptr<cciISurfaceDriver>  loader;
  rv = ldrSrvc->GetDriver(type,createCaps,getter_AddRefs(loader));
  if(CCI_SUCCEEDED(rv))
     loader.forget(_result);
  
  return rv;
}

DAIM_GLUE_EXPORT
cci_result CCI_SaveImage( cciISurfaceDriver* loader, const char* path, cciIImage* image, const char* options  )
{  
  cci_result rv;
  
  dmImageData imData;
  
  rv = image->Lock(imData,dm_null);
  if(CCI_FAILED(rv))
     return rv;
  
  cci_Ptr<cciIColorTable> colorTable;
  image->GetColorTable(getter_AddRefs(colorTable));
  
  rv = loader->SaveImageBits(path,imData,dm_null,colorTable,options);
  image->Unlock();
  
  return rv;
}

DAIM_GLUE_EXPORT
cci_result CCI_LoadImage( cciILoaderService* srvc, const char* path, cciIImage* *_result)
{
  cci_result rv;
  cci_Ptr<cciILoaderService> ldrSrvc = srvc; 
  if(!ldrSrvc)
  {
    ldrSrvc = do_GetService(LOADER_SERVICE_CONTRACTID,&rv);
    if(CCI_FAILED(rv))
       return rv;
  }

  cci_Ptr<cciISurface> surface;

  rv = ldrSrvc->OpenSurface(path,cciIDriverProxy::IO_READONLY,getter_AddRefs(surface));
  if(CCI_FAILED(rv))
     return rv;

  rv = CCI_NewImage(surface->Width(),surface->Height(),surface->PixelFormat(),_result);
  if(CCI_FAILED(rv))
     return rv;
  
  rv = (*_result)->LoadSurfaceBits(surface,dm_null);

  if(CCI_SUCCEEDED(rv)) 
  {
    cci_Ptr<cciIColorTable> colorTable;

    // Try to get a possible color table
    surface->GetColorTable(getter_AddRefs(colorTable));    
    (*_result)->SetColorTable(colorTable);
  }
  
  return rv;
}
