#define dmUseCCI
#define dmUseUtilitiesExtra
#define dmUseKernelBuffer
#define dmUseKernelImage
#include "daim_kernel.h"
#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Parameters.h"
#include "daim_modules/CCI_ImageList.h"

#include "daim_modules/processing/CCI_Colorspace.h"
#include "daim_modules/processing/CCI_ImageMath.h"

//------------------------------------------------------------------
#define CCIC_NAME CCIC_ImageMath
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_ImageMath_Version
#define CCI_REVISION  2
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "common/dmUserLib.h"

#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"

#include "operators/dmAccumulator.h"

#define _NOINDEX_ ~0UL

//--------------------------------------------------------------------
CCI_DECL_DATA(ImageMath)
{
  dm_uint _Dummy;
};
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( ImageMath )
{
  CCI_CONSTRUCT_SUPER()
  CCI_UNUSUED_INSTANCE(ImageMath)

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( ImageMath )
{
  CCI_UNUSUED_INSTANCE(ImageMath)
  CCI_DESTROY_SUPER()
}
/*
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( ImageMath )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( ImageMath )
CCI_END_SETTER()
*/
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
static inline CCI_METHOD_GET( ImageMath )
{
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
static inline CCI_METHOD_SET( ImageMath )
{
  CCI_RETURN_INVOKE_SUPER()
}
/*
//---------------------------------------------------------------------
// Method: DoImageMath
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMath, DoImageMath )
{
  CCI_INSTANCE(ImageMath)

  bool res = false;

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       lhs_image = _IParams.GetImage();
    dmImageBuffer* buffer    = _IParams.GetImageBuffer();
    dmRegion*      rgn       = _IParams.GetRoi();

    if(lhs_image) 
    {   
      dmASSERT( buffer != NULL );

      // Get rhs image operand
      dmIImageList _ImageList;
      if(_ImageList.QueryInterface(_IParams.GetImageList())) 
      {
        dmImage* rhs_image = _ImageList.GetBuffer(_This->BufferIndex);
        if(rhs_image == NULL) {
          CCI_SET_ERROR_MSG(_TXT("No Image in Buffer !"));
          CCI_RETURN_FAIL()
        }

        // This will ensure that images will be of the same type
        buffer->CreateBuffer(*lhs_image,*rgn);
        lhs_image->GetCopy(*rhs_image,*rgn,rgn->Rectangle().TopLeft());
     
        dmRegion _Rgn = lhs_image->Rect();
        dmArithmetics(dmBufferParameters(*buffer,*lhs_image,_Rgn,
                      static_cast<dmELutIndex>(_IParams.GetMapIndex())),
                      CCI_P(operation));

        CCI_RETURN_OK()
      }    
    } else 
      CCI_SET_ERROR_MSG(_TXT("No image !"));
  } else 
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));    

  CCI_RETURN_FAIL()
}
*/
//---------------------------------------------------------------------
// Defined in daim_kernel library
bool __dmKernel _dmDoImageMath2( dmBufferParameters& , dm_int );
bool __dmKernel _dmDoImageMath5( dm_int op, dmImage& lhs, 
                                 const dmImage& rhs , 
                                 const dmRegion&, const dmPoint& );

//---------------------------------------------------------------------
// Method : DoImageMath
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMath, DoImageMath )
{
  CCI_UNUSUED_INSTANCE(ImageMath)

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       lhs_image = _IParams.GetImage();
    dmRegion*      rgn       = _IParams.GetRoi();
    dmImageBuffer* buffer    = _IParams.GetImageBuffer();

    if(lhs_image) 
    {   
      dmASSERT( buffer != NULL );
      if(rgn->IsEmptyRoi()) 
       rgn->SetRectRoi(lhs_image->Rect());

      // Get rhs image operand
      dmIImageList _ImageList;
      if(_ImageList.QueryInterface(_IParams.GetImageList()))
      {
        if(CCI_P(index)!=_NOINDEX_)
        {
          dmImage* rhs_image = _ImageList.GetBuffer(CCI_P(index));
          if(rhs_image == NULL) {
            CCI_SET_ERROR_MSG(_TXT("No Image in Buffer !"));
            CCI_RETURN_FAIL()
          }

          if(dmIsPixelFormatScalar(lhs_image->PixelFormat())  &&
             dmIsPixelFormatScalar(rhs_image->PixelFormat()))
          {  
            switch(CCI_P(operation))
            {
              case dmTk::Math::MulPixels : 
                dmMultiplyImage(*lhs_image,*rhs_image,*rgn,rgn->Rectangle().TopLeft());
                CCI_RETURN_OK()
                break;
              case dmTk::Math::DivPixels :              
                dmDivideImage(*lhs_image,*rhs_image,*rgn,rgn->Rectangle().TopLeft());
                CCI_RETURN_OK()
                break;
            }           
          }
          
          if(lhs_image->PixelFormat()!=rhs_image->PixelFormat())
          {
             // This will ensure that images will be of the same type
             buffer->CreateBuffer(*lhs_image,*rgn);
             lhs_image->GetCopy(*rhs_image,*rgn,rgn->Rectangle().TopLeft());
       
             dmBufferParameters _params(*buffer,*lhs_image,*rgn);
             _dmDoImageMath2(_params,CCI_P(operation));
          } else
             _dmDoImageMath5(CCI_P(operation),*lhs_image,*rhs_image,
                                              *rgn,rgn->Rectangle().TopLeft());
          CCI_RETURN_OK()
        } 
        else // Perform the operation for all images in the list
        {
          dmImage* rhs_image = lhs_image;
          dm_uint size = _ImageList.Size();
          for(dm_uint index=0;index<size;++index)
          {
            lhs_image = _ImageList.GetBuffer(index);
            if(lhs_image) 
            {
              if(dmIsPixelFormatScalar(lhs_image->PixelFormat()) &&
                 dmIsPixelFormatScalar(rhs_image->PixelFormat()))
              {  
                 switch(CCI_P(operation))
                 {
                   case dmTk::Math::MulPixels : 
                     dmMultiplyImage(*lhs_image,*rhs_image,*rgn,rgn->Rectangle().TopLeft());
                     CCI_RETURN_OK()
                     break;
                   case dmTk::Math::DivPixels :
                    dmDivideImage(*lhs_image,*rhs_image,*rgn,rgn->Rectangle().TopLeft());
                    CCI_RETURN_OK()
                    break;
                 }           
              }
              
              if(lhs_image->PixelFormat()!=rhs_image->PixelFormat()) 
              {
                // This will ensure that images will be of the same type
                buffer->CreateBuffer(*rhs_image,*lhs_image->TypeDescriptor(),*rgn);

                dmBufferParameters _params(*buffer,*lhs_image,*rgn);
                if(!_dmDoImageMath2(_params,CCI_P(operation)))
                     dmLOG("Failed to perform image operation !\n");

              } else
               if(!_dmDoImageMath5(CCI_P(operation),*lhs_image,*rhs_image,
                                                *rgn,rgn->Rectangle().TopLeft()))
                     dmLOG("Failed to perform image operation !\n"); 
            }
          } 
          CCI_RETURN_OK()
        }  
      }    
    } else 
      CCI_SET_ERROR_MSG(_TXT("No image !"));
  } else 
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : Accumulate
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMath, Accumulate )
{
  CCI_UNUSUED_INSTANCE(ImageMath)

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*       image    = _IParams.GetImage();
    dmRegion*      rgn      = _IParams.GetRoi();
    dmImageBuffer* buffer   = _IParams.GetImageBuffer();

    dmIImageList ImageList;
    if(ImageList.QueryInterface(_IParams.GetImageList())) 
    {
      dm_uint count = ImageList.Size();
      if(count>0) 
      {
        dmImage*      rhs_image;
        dmAccumulator accumulator;

        // Set the destination rectangle
        if(image==NULL) {
          if(rgn->IsEmptyRoi()) {           // The ROI is empty, get the total size from
            for(dm_uint i=0;i<count;++i) {  // buffers size
              rhs_image = ImageList.GetBuffer(i);
              if(rhs_image!=NULL)
                rgn->AddCoordinates(rhs_image->Rect());
            }
          }
          // Create the buffer that will hold the result
          buffer->CreateBuffer(*ImageList.GetBuffer(0)->TypeDescriptor(),*rgn);        
          accumulator.Initialize(buffer->Buffer(),*rgn);
        } else {
          // If the image is not null we should have a non empty ROI 
          // \see Parameters_ImageBuffer()
          dmASSERT( !rgn->IsEmptyRoi() );
          accumulator.Initialize(image,*rgn);
        }
        
        // Accumulate

        for(dm_uint i=0;i<count;++i) 
        {
          rhs_image = ImageList.GetBuffer(i); 
          accumulator.Add( rhs_image );
        }

        accumulator.Finalize(CCI_P(normalize)!=dm_false);
        CCI_RETURN_OK()

      } // count <= 0 !
    }
  } else 
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : DoImageListMath
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMath, DoImageListMath )
{
  CCI_UNUSUED_INSTANCE(ImageMath)

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmRegion*      rgn      = _IParams.GetRoi();
    dmImageBuffer* buffer   = _IParams.GetImageBuffer();

    dmIImageList LhsList;
    if(LhsList.QueryInterface(CCI_P(imagelist)))
    {
      dmIImageList RhsList;
      // Do we really have to get the default imagelist ?
      if(RhsList.QueryInterface(_IParams.GetImageList())) 
      {
        // Loop over all images in the list 
        dm_uint count = dm_min(LhsList.Size(),RhsList.Size());
        if(count>0) 
        {
          dmImage *lhs_image;
          dmImage *rhs_image;

          LhsList.Resize(count);
          for(dm_uint i=0;i<count;++i) 
          { 
            lhs_image = LhsList.GetBuffer(i);    
            rhs_image = RhsList.GetBuffer(i);
            if(lhs_image==NULL) {
              if(rhs_image != NULL) 
                LhsList.StoreBuffer(i,rhs_image);
            }
            else if(rhs_image != NULL)
            {
              if(rgn->IsEmptyRoi()) 
                 rgn->SetRectRoi(rhs_image->Rect());
              
              // Handle the special case of image multiplication / division
              // Require that image are both scalars
              if(dmIsPixelFormatScalar(lhs_image->PixelFormat()) &&
                 dmIsPixelFormatScalar(rhs_image->PixelFormat()))
              {  
                 bool done = false;
                            
                 switch(CCI_P(operation))
                 {
                   case dmTk::Math::MulPixels : 
                     dmMultiplyImage(*lhs_image,*rhs_image,*rgn,rgn->Rectangle().TopLeft());
                     done = true;
                     break;
                   case dmTk::Math::DivPixels :
                    dmDivideImage(*lhs_image,*rhs_image,*rgn,rgn->Rectangle().TopLeft());
                    done = true;
                    break;
                 }     
                 
                 if(done)
                    continue;      
              }
              
              if(lhs_image->PixelFormat()!=rhs_image->PixelFormat()) 
              {
                // Are the image of the same type ? 
                // This will ensure that images will be of the same type
                buffer->CreateBuffer(*rhs_image,*lhs_image->TypeDescriptor(),*rgn);

                dmBufferParameters _params(*buffer,*lhs_image,*rgn);
                if(!_dmDoImageMath2(_params,CCI_P(operation)))
                     dmLOG("Failed to perform image operation !\n");
              } else             
                if(!_dmDoImageMath5(CCI_P(operation),*lhs_image,*rhs_image,
                                *rgn,rgn->Rectangle().TopLeft()))
                     dmLOG("Failed to perform image operation !\n");
            }
          }
          CCI_RETURN_OK()
        }
      } 
    } else
      CCI_RETURN_INVALID_PARAM()
  } else 
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: AddMul
//---------------------------------------------------------------------
// defined in daim_kernel library
bool __dmKernel _dmDoImageMath4( dmImage& lhs , const dmRegion& r,
                                 dm_real a, dm_real b );
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMath, AddMul )
{
  CCI_UNUSUED_INSTANCE(ImageMath)

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*   lhs_image = _IParams.GetImage();
    dmRegion*  rgn       = _IParams.GetRoi();

    if(lhs_image) 
    {   
      if(_dmDoImageMath4(*lhs_image,*rgn,CCI_P(mulval),CCI_P(addval)))
        CCI_RETURN_OK()

    } else 
      CCI_SET_ERROR_MSG(_TXT("No image !"));
  } else 
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));    

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : Blend
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMath, Blend )
{
  CCI_UNUSUED_INSTANCE(ImageMath)

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage*  image   = _IParams.GetImage(); 
    if(image) 
    {   
      // Get rhs image operand
      dmIImageList _ImageList;
      if(_ImageList.QueryInterface(_IParams.GetImageList()))
      {
        dmImage* rhs_image = _ImageList.GetBuffer(CCI_P(index));
        if(rhs_image == NULL) {
          CCI_SET_ERROR_MSG(_TXT("No Image in Buffer !"));
          CCI_RETURN_FAIL()
        }
  
        dmRegion mask;
        dmPoint  offs(CCI_P(x),CCI_P(y));
      
        dmRegion* rgn = _IParams.GetMask();
        if(rgn==dm_null || rgn->IsEmptyRoi()) {
           mask.SetRectRoi(rhs_image->Rect());
        } else
           mask = *rgn;

        if(CCI_P(percent)==1) { // Need just to copy the image
           image->GetCopy(*rhs_image,mask,offs);
           CCI_RETURN_OK()
        }

        if(image->PixelFormat()!=rhs_image->PixelFormat()) 
        {
           dmLink<dmImage> copyOf = rhs_image->CreateCopy(*image->TypeDescriptor(),
                                                          mask.Rectangle());
           mask.OffsetRoi();                            
           if(!dmBlendImage(CCI_P(percent),*image,*copyOf,mask,offs))
               CCI_RETURN_FAIL()
        } else {
           if(dmBlendImage(CCI_P(percent),*image,*rhs_image,mask,offs))
            CCI_RETURN_OK()
        }
      }    
    } else 
      CCI_SET_ERROR_MSG(_TXT("No image !"));
  } else 
    CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : Truncate
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageMath, Truncate )
{
  CCI_UNUSUED_INSTANCE(ImageMath)
  
  if(!dmIsPixelFormatScalar(CCI_P(format)))
      CCI_RETURN_INVALID_FORMAT()

  dmIParameters _IParams;
  if(_IParams.QueryInterface(CCI_P(params)))
  {
    dmImage* image = _IParams.GetImage();

    if(!image) {
      CCI_SET_ERROR_MSG(_TXT("No image !"));
      CCI_RETURN_FAIL() 
    }

    dmIImageList _ImageList;
    if(_ImageList.QueryInterface(_IParams.GetImageList()))
    {
      dmRegion* rgn = _IParams.GetRoi();
      dmRect    rct = rgn->Rectangle();
      if(_ImageList.CreateBuffer(CCI_P(index),CCI_P(format),rct.Width(),rct.Height()))
      {
        dmImage* dest = _ImageList.GetBuffer(CCI_P(index));
        dmASSERT(dest!=dm_null);
       
        dmLink<dmImage> src = dmNewLink<dmImage>(image);
       
        // Extract image if not a scalar format
        if(!dmIsPixelFormatScalar(src->PixelFormat()))
        {
          dm_uint  channel = _IParams.GetChannel();

          // Get as grey level image
          if(channel != dmIParameters::flAllChannels) 
          {
            dmRegion region = *rgn;
            
            src = dmCreateCopy(src,region.Rectangle(),dmPixelFormat8bppIndexed);
            // Truncate with the 
            region.OffsetRoi();
            if(dmTruncateImage(*src,*dest,region,dmPoint(0,0)))
               CCI_RETURN_OK()
          }
          else 
          {
            // Extract the corresponding channel restricted to
            // the ROI bounding box - store in internal buffer
            dmIColorSpace _IColorspace;
            if(_IColorspace.QueryInterface(_IParams.GetColorSpace())) 
            {
              _IColorspace.Extract(_IParams,channel,dm_noindex);
              dmImageBuffer* _buffer = _IParams.GetImageBuffer();
              dmASSERT( _buffer != NULL );
 
              src = _buffer->Buffer();
              if(dmTruncateImage(*src,*dest,_buffer->BufferRgn(),dmPoint(0,0)))
                 CCI_RETURN_OK()          
            }  
          }
        }
        else
        {
          // Scalar to scalar conversion
          if(dmTruncateImage(*src,*dest,*rgn,dmPoint(0,0)))
             CCI_RETURN_OK()
        }
      }
    } 
  } else 
     CCI_SET_ERROR_MSG(_TXT("Invalid parameters !"));
  
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( ImageMath )
  CCI_REGISTER_METHOD_PP( ImageMath, DoImageMath     )
  CCI_REGISTER_METHOD_PP( ImageMath, Accumulate      )
  CCI_REGISTER_METHOD_PP( ImageMath, DoImageListMath )
  CCI_REGISTER_METHOD_PP( ImageMath, AddMul          )
  CCI_REGISTER_METHOD_PP( ImageMath, Blend           )
  CCI_REGISTER_METHOD_PP( ImageMath, Truncate        )
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( ImageMath )
{
  CCI_INVOKE_FACTORY( ImageMath )
}
//---------------------------------------------------------------------
