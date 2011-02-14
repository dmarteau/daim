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
#include "cciImageList.h"
#include "cciImageContainerUtils.h"
#include "cciScriptableImage.h"
#include "cciIRegionContainer.h"

/* Implementation file */
CCI_IMPL_ISUPPORTS3(cciImageList, cciIImageList,
                                  cciIImageListContainer,
                                  cciIImageContainer)

#define BUFFER_DFLT_SIZE 16
#define BUFFER_MAX_SIZE  4096
#define CHECK_SIZE(pos) (pos < mBuffers.size())

#define _NOINDEX_ dm_noindex

cciImageList::cciImageList()
{
  // Init
  mBuffers.resize(BUFFER_DFLT_SIZE);
}

cciImageList::~cciImageList()
{
  /* destructor code */
}

//--------------------------------------------------------------------
cci_result cciImageList::privStoreBuffer(  dm_uint index,
                                           const dmImage* image,
                                           const dm_rect* clip_rect,
                                           EPixelFormat format )
{
  dmDEBUG_ASSERT( image != NULL );
  dmRect rect(image->Rect());

  if(clip_rect)
    rect.Clip(*clip_rect);

  dmImageDescriptor* _Desc = NULL;
  if(format!=dmPixelFormatUndefined)
    _Desc = dmGetDescriptor(format);

  if(_Desc == NULL)
     _Desc = image->TypeDescriptor();

  if(!rect.IsEmpty())
  {
    if(index!=_NOINDEX_)
    {
      dmLink<dmImage>& _buff = mBuffers[index];
      if( _buff.IsNull()  == false  &&
          _buff.Pointer() != image  && // prevent copy onto itself
          _buff->PixelFormat()==_Desc->PixelFormat() &&
          _buff->Width()   == rect.Width()  &&
          _buff->Height()  == rect.Height()  )

       _buff->GetCopy(*image,rect);

      else
       mBuffers[index] = image->CreateCopy(*_Desc,rect);
    } else
       mBuffers.push_back(image->CreateCopy(*_Desc,rect));

    return CCI_OK;
  }

  return CCI_ERROR_FAILURE;
}

//=========================
// cciIImageList
//=========================


/* [noscript] void storeBuffer (in dm_uint32 index, in cciImage image, in dmRectPtr rect, in EPixelFormat format); */
CCI_IMETHODIMP cciImageList::StoreBuffer(dm_uint32 index, dmImage *image, dm_rect *rect, EPixelFormat format)
{
  if(image!=NULL && (index==_NOINDEX_ || CHECK_SIZE(index) ))
  {
    return privStoreBuffer(index,image,rect,format);
  }

  return CCI_ERROR_INVALID_ARG;
}

/* [noscript,notxpcom] dmImagePtr getNativeBuffer(in dm_uint32 index); */
CCI_IMETHODIMP_(dmImage *) cciImageList::GetNativeBuffer(dm_uint32 index)
{
  if(CHECK_SIZE(index))
    return mBuffers[index];

  return dm_null;
}

/* boolean isEmpty (in dm_uint32 index); */
CCI_IMETHODIMP cciImageList::IsEmpty(dm_uint32 index, dm_bool *retval CCI_OUTPARAM)
{
  if( CHECK_SIZE(index) )
  {
    *retval = RETVAL_BOOL(!mBuffers[index].IsNull());
    return CCI_OK;
  }

  return CCI_ERROR_INVALID_ARG;
}

/* void releaseBuffer (in dm_uint32 index); */
CCI_IMETHODIMP cciImageList::ReleaseBuffer(dm_uint32 index)
{
  if(CHECK_SIZE(index) )
  {
    mBuffers[index].Release();
    return CCI_OK;
  }

  return CCI_ERROR_INVALID_ARG;
}

/* attribute dm_uint32 size; */
CCI_IMETHODIMP cciImageList::GetSize(dm_uint32 *aSize)
{
  *aSize = mBuffers.size();
  return CCI_OK;
}
CCI_IMETHODIMP cciImageList::SetSize(dm_uint32 aSize)
{
  if(aSize < BUFFER_MAX_SIZE)
  {
    if(aSize<1) aSize = 1;
    if(aSize > mBuffers.size())
       mBuffers.resize(aSize);

    return CCI_OK;
  }

  return CCI_ERROR_INVALID_ARG;
}

/* void createBuffer (in dm_uint32 index, in EPixelFormat format, in dm_uint32 width, in dm_uint32 height); */
CCI_IMETHODIMP cciImageList::CreateBuffer(dm_uint32 index, EPixelFormat format, dm_uint32 width, dm_uint32 height)
{
  if(CHECK_SIZE(index))
  {
   dmImageDescriptor* pDesc = dmGetDescriptor(format);
   if(pDesc)
   {
     cci_result rv;

     // Check if we can reuse a buffer
     // This buffer should not been held by someone else
     dmLink<dmImage> _buff = mBuffers[index];
     if( _buff.IsNull()       == false  &&
         _buff.RefCount()     == 1 &&
         _buff->PixelFormat() == pDesc->PixelFormat() &&
         _buff->Width()       == width &&
         _buff->Height()      == height  )
     {
        // Ok return that buffer
        return CCI_OK;
     }
     else
     {
       dmTRY

       _buff = pDesc->CreateImage(width,height);

       dmCATCH(_E)
         dmTRACE_EXCEPTION(_E);
         return CCI_ERROR_FAILURE;
       dmDONE

       if(!_buff.IsNull())
       {
         mBuffers[index] = _buff;
         rv = CCI_OK;
       }
       else
         rv = CCI_ERROR_FAILURE;

       return rv;
     }
   }
   else
     dmLOG_ERROR("cciImageList:Invalid image format !");
  }

  return CCI_ERROR_INVALID_ARG;

}

/* void clear (); */
CCI_IMETHODIMP cciImageList::Clear()
{
  for(int i=mBuffers.size();--i>=0;)
    mBuffers[i].Release();

  return CCI_OK;
}

/* void moveBuffer (in dm_uint32 from, in dm_uint32 to); */
CCI_IMETHODIMP cciImageList::MoveBuffer(dm_uint32 from, dm_uint32 to)
{
  if(CHECK_SIZE(from) && CHECK_SIZE(to) )
  {
    mBuffers[to] = mBuffers[from];
    return CCI_OK;
  }

  return CCI_ERROR_INVALID_ARG;
}

/* void swapBuffers (in dm_uint32 first, in dm_uint32 second); */
CCI_IMETHODIMP cciImageList::SwapBuffers(dm_uint32 first, dm_uint32 second)
{
  if(CHECK_SIZE(first) && CHECK_SIZE(second) )
  {
    dmLink<dmImage> _tmp = mBuffers[second];
    mBuffers[second] = mBuffers[first];
    mBuffers[first]  = _tmp;
    return CCI_OK;
  }

  return CCI_ERROR_INVALID_ARG;
}

/* void cloneBuffer (in dm_uint32 src, in dm_uint32 dst); */
CCI_IMETHODIMP cciImageList::CloneBuffer(dm_uint32 src, dm_uint32 dst)
{
  if(CHECK_SIZE(src) && CHECK_SIZE(dst))
  {
    cci_result rv;

    dmLink<dmImage> _src = mBuffers[src];
    if(!_src.IsNull())
    {
      rv = privStoreBuffer(dst,_src,NULL,dmPixelFormatUndefined);
    }
    else
    {
      mBuffers[dst].Release();
      rv = CCI_OK;
    }

    return rv;
  }

  return CCI_ERROR_INVALID_ARG;
}

/* void setBufferFormat (in dm_uint32 index, in EPixelFormat format); */
CCI_IMETHODIMP cciImageList::SetBufferFormat(dm_uint32 index, EPixelFormat format)
{
  dmImageDescriptor* _Desc = dmGetDescriptor(format);

  if(_Desc!=NULL)
  {
    cci_result rv;

    if(index==_NOINDEX_)
    {
      for(size_t i=0;i<mBuffers.size();++i)
      {
        dmLink<dmImage>& _src = mBuffers[i];
        if(!_src.IsNull() && (_src->PixelFormat()!=_Desc->PixelFormat()))
          mBuffers[i] = _src->CreateCopy(*_Desc);
      }
      rv = CCI_OK;
    }
    else
    if(CHECK_SIZE(index))
    {
      dmLink<dmImage>& _src = mBuffers[index];
      if(!_src.IsNull() && (_src->PixelFormat()!=_Desc->PixelFormat()))
        mBuffers[index] = _src->CreateCopy(*_Desc);

      rv = CCI_OK;
    } else
      rv = CCI_ERROR_FAILURE;

    return rv;
  }

  return CCI_ERROR_INVALID_ARG;
}

/* dm_uint32 getBufferFormat (in dm_uint32 index); */
CCI_IMETHODIMP cciImageList::GetBufferFormat(dm_uint32 index, dm_uint32 *_retval CCI_OUTPARAM)
{
  if(CHECK_SIZE(index))
  {
    dmLink<dmImage>& _Img = mBuffers[index];
    if(!_Img.IsNull())
      *_retval = _Img->PixelFormat();
    else
      *_retval = dmPixelFormatUndefined;

    return CCI_OK;
  }

  return CCI_ERROR_INVALID_ARG;
}

/* void spliceBuffers (in dm_uint32 to, in dm_uint32 from, in cciIImageList source, in dm_uint32 count); */
CCI_IMETHODIMP cciImageList::SpliceBuffers(dm_uint32 to, dm_uint32 from, cciIImageList *source, dm_uint32 count)
{
  CCI_ENSURE_ARG_POINTER(source);

  cci_result rv;

  dm_uint32 srcSize;
  rv = source->GetSize(&srcSize);
  if (CCI_FAILED(rv))
      return rv;

  if(from < srcSize)
  {
    // For optimisation we require a cciIImageListContainer
    // This will prevent creating a new dmImage container for the data
    cci_Ptr<cciIImageListContainer> _Container = do_QueryInterface(source,&rv);
    if (CCI_FAILED(rv))
        return rv;

    if(count==_NOINDEX_)
       count = srcSize - from;

    dm_uint32 last_from = from + count;

    if(last_from > srcSize)
       last_from = srcSize;

    // Make room for all elements
    if(to + count > mBuffers.size())
       mBuffers.resize(to + count);

    for(;from < last_from;++from,++to)
    {
      rv = _Container->GetImageLink(mBuffers[to],from);
      if (CCI_FAILED(rv))
          return rv;

      source->ReleaseBuffer(from);
    }

    rv = CCI_OK;
  }
  else
    rv = CCI_ERROR_INVALID_ARG;

  return rv;
}

/* void copyBuffers (in dm_uint32 to, in dm_uint32 from, in cciIImageList source, in dm_uint32 count); */
CCI_IMETHODIMP cciImageList::CopyBuffers(dm_uint32 to, dm_uint32 from, cciIImageList *source, dm_uint32 count)
{
  CCI_ENSURE_ARG_POINTER(source);

  cci_result rv;

  dm_uint32 srcSize;
  rv = source->GetSize(&srcSize);
  if (CCI_FAILED(rv))
      return rv;

  if(from >= srcSize)
     return CCI_ERROR_INVALID_ARG;
  
  // For optimisation we require a cciIImageListContainer
  // This will prevent creating a new dmImage container for the data
  cci_Ptr<cciIImageListContainer> _Container = do_QueryInterface(source,&rv);
  if (CCI_FAILED(rv))
      return rv;

  if(count==_NOINDEX_)
     count = srcSize - from;

  dm_uint last_from = from + count;
  if(last_from > srcSize)
     last_from = srcSize;

  if(to + count > mBuffers.size())
     mBuffers.resize(to + count);

  dmLink<dmImage> _src;

  for(;from < last_from;++from,++to)
  {
    rv = _Container->GetImageLink(_src,from);
    if (CCI_FAILED(rv))
        return rv;

    if(!_src.IsNull())
       mBuffers[to] = _src->CreateCopy();
  }

  return rv;
}

/* void setCapacity (in dm_uint32 capacity); */
CCI_IMETHODIMP cciImageList::SetCapacity(dm_uint32 capacity)
{
  if(mBuffers.size() < capacity)
  {
    if(capacity > BUFFER_MAX_SIZE)
       return CCI_ERROR_INVALID_ARG;

    mBuffers.resize(capacity);
  }
  return CCI_OK;
}

/* [noscript] void getBufferData (in dm_uint32 index, in dmImageDataRef data); */
CCI_IMETHODIMP cciImageList::GetBufferData(dm_uint32 index, dmImageData & data)
{
  if(CHECK_SIZE(index))
  {
    dmLink<dmImage>& _Img = mBuffers[index];
    if(!_Img.IsNull() && _Img->GetImageData(data))
       return CCI_OK;

    return CCI_ERROR_FAILURE;
  }

  return CCI_ERROR_INVALID_ARG;
}

/* [noscript] void setBufferData (in dm_uint32 index, in dmImageDataRef data); */
CCI_IMETHODIMP cciImageList::SetBufferData(dm_uint32 index, dmImageData & data)
{
  if(data.Scan0 == dm_null||(data.Width == 0)||(data.Height ==  0))
    return CCI_ERROR_INVALID_ARG;

  if(index==_NOINDEX_ || CHECK_SIZE(index) )
  {
    dmLink<dmImage> image;

    dmImageDescriptor* _descriptor = dmGetDescriptor(data.PixelFormat);
    if(_descriptor)
    {
      image = _descriptor->CreateImage(data);
      return privStoreBuffer(index,image,dm_null,data.PixelFormat);
    }
    else {
      dmLOG_ERROR("cciImageList:Invalid pixel format !");
    }
  }

  return CCI_ERROR_INVALID_ARG;
}


/* cciImage getImage (in dm_uint32 index); */
CCI_IMETHODIMP cciImageList::GetImage(dm_uint32 index, cciImage *_retval CCI_OUTPARAM)
{
  if(CHECK_SIZE(index)) 
  {
    // Get the image link
    dmLink<dmImage>& _Img = mBuffers[index];
    if(_Img.IsNull())
      return CCI_ERROR_FAILURE;

    // Create a new wrapper for this image
    cciIImageContainer* wrapper = new cciScriptableImage(_Img);
    if(!wrapper)
       return CCI_ERROR_OUT_OF_MEMORY;
    
    CCI_ADDREF( *_retval = wrapper );
    return CCI_OK;
  }
  
  return CCI_ERROR_INVALID_ARG;
}

/* void setImage (in dm_uint32 index, in cciImage image, in cciRegion rgn, in EPixelFormat format); */
CCI_IMETHODIMP cciImageList::SetImage(dm_uint32 index, cciImage image, cciRegion rgn, EPixelFormat format)
{
  dmImage* img = CCI_IF_NATIVE(image);
  CCI_ENSURE_ARG_POINTER(img);  
  
  dmRegion* roi = CCI_IF_NATIVE(rgn);

  return StoreBuffer(index,img,roi?&roi->Rectangle():dm_null,format);
}


//=========================
// cciIImageListContainer
//=========================

CCI_IMETHODIMP cciImageList::GetImageLink(dmLink<dmImage>& link, dm_uint32 index )
{
  if(CHECK_SIZE(index) )
  {
    link = mBuffers[index];
    return CCI_OK;
  }

  return CCI_ERROR_INVALID_ARG;
}

CCI_IMETHODIMP cciImageList::SetImageLink( dmLink<dmImage>& link, dm_uint32 index )
{
  if(CHECK_SIZE(index) )
  {
    mBuffers[index] = link;
    return CCI_OK;
  }

  return CCI_ERROR_INVALID_ARG;
}


//===============================================
// cciIImageContainer
//===============================================

/* [noscript,notxpcom] dmImagePtr getNative (); */
CCI_IMETHODIMP_(dmImage *) cciImageList::GetNative()
{
  if(mBuffers.size()>0) {
     return mBuffers[0];
  }
  
  return dm_null;
}

