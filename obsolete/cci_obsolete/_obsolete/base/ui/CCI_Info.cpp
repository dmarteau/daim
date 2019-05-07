
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

/* $ChangeLog  v3.2, 22 sept. 2006 01:20:34 , David
 *
 * Added method GetImageA
 */

#define dmUseKernelImageTemplates
#define dmUseKernelBuffer
#define dmUseServicesMngr

#ifndef NO_DAIM_USER_INTERFACE
#define dmUseToolManager
#include "daim_tools.h"
#include "dmToolClasses.h"
#else  //NO_DAIM_USER_INTERFACE
#include "daim_utilities.h"
#endif //NO_DAIM_USER_INTERFACE

#include "daim/daim_base.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_Document.h"
#include "daim_modules/CCI_EventListener.h"
#include "daim_modules/CCI_Info.h"
#include "daim_modules/CCI_PropertyList.h"
#include "daim_modules/CCI_MaskBuffer.h"
#include "daim_modules/CCI_ImageList.h"
#include "daim_modules/CCI_Parameters.h"

//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_ImageInfo
#define CCIC_SUPER CCIC_Document
#define CCIC_SUPER_VERSION CCI_Document_Version
#define AUTHOR _TXT("David Marteau")
#define CCI_VERSION   CCI_ImageInfo_Version
#define CCI_REVISION  2
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2004"))
//--------------------------------------------------------------------
// 
//--------------------------------------------------------------------
#define CCI_INTERNAL_MODULE

#include "daim_modules/CCI_Base.h"
#include "daim_info.h"
#include "daim_unitsprefs.h"

#include "daim_modules/CCI_Interface.h"
//--------------------------------------------------------------------
#define IMPLEMENT_SIMPLE_DOCUMENT()           \
    cci_Object* obj;                          \
    virtual void OnSetTitle     ( dm_bool bStatus ) { CCI_Set(obj,CCIA_DECL_(ImageInfo,Title     ),bStatus); } \
    virtual void OnSetMask      ( dm_bool bStatus ) { CCI_Set(obj,CCIA_DECL_(ImageInfo,MaskUpdate),bStatus); } \
    virtual void OnSetModified  ( dm_bool bStatus ) { CCI_Set(obj,CCIA_DECL_(Document ,Modified  ),bStatus); } \
    virtual void OnSetActiveInfo( dm_bool bStatus ) { CCI_Set(obj,CCIA_DECL_(ImageInfo,Activate  ),bStatus); } \
    virtual void OnRecalculate  ( dm_bool bStatus ) { CCI_Set(obj,CCIA_DECL_(Document ,Updated   ),bStatus); } \
    virtual void OnNewDocument  ( dm_bool bStatus ) { CCI_Set(obj,CCIA_DECL_(Document ,New       ),bStatus); } \
    virtual void* This()         { return obj; }

//--------------------------------------------------------------------
// Document type that dot not use graphical interface
//--------------------------------------------------------------------
class document_simple : public daim_document
{
  public:
  ~document_simple() {}
   document_simple(daim_info* _info,cci_Object* ob) 
   : daim_document(_info), obj(ob) {}

   IMPLEMENT_SIMPLE_DOCUMENT()
};
//--------------------------------------------------------------------

#ifndef NO_DAIM_USER_INTERFACE
#include "daim_info_mngr.h"
#include "daim_manager.h"
#include "daim_layers.h"

//--------------------------------------------------------------------
// Document that implements tool managers for handling 
// graphical interface
//--------------------------------------------------------------------
class document_interface : public daim_info_manager
{
  public:
  ~document_interface() {}
   document_interface(daim_info* _info,cci_Object* ob) 
   : daim_info_manager(_info), obj(ob) {}

   IMPLEMENT_SIMPLE_DOCUMENT()

  protected:
};
//--------------------------------------------------------------------
#endif //NO_DAIM_USER_INTERFACE
//--------------------------------------------------------------------
CCI_DECL_DATA( ImageInfo )
{
  dm_uint       doctype;
  dm_uint       mask;
  daim_info     info;

  dmString      filename;

  #ifndef NO_DAIM_USER_INTERFACE
  daim_CmdUI  cmdui;
  #endif
};

#define THIS_DOC(_This)       _This->info.mDocument
#define THIS_INTERFACE(_This) static_cast<document_interface*>(THIS_DOC(_This))
#define IS_INTERFACE(_This)   (_This->doctype==CCIV_ImageInfo_UserInterface)

//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( ImageInfo )
{
  CCI_CONSTRUCT_SUPER()

  CCI_INSTANCE( ImageInfo )

#ifndef NO_DAIM_USER_INTERFACE
  CCI_INIT_MEMBER(cmdui)
#endif

  CCI_INIT_MEMBER(info);
  CCI_INIT_MEMBER(filename);

  _This->mask    = dm_true;
  _This->doctype = 
    #ifndef NO_DAIM_USER_INTERFACE
     CCIV_ImageInfo_UserInterface;
    #else  
     CCIV_ImageInfo_SimpleDocument;
    #endif

  CCI_BEGIN_INIT_MAP(ImageInfo)
    CCI_ATTR_SET( ImageInfo, DocumentType, _This->doctype = CCIA_DATA )
  CCI_END_INIT_MAP()

  //--------------------------
  // Create Document handler
  //--------------------------
  if IS_INTERFACE(_This)
  {
    #ifndef NO_DAIM_USER_INTERFACE
        document_interface* document = new document_interface(&_This->info,_THIS_OBJECT);
        document->Obtain();
        document->InitROI(); 
        document->InitMask();
        document->InitLayers();
        THIS_DOC(_This) = document;

        document->SetDefaultCursor(eCursor_crosshair);
    #endif
  } else {
    THIS_DOC(_This) = new document_simple(&_This->info,_THIS_OBJECT);
  }

  CCI_RETURN_THIS()
}
//---------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( ImageInfo )
{
  CCI_INSTANCE( ImageInfo )

  #ifndef NO_DAIM_USER_INTERFACE
  if IS_INTERFACE(_This) {
    THIS_INTERFACE(_This)->Release();
  }
  CCI_DESTROY_MEMBER( cmdui )
  #endif

  CCI_DESTROY_MEMBER(filename);
  CCI_DESTROY_MEMBER(info);
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( ImageInfo )
 
  CCI_ATTR_GET(Document,Width  , CCIA_RETVAL = _This->info.GetImageRect().Width()  )
  CCI_ATTR_GET(Document,Height , CCIA_RETVAL = _This->info.GetImageRect().Height() )

  CCI_ATTR_GET(ImageInfo,ImageFormat  , CCIA_RETVAL = _This->info.GetImageFormat() )
  CCI_ATTR_GET(ImageInfo,Channels     , CCIA_RETVAL = _This->info.NumChannels()    )
  CCI_ATTR_GET(ImageInfo,Activate     , CCIA_RETVAL = _BOOL(_This->info.IsActiveInfo()) )
  CCI_ATTR_GET(ImageInfo,Title        , 
     if(_This->info.GetTitle().Empty()) 
      CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->filename.CStr());
     else
      CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->info.GetTitle().CStr())  
  )
  CCI_ATTR_GET(ImageInfo,Path         , CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->info.GetPath().CStr()) )
  CCI_ATTR_GET(ImageInfo,Image        , CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->info.GetImage().Pointer())  )
  CCI_ATTR_GET(ImageInfo,Buffer       , CCIA_RETVAL = CCIA_TRANSLATE_VAL(_This->info.GetBuffer()) )

  CCI_ATTR_GET(ImageInfo,MaskUpdate   , CCIA_RETVAL = _This->mask                  )
  CCI_ATTR_GET(ImageInfo,MaskSize     , CCIA_RETVAL = _This->info.GetMaskSize()    )

#ifndef NO_DAIM_USER_INTERFACE
  CCI_ATTR_GET(ImageInfo,LUT          , CCIA_RETVAL = (IS_INTERFACE(_This)?CCIA_TRANSLATE_VAL(THIS_INTERFACE(_This)->GetLUT()):dm_null) )
  CCI_ATTR_GET(ImageInfo,MaskColor    , CCIA_RETVAL = (IS_INTERFACE(_This)?THIS_INTERFACE(_This)->GetMaskColor()   :eClr_NONE )      )
  CCI_ATTR_GET(ImageInfo,ExMaskColor  , CCIA_RETVAL = (IS_INTERFACE(_This)?THIS_INTERFACE(_This)->GetExtMaskColor():eClr_NONE )      )
#endif // NO_DAIM_USER_INTERFACE

  CCI_ATTR_GET(ImageInfo,DocumentType , CCIA_RETVAL = _This->doctype )
  CCI_ATTR_GET(ImageInfo,DocAttributs , CCIA_RETVAL = _This->info.GetDocAttrs()     )
  CCI_ATTR_GET(ImageInfo,LastDocError , CCIA_RETVAL = _This->info.GetLastDocError() )
  CCI_ATTR_GET(ImageInfo,IsValidMask  , CCIA_RETVAL = _BOOL(!_This->info.GetMask().IsEmptyRoi()) )

  CCI_ATTR_GET(Document, EmptyDocument, CCIA_RETVAL = _BOOL(!_This->info.IsValidView()) )

#ifndef NO_DAIM_USER_INTERFACE
  CCI_ATTR_GET(ImageInfo, CurrentManager, 
       daim_manager* manager = THIS_INTERFACE(_This)->SelectManager(NULL,0);
       if(manager) 
         CCIA_RETVAL = CCIA_TRANSLATE_VAL(manager->ToolName().CStr())
  )
#endif // NO_DAIM_USER_INTERFACE

CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( ImageInfo )

  CCI_ATTR_SET(ImageInfo,Title       ,_This->info.SetTitle(CCIA_TRANSLATE_DATA(char_t*)) )
  CCI_ATTR_SET(ImageInfo,MaskUpdate  ,_This->mask  = CCIA_DATA )

#ifndef NO_DAIM_USER_INTERFACE
  CCI_ATTR_SET(ImageInfo,MaskColor  , if(IS_INTERFACE(_This)) THIS_INTERFACE(_This)->SetMaskColor(CCIA_DATA,true)     )
  CCI_ATTR_SET(ImageInfo,ExMaskColor, if(IS_INTERFACE(_This)) THIS_INTERFACE(_This)->SetExtMaskColor(CCIA_DATA,true)  )
#else  // NO_DAIM_USER_INTERFACE
  CCIA_DISCARD(ImageInfo,MaskColor   )
  CCIA_DISCARD(ImageInfo,ExMaskColor )
#endif // NO_DAIM_USER_INTERFACE

  // Get  only
  CCIA_DISCARD(ImageInfo,LUT          )
  CCIA_DISCARD(ImageInfo,DocumentType )
  CCIA_DISCARD(ImageInfo,Image        ) 
  CCIA_DISCARD(ImageInfo,Buffer       ) 
  CCIA_DISCARD(ImageInfo,MaskSize     ) 
  CCIA_DISCARD(ImageInfo,Path         ) 
  CCIA_DISCARD(Document ,Width        ) 
  CCIA_DISCARD(Document ,Height       )

  CCIA_DISCARD(ImageInfo,DocAttributs ) 
  CCIA_DISCARD(ImageInfo,LastDocError ) 
  CCIA_DISCARD(ImageInfo,IsValidMask  )
  CCIA_DISCARD(ImageInfo,ImageFormat  )
  CCIA_DISCARD(ImageInfo,Channels     )

CCI_END_SETTER()
//---------------------------------------------------------------------
// Method: Refresh
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( ImageInfo, Refresh )
{
  CCI_INSTANCE( ImageInfo )
  _This->info.Refresh();

  CCI_RETURN_OK();
}
//---------------------------------------------------------------------
// Method: SetMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo, SetMask )
{
  CCI_INSTANCE( ImageInfo )
  if(CCI_P(region))
    _This->info.SetMask(*CCI_P(region),CCI_P(operation));
  else
    _This->info.KillMask();

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: GetMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo, GetMask )
{
  CCI_INSTANCE( ImageInfo )
  if(CCI_P(region)) {
    *CCI_P(region) = _This->info.GetMask();
    CCI_RETURN_OK()
  } else {
    if(!_This->info.GetMask().IsEmptyRoi())
      CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: SetRoi
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo, SetROI )
{
  CCI_INSTANCE( ImageInfo )
  if(CCI_P(region)) {
    _This->info.mROI = *CCI_P(region);
  } else {
    _This->info.mROI.KillRoi();
  }

#ifndef NO_DAIM_USER_INTERFACE
  //---------------------------------------------
  // La roi est stocke localement
  // On detruit celle qui pourrait être affichée
  //---------------------------------------------
  if IS_INTERFACE(_This) {
    daim_manager* manager = THIS_INTERFACE(_This)->SelectManager(DM_ROI,0);
    if(manager) 
      manager->OnCommand(cmdKillROI);
  }

#endif // NO_DAIM_USER_INTERFACE

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: GetRoi
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo, GetROI )
{
  CCI_INSTANCE( ImageInfo )

#ifndef NO_DAIM_USER_INTERFACE
  if IS_INTERFACE(_This) {
    daim_manager* manager = THIS_INTERFACE(_This)->SelectManager(DM_ROI,0);
    //---------------------------------------------
    // On récupère éventuellement la roi courante
    //---------------------------------------------
    if(manager)
      manager->OnCommand(cmdGetROI);
  }
#endif // NO_DAIM_USER_INTERFACE

  if(CCI_P(region)) 
  {
    dmRegion* _roi = CCI_P(region);

    *_roi = _This->info.mROI;
     _roi->ClipToRect(_This->info.GetImageRect());
 
    if(_roi->IsEmptyRoi())
       _roi->SetRectRoi(_This->info.GetImageRect());

    CCI_RETURN_OK()
  }
  else if(!_This->info.mROI.IsEmptyRoi())
    CCI_RETURN_OK()

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: MaskToRoi
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( ImageInfo, MaskToRoi )
{
  CCI_INSTANCE( ImageInfo )
  _This->info.mROI = _This->info.GetMask();

#ifndef NO_DAIM_USER_INTERFACE
  //---------------------------------------------
  // La roi est stocke localement
  // On detruit celle qui pourrait être affichée
  //---------------------------------------------
  if IS_INTERFACE(_This) {
    daim_manager* manager = THIS_INTERFACE(_This)->SelectManager(DM_ROI,0);
    if(manager) 
      manager->OnCommand(cmdKillROI);
  }
#endif // NO_DAIM_USER_INTERFACE
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: RoiToMask
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( ImageInfo, RoiToMask )
{
  CCI_INSTANCE( ImageInfo )

#ifndef NO_DAIM_USER_INTERFACE
  if IS_INTERFACE(_This) {
    daim_manager* manager = THIS_INTERFACE(_This)->SelectManager(DM_ROI,0);
    //---------------------------------------------
    // On récupère éventuellement la roi courante
    //---------------------------------------------
    if(manager)
      manager->OnCommand(cmdGetROI);
  }
#endif // NO_DAIM_USER_INTERFACE

  _This->info.SetMask(_This->info.mROI,0);
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: GetDensityRange
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo, GetDensityRange )
{
  CCI_INSTANCE( ImageInfo )
  
  _This->info.GetDensityRange(CCI_P(rmin),CCI_P(rmax));
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: SetDensityRange
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo, SetDensityRange )
{
  CCI_INSTANCE( ImageInfo )
   
  _This->info.SetDensityRange(CCI_P(rmin),CCI_P(rmax)); 
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: MarkModified
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo, MarkModified )
{
  CCI_INSTANCE( ImageInfo )
  _This->info.SetModified(CCI_P(bModified)!=dm_false);
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: RestoreFromBuffer
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( ImageInfo, RestoreFromBuffer )
{
  CCI_Try

    CCI_INSTANCE( ImageInfo )
    if(_This->info.RestoreFromBuffer())
      CCI_RETURN_OK()

  CCI_Catch(ImageInfo,RestoreFromBuffer)
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: SaveToBuffer
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( ImageInfo, SaveToBuffer  )
{
  CCI_Try

    CCI_INSTANCE( ImageInfo )
    if(_This->info.SaveToBuffer())
      CCI_RETURN_OK()

  CCI_Catch(ImageInfo,SaveToBuffer)
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetSize
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, GetSize )
{
  CCI_INSTANCE( ImageInfo )
  const dmRect& r    = _This->info.GetImageRect();
  if(!r.IsEmpty()) {
    CCI_P(width)  = r.Width();
    CCI_P(height) = r.Height();
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: OpenDocument
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, OpenDocument )
{
  CCI_INSTANCE( ImageInfo )
  if(_This->info.OpenDocument(CCI_P(path),_BOOL(CCI_P(bReplace))) ) {
     _This->filename = _This->info.GetPath().FileName();
     CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: SaveDocument
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, SaveDocument )
{
  CCI_INSTANCE( ImageInfo )
  if(_This->info.SaveDocument(CCI_P(path),_BOOL(CCI_P(bReplace))))
     CCI_RETURN_OK()

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: SaveSelection
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo, SaveSelection )
{
  CCI_INSTANCE( ImageInfo )

#ifndef NO_DAIM_USER_INTERFACE
  //==================
  // Get Current ROI
  //==================
  if IS_INTERFACE(_This) {
    daim_manager* manager = THIS_INTERFACE(_This)->SelectManager(DM_ROI,0);
    if(manager)
       manager->OnCommand(cmdGetROI);
  }
#endif // NO_DAIM_USER_INTERFACE

  if(_This->info.SaveRegion(CCI_P(path),_This->info.mROI))
     CCI_RETURN_OK()

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method : SetActive
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo, Select )
{
  CCI_INSTANCE( ImageInfo )
  _This->info.SetActiveInfo(CCI_P(bActive)!=dm_false);
  if(_This->info.IsActiveInfo())
     CCI_RETURN_OK()

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetInfo
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( ImageInfo, GetInfo )
{
  daim_info* info = daim_info::GetInfo();
  if(info && info->mDocument) {
    // Add Ref for the caller
    CCI_AddRef( static_cast<cci_Object*>(info->mDocument->This()) );
    return dm_ptr_to_long(info->mDocument->This());
  }
  return dm_null;
}
//---------------------------------------------------------------------
// Method: GetImageData
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo, GetImageData )
{
  CCI_INSTANCE( ImageInfo )
  dmImage* im = _This->info.GetImage(); 
  if(im) {
   dmImageData imagedata;
   if(im->GetImageData(imagedata)) {
     CCI_P(format) = imagedata.PixelFormat;
     CCI_P(width)  = imagedata.Width;
     CCI_P(height) = imagedata.Height;
     CCI_P(stride) = imagedata.Stride; 
     CCI_P(scan0)  = imagedata.Scan0; 
     CCI_RETURN_OK()
   }
  }
  CCI_RETURN_FAIL()
}   
//---------------------------------------------------------------------
// Method: SetImageData
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageInfo, SetImageData )
{
  CCI_INSTANCE( ImageInfo )

  CCI_ENSURE_PARAM( CCI_P(scan0)  !=  NULL );
  CCI_ENSURE_PARAM( CCI_P(width)  !=  0    );
  CCI_ENSURE_PARAM( CCI_P(height) !=  0    );
 
  dmLink<dmImage> image;

  dmImageData imagedata;
  imagedata.PixelFormat = static_cast<EPixelFormat>(CCI_P(format));
  imagedata.Width       = CCI_P(width);
  imagedata.Height      = CCI_P(height);
  imagedata.Stride      = CCI_P(stride);
  imagedata.Scan0       = CCI_P(scan0);

  dmImageDescriptor* _descriptor = dmGetDescriptor(imagedata.PixelFormat);
  if(_descriptor)
  {
    image = _descriptor->CreateImage(imagedata);
    if(CCI_P(create_copy)) 
       image = image->CreateCopy();
    _This->info.SetImage(image);
    CCI_RETURN_OK()
  } else
    CCI_SET_ERROR_MSG(_TXT("Invalid pixel format !"));

  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method: NewDocument
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageInfo, NewDocument )
{
  dmImageDescriptor* _descriptor = dmGetDescriptor(CCI_P(format));
  if(_descriptor)
  {
    if(CCI_P(width)>0 && CCI_P(height)>0) 
    {
      CCI_INSTANCE( ImageInfo )
      dmLink<dmImage> image = _descriptor->CreateImage(CCI_P(width),CCI_P(height));
      if(!image.IsNull()) 
      {
        image->Clear();

        _This->info.ClearDocument();
        _This->info.SetImage(image);
        CCI_RETURN_OK()
      }
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: CloseDocument
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Document, CloseDocument )
{
  CCI_INSTANCE( ImageInfo )

  _This->info.ClearImage();
  _This->info.ClearDocument();
  CCI_RETURN_OK()
}

//---------------------------------------------------------------------
// Method: Update
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( Document, Update )
{
  CCI_INSTANCE( ImageInfo )

#ifndef NO_DAIM_USER_INTERFACE
  if IS_INTERFACE(_This) {
    THIS_INTERFACE(_This)->UpdateMask(true);
  }
#endif

  _This->info.UpdateObject();
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------

#ifndef NO_DAIM_USER_INTERFACE
//---------------------------------------------------------------------
// Method: ApplyLUT
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD_NP( ImageInfo, ApplyLUT )
{
  CCI_INSTANCE( ImageInfo )
  if( IS_INTERFACE(_This) && THIS_INTERFACE(_This)->ApplyLUT())
    CCI_RETURN_OK()

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: EqualizeLUT
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo, EqualizeLUT )
{
  CCI_INSTANCE( ImageInfo )
  if IS_INTERFACE(_This) {
    THIS_INTERFACE(_This)->EqualizeLUT(CCI_P(bEqualize)!=dm_false);
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: SetCustomLUT
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo, SetCustomLUT )
{
  CCI_INSTANCE( ImageInfo )
  if IS_INTERFACE(_This) {
    THIS_INTERFACE(_This)->SetCustomLUT(CCI_P(lut));
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: OnCommand
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, OnCommand )
{
  CCI_INSTANCE( ImageInfo )
  if IS_INTERFACE(_This) 
  {
    dm_param rv = CCI_ERR_METHOD_FAILED;  

    CCI_Set(_THIS_OBJECT,CCIA_DECL_(Document,Command),CCI_P(cmd));

    if(THIS_INTERFACE(_This)->OnCommand(CCI_P(cmd)))
      rv = CCI_ERR_OK;

    CCI_Set(_THIS_OBJECT,CCIA_DECL_(Document,Command),0);
    return rv;
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: OnCmdUpdate
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, OnCmdUpdate )
{
  CCI_INSTANCE( ImageInfo )
  if IS_INTERFACE(_This) {
    daim_CmdUI& cmdui = _This->cmdui;
    cmdui.nCode   = CCI_P(cmd);
    cmdui.checked = CCI_P(checked);
    if(THIS_INTERFACE(_This)->OnCommandUpdate(&cmdui)) {
      CCI_RETVAL_P(enabled) = _BOOL(cmdui.bEnabled);
      CCI_RETVAL_P(checked) = cmdui.checked;
      CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: GetManager
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, GetManager )
{
  CCI_INSTANCE( ImageInfo )
  if IS_INTERFACE(_This) {
    dmToolManager* pManager = THIS_INTERFACE(_This);
    if(pManager) {
      CCI_RETVAL_P(toolmngr) = pManager;
      CCI_RETURN_OK()
    }
  }
  CCI_P(toolmngr) = NULL;
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: Draw
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, Draw )
{
  CCI_INSTANCE( ImageInfo )
  if IS_INTERFACE(_This) 
  {
    if(CCI_P(flags)==CCIV_Document_DrawBckgnd) {
      if(THIS_INTERFACE(_This)->DrawImage())
        CCI_RETURN_OK()
    } else {
      THIS_INTERFACE(_This)->Display();
      CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: SelectManager
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo, SelectManager )
{
  CCI_INSTANCE( ImageInfo )
  if IS_INTERFACE(_This) {
    dmToolManager* pManager = THIS_INTERFACE(_This)->SelectManager(CCI_P(name),CCI_P(flags));
    if(pManager) {
      CCI_RETVAL_P(toolmngr) = pManager;
      CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: AddListener
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo , AddListener )
{
  CCI_INSTANCE(ImageInfo)

  if IS_INTERFACE(_This) {
    dmIEventListener _Listener;
    if(_Listener.QueryInterface(CCI_P(listener)) &&
       _Listener.AddListener(THIS_INTERFACE(_This),CCI_P(evtMask),CCI_P(flags))) {
  
       CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: RemoveListener
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo , RemoveListener )
{
  CCI_INSTANCE(ImageInfo)

  if IS_INTERFACE(_This) {
    dmIEventListener _Listener;
    if(_Listener.QueryInterface(CCI_P(listener)) &&
       _Listener.RemoveListener(THIS_INTERFACE(_This),CCI_P(evtMask),CCI_P(flags))) {

       CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
#endif //NO_DAIM_USER_INTERFACE
//---------------------------------------------------------------------
// Method: GetProperties
//---------------------------------------------------------------------
#define LONG2STR(fmt,lvalue) (fmt.Format(dmString::npos,"%ld",lvalue),fmt.CStr())
#define DBLE2STR(fmt,lvalue) (fmt.Format(dmString::npos,"%lf",lvalue),fmt.CStr())

static inline CCI_IMPL_METHOD( Document, GetProperties )
{
  CCI_INSTANCE( ImageInfo )

  dmIPropertyList _IPropList;
  if(_IPropList.QueryInterface(CCI_P(propertylist)))
  {
    dm_param key = CCI_P(key);

    dmImage* image = _This->info.GetImage();

    if(image)  
    {
      EPixelFormat _format = image->PixelFormat();

      dm_uint bits      = dmGetPixelFormatBits  (_format);
      dm_uint isIndexed = dmIsPixelFormatIndexed(_format);
      dm_uint isScalar  = dmIsPixelFormatScalar (_format);
      dm_uint isSigned  = dmIsPixelFormatSigned (_format);

      dmString fmt;

      _IPropList.SetProperty(key,NULL,_TXT("image:width"  ),LONG2STR(fmt,image->Width()));
      _IPropList.SetProperty(key,NULL,_TXT("image:height" ),LONG2STR(fmt,image->Height()));
      _IPropList.SetProperty(key,NULL,_TXT("image:bits"   ),LONG2STR(fmt,bits));
      _IPropList.SetProperty(key,NULL,_TXT("image:indexed"),LONG2STR(fmt,isIndexed));
      _IPropList.SetProperty(key,NULL,_TXT("image:scalar" ),LONG2STR(fmt,isScalar));
      _IPropList.SetProperty(key,NULL,_TXT("image:signed" ),LONG2STR(fmt,isSigned));
      _IPropList.SetProperty(key,NULL,_TXT("image:title"  ),_This->info.GetTitle().CStr());
      _IPropList.SetProperty(key,NULL,_TXT("image:path"   ),_This->info.GetPath().CStr());

      if(image->HasUnits())
      {
         dmConstLink<dmSpatialUnits> units = image->Units();

         _IPropList.SetProperty(key,NULL,_TXT("units:name"  ),units->mUnitName.CStr());
         _IPropList.SetProperty(key,NULL,_TXT("units:label" ),units->mUnitLabel.CStr());
         _IPropList.SetProperty(key,NULL,_TXT("units:uppx"  ),DBLE2STR(fmt,units->mUnitsPerPixel));
         _IPropList.SetProperty(key,NULL,_TXT("units:ar"    ),DBLE2STR(fmt,units->mAspectRatio));
      } else {
         _IPropList.SetProperty(key,NULL,_TXT("units:name"  ),"pixels");
         _IPropList.SetProperty(key,NULL,_TXT("units:label" ),"");
         _IPropList.SetProperty(key,NULL,_TXT("units:uppx"  ),"1");
         _IPropList.SetProperty(key,NULL,_TXT("units:ar"    ),"1");
      }

      CCI_RETURN_OK();
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: SetImageFormat
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo, SetImageFormat )
{
  CCI_INSTANCE( ImageInfo )

  dmImage* image = _This->info.GetImage();
  if(image && image->PixelFormat()!=CCI_P(format)) {
    dmLink<dmImage> dst = _This->info.ConvertTo(CCI_P(format));
    if(!dst.IsNull()) {
      _This->info.SetDensityRange(0,0,false); // Reset density range
      _This->info.SetImage(dst);
      _This->info.SetModified(true);
    } else
      CCI_RETURN_FAIL() 
  }

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: SetDisplay
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, SetDisplay )
{
  CCI_UNUSUED_INSTANCE( ImageInfo )
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
#ifndef NO_DAIM_USER_INTERFACE
//---------------------------------------------------------------------
// Method: DoMaskCommand
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo, DoMaskCommand )
{
  CCI_INSTANCE( ImageInfo )

  dmIMaskBuffer _IMaskBuffer;
  if(_IMaskBuffer.QueryInterface(CCI_P(maskbuffers))) 
  {
    switch( CCI_P(command) )
    {
      case cmdCopyMask   : 
       if(!_IMaskBuffer.IsEmptyStack()) {
          _This->info.SetMask(*_IMaskBuffer.TopMask(),0);
          CCI_RETURN_OK()
       }
       break;

      case cmdPopMask    :  
       if(!_IMaskBuffer.IsEmptyStack()) {
          _This->info.SetMask(*_IMaskBuffer.TopMask(),0);
          _IMaskBuffer.PopMask();
          CCI_RETURN_OK()
       }
       break;

      case cmdPushMask   : 
       if(!_This->info.GetMask().IsEmptyRoi()) {
          _IMaskBuffer.PushMask(&_This->info.GetMask());
          CCI_RETURN_OK()
       }
       break;

      case cmdSwapMask   :  
       if(!_IMaskBuffer.IsEmptyStack() && 
          !_This->info.GetMask().IsEmptyRoi()) 
       {
          _IMaskBuffer.SwapMask(&_This->info.GetMask());
          _This->info.ResetMask();
          CCI_RETURN_OK()
       }
       break;

      case cmdRotateMask :  
       if(!_IMaskBuffer.IsEmptyStack() && 
          !_This->info.GetMask().IsEmptyRoi()) 
       {
          _IMaskBuffer.RotateMask(&_This->info.GetMask());
          _This->info.ResetMask();
          CCI_RETURN_OK()
       }
       break;

      case cmdAddMask    :  
       if(!_IMaskBuffer.IsEmptyStack()) {
          _This->info.SetMask(*_IMaskBuffer.TopMask(),dmTk::Math::AddPixels);
          _IMaskBuffer.PopMask();
          CCI_RETURN_OK()
       }
       break;

      case cmdSubMask    :  
       if(!_IMaskBuffer.IsEmptyStack()) {
          _This->info.SetMask(*_IMaskBuffer.TopMask(),dmTk::Math::SubPixels);
          _IMaskBuffer.PopMask();
          CCI_RETURN_OK()
       }
       break;

      case cmdAndMask :  
       if(!_IMaskBuffer.IsEmptyStack()) {
          _This->info.SetMask(*_IMaskBuffer.TopMask(),dmTk::Math::AndPixels);
          _IMaskBuffer.PopMask();
          CCI_RETURN_OK()
       }
       break;

      case cmdXorMask :  
       if(!_IMaskBuffer.IsEmptyStack()) {
          _This->info.SetMask(*_IMaskBuffer.TopMask(),dmTk::Math::XorPixels);
          _IMaskBuffer.PopMask();
          CCI_RETURN_OK()
       }
       break;         

      case cmdSaveMask : 
         _IMaskBuffer.SetMask(&_This->info.GetMask());
         CCI_RETURN_OK()
         break;
       
      case cmdRestoreMask :
         _IMaskBuffer.GetMask(&_This->info.GetMask());
         _This->info.ResetMask();
         CCI_RETURN_OK()
         break;

      case cmdKillMask             :
      case cmdInvertMask           :
      case cmdMaskSelection        :
      case cmdFillHoles            :
      case cmdCleanBorders         :
      case cmdEnableMaskSelection  :
      case cmdDisableMaskSelection :
      case cmdDeleteMaskSelection  :
      case cmdSelectMaskSelection  :
      case cmdResetMaskSelection   :
      if IS_INTERFACE(_This) {
         THIS_INTERFACE(_This)->OnCommand(CCI_P(command));
         CCI_RETURN_OK()       
      }  
      break;
    }
  }  

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method: CreateLayer
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageInfo , CreateLayer )
{
  CCI_INSTANCE(ImageInfo)
  
  if(!EMPTY_STRING(CCI_P(name))) 
  {
    daim_layers* _Layers = THIS_INTERFACE(_This)->GetLayers();
    
    EColorType _Color = DM_INT_TO_COLORREF(CCI_P(color));
   
  
    if(CCI_P(param))
    {
      dmIParameters _IParams;
      if(_IParams.QueryInterface(CCI_P(param))) {
        dmRegion* _Mask = _IParams.GetMask ();
        if(_Mask) 
        {
          _Layers->CreateLayer(CCI_P(name),_Color,*_Mask);
          CCI_RETURN_OK()
        }
      } 
    } else {
      _Layers->SetLayerColor(CCI_P(name),_Color);
      CCI_RETURN_OK()
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
#endif //NO_DAIM_USER_INTERFACE

//---------------------------------------------------------------------
// Method: GetPixelInfo
//---------------------------------------------------------------------
struct _ScalarPixelInfoVisitor
{
  dm_real value;
  dm_int x,y;

  _ScalarPixelInfoVisitor( dm_int _x, dm_int _y ) 
  : x(_x),y(_y) {} 

  template<EPixelFormat _PixelFormat>
  void operator()( const dmIImage<_PixelFormat>& _img ) {
     value = _img.Gen()[y][x];
  }
};
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo, GetPixelInfo )
{
  CCI_INSTANCE( ImageInfo )

  dmImage* image = _This->info.GetImage(); 
  if(image) 
  {
    if(CCI_P(channel) > 0 && 
       CCI_P(x) >= 0 && CCI_P(x) < static_cast<dm_int>(image->Width()) &&
       CCI_P(y) >= 0 && CCI_P(y) < static_cast<dm_int>(image->Height()) )
    {
      if( dmIsPixelFormatScalar(image->PixelFormat()) ) 
      {
        if(CCI_P(channel)==1) {
          _ScalarPixelInfoVisitor _Visitor(CCI_P(x),CCI_P(y));
          if(dmImplementScalarOperation(_Visitor,*image)) {
             CCI_RETVAL_P(value) = _Visitor.value;
             CCI_RETURN_OK()
          }
        }        
      } else 
        if(CCI_P(channel)<=3) {
        dmIImage<dmPixelFormat24bppRGB>* _img = dmIImage<dmPixelFormat24bppRGB>::Cast(image);
        if(_img) {
          const dmRGBColor& rgb = _img->Gen()[CCI_P(y)][CCI_P(x)];
          switch(CCI_P(channel)) {
            case 1: CCI_RETVAL_P(value) = rgb.red;   break;
            case 2: CCI_RETVAL_P(value) = rgb.green; break;
            case 3: CCI_RETVAL_P(value) = rgb.blue ; break;
          }
          CCI_RETURN_OK()
        }
      }
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo, SetUnits )
{
  if(CCI_P(aspectratio)   <= 0 || CCI_P(unitsperpixel) <= 0)
    CCI_RETURN_INVALID_PARAM()

  dm_param rv = CCI_ERR_METHOD_FAILED;

  if(_THIS_OBJECT) // Invoked as object method
  {
    CCI_INSTANCE( ImageInfo )
    dmImage* image = _This->info.GetImage(); 
    if(image) {
      dmSpatialUnits _Units;
      _Units.mUnitsPerPixel = CCI_P(unitsperpixel);
      _Units.mAspectRatio   = CCI_P(aspectratio);
      _Units.mUnitLabel     = CCI_P(unitlabel);
      _Units.mUnitLabel     = CCI_P(unitname);
      rv = CCI_ERR_OK;
    }
  }

  // Set default units
  if(!_THIS_OBJECT || CCI_P(setdefaults))
  {
    dmSpatialUnits& _Units = dmGetServices<dmUnitsPrefs>()->mUnits;
    _Units.mUnitsPerPixel  = CCI_P(unitsperpixel);
    _Units.mAspectRatio    = CCI_P(aspectratio);
    _Units.mUnitLabel      = CCI_P(unitlabel);
    _Units.mUnitName       = CCI_P(unitname);
    rv = CCI_ERR_OK;
  }

  return rv;
}
//---------------------------------------------------------------------
// Method: SelectImage
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo , SelectImage )
{
  CCI_INSTANCE(ImageInfo)

  dmIImageList _ImageList;
  if(_ImageList.QueryInterface(CCI_P(imagelist))) 
  {
    dmLink<dmImage> src;

    _ImageList.GetBufferA(CCI_P(index),src);

    if(!src.IsNull())
    {
      _This->info.SetImage(src);
      CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: StoreImage
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo , StoreImage )
{
  CCI_INSTANCE(ImageInfo)

  dmIImageList _ImageList;
  if(_ImageList.QueryInterface(CCI_P(imagelist))) 
  {
    dmImage* image = _This->info.GetImage(); 
    if(image && _ImageList.StoreBuffer(CCI_P(index),image,NULL,CCI_P(format))) {
      CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: Duplicate
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageInfo , Duplicate )
{
  CCI_INSTANCE(ImageInfo)

  dmIImageInfo _ImageInfo;
  if(_ImageInfo.QueryInterface(CCI_P(sourceinfo))) 
  {
    dmRegion roi;
    dmImage* src = _ImageInfo.GetImage();
   
    if(src) 
    {
      _ImageInfo.GetROI(roi);  
      if(roi.IsEmptyRoi()) 
         roi.SetRectRoi(src->Rect());

      dmLink<dmImage> img = src->CreateCopy(roi.Rectangle());

      if(!roi.IsRectRoi()) {
        roi.XorRoi(roi.Rectangle());
        roi.OffsetRoi();
        img->ClearArea(roi);
      }

      _This->info.SetImage(img);
      CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method: SetRectRoi
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo , SetRectRoi )
{
  CCI_UNUSUED_INSTANCE(ImageInfo)
  
  dmRect r;
  r.top_left.x     = dm_min(CCI_P(left),CCI_P(right) );
  r.top_left.y     = dm_min(CCI_P(top) ,CCI_P(bottom));
  r.bottom_right.x = dm_max(CCI_P(left),CCI_P(right) );
  r.bottom_right.y = dm_max(CCI_P(top) ,CCI_P(bottom));
  
  dmRegion _rgn(r);

  return CCI_Invoke_PP_OK(_This_ob,ImageInfo,SetROI, _in &_rgn);
}
//---------------------------------------------------------------------
// Method: CopyRect
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( ImageInfo , CopyRect )
{
  CCI_INSTANCE(ImageInfo)

  dmIImageInfo _ImageInfo;
  if(_ImageInfo.QueryInterface(CCI_P(sourceinfo))) 
  {
    dmImage* src = _ImageInfo.GetImage();

    if(src) 
    {
      dmRect r;
      r.top_left.x     = dm_min(CCI_P(left),CCI_P(right) );
      r.top_left.y     = dm_min(CCI_P(top) ,CCI_P(bottom));
      r.bottom_right.x = dm_max(CCI_P(left),CCI_P(right) );
      r.bottom_right.y = dm_max(CCI_P(top) ,CCI_P(bottom) );

      if(dmClipRectangle(r,src->Rect())) 
      {
        dmLink<dmImage> img = src->CreateCopy(r);
      
        _This->info.SetImage(img);
        CCI_RETURN_OK()
      }
    }
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
#define IF_EXECCOMMAND( cmd ) \
    if(EQUAL_STRING(cmd_name,cmd))

static inline CCI_IMPL_METHOD( Document , ExecCommand )
{
  #ifdef NO_DAIM_USER_INTERFACE
  
  CCI_UNUSUED_INSTANCE(ImageInfo)

  #else // NO_DAIM_USER_INTERFACE

  CCI_INSTANCE(ImageInfo)
  
  const char_t* cmd_name = CCI_P(command);
  
  IF_EXECCOMMAND("DeleteLayer") {
     THIS_INTERFACE(_This)->GetLayers()->DeleteLayer(CCI_P(arg));
     CCI_RETURN_OK()
  }
  
  IF_EXECCOMMAND("MoveLayerToBack") {
    THIS_INTERFACE(_This)->GetLayers()->MoveLayerToBack(CCI_P(arg));
    CCI_RETURN_OK()
  }
  
  IF_EXECCOMMAND("MoveLayerToFront") {
    THIS_INTERFACE(_This)->GetLayers()->MoveLayerToFront(CCI_P(arg));
    CCI_RETURN_OK()
  }
  
  IF_EXECCOMMAND("MoveLayerBackward") {
    THIS_INTERFACE(_This)->GetLayers()->MoveLayerBackward(CCI_P(arg));
    CCI_RETURN_OK()
  }
  
  IF_EXECCOMMAND("MoveLayerForward") {
    THIS_INTERFACE(_This)->GetLayers()->MoveLayerForward(CCI_P(arg));
    CCI_RETURN_OK()
  }

  IF_EXECCOMMAND("ShowLayers") {
    THIS_INTERFACE(_This)->ShowLayers(EQUAL_STRING(CCI_P(arg),"true")); 
    CCI_RETURN_OK()
  }

  IF_EXECCOMMAND("ClearLayers") {
    THIS_INTERFACE(_This)->KillLayers(EQUAL_STRING(CCI_P(arg),"true")); 
    CCI_RETURN_OK()
  }

  IF_EXECCOMMAND("UpdateLayers") {
    THIS_INTERFACE(_This)->UpdateMask(EQUAL_STRING(CCI_P(arg),"true"));
    CCI_RETURN_OK()
  }

  IF_EXECCOMMAND("GetLayer") {
    dmIParameters _IParams;
    if(_IParams.QueryInterface(CCI_P(param))) 
    {
      daim_layers*    _Layers = THIS_INTERFACE(_This)->GetLayers();
      daim_layer_rgn* _Layer  = _Layers->GetLayer(CCI_P(arg));
      if(_Layer) {
        _IParams.SetMask(&_Layer->Region);
        CCI_RETURN_OK()
      }
    } 
    CCI_RETURN_FAIL()
  }
   
  IF_EXECCOMMAND("ShowMasks") {
    THIS_INTERFACE(_This)->ShowMask(EQUAL_STRING(CCI_P(arg),"true")); 
    CCI_RETURN_OK()
  }

  IF_EXECCOMMAND("SetMaskVisibility") {
    THIS_INTERFACE(_This)->SetMaskVisibility(EQUAL_STRING(CCI_P(arg),"true")); 
    CCI_RETURN_OK()
  }
 
  #endif   // NO_DAIM_USER_INTERFACE
  
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
#define RETURN_EXECCOMMAND_SUPPORTED( cmd ) \
    if(EQUAL_STRING(cmd_name,cmd)) CCI_RETURN_OK();

static inline CCI_IMPL_METHOD( Document , IsCommandSupported )
{
  CCI_UNUSUED_INSTANCE(Document)

  #ifndef NO_DAIM_USER_INTERFACE

  const char_t* cmd_name = CCI_P(command);
  
  RETURN_EXECCOMMAND_SUPPORTED("CreateLayer");
  RETURN_EXECCOMMAND_SUPPORTED("DeleteLayer");
  RETURN_EXECCOMMAND_SUPPORTED("ShowLayers");
  RETURN_EXECCOMMAND_SUPPORTED("ClearLayers");
  RETURN_EXECCOMMAND_SUPPORTED("UpdateLayers");
  RETURN_EXECCOMMAND_SUPPORTED("SetLayerProperty");
  RETURN_EXECCOMMAND_SUPPORTED("MoveLayerToBack");
  RETURN_EXECCOMMAND_SUPPORTED("MoveLayerToFront");
  RETURN_EXECCOMMAND_SUPPORTED("MoveLayerBackward");
  RETURN_EXECCOMMAND_SUPPORTED("MoveLayerForward");
  RETURN_EXECCOMMAND_SUPPORTED("ShowMasks");
  RETURN_EXECCOMMAND_SUPPORTED("SetMaskVisibility");

  #endif   // NO_DAIM_USER_INTERFACE
  
  CCI_RETURN_INVOKE_SUPER()
  
}
//---------------------------------------------------------------------
// Method : SetCursorType
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( Document, SetCursorType )
{
  #ifndef NO_DAIM_USER_INTERFACE
   if(CCI_P(cursor) == 0) {
      CCI_P(cursor) = eCursor_crosshair;
   }
  #endif   // NO_DAIM_USER_INTERFACE
  CCI_RETURN_INVOKE_SUPER()
}
//---------------------------------------------------------------------
// Method : GetImageA
//---------------------------------------------------------------------
static inline CCI_IMPL_METHOD( ImageInfo, GetImageA )
{
  CCI_INSTANCE( ImageInfo )

  if( CCI_P(link)!=NULL )
  {
    switch(CCI_P(me))
    {
      case CCI_ME_GET:
        *CCI_P(link) = _This->info.GetImage();
         CCI_RETURN_OK()

      case CCI_ME_SET:
        _This->info.SetImage( *CCI_P(link) );
        CCI_RETURN_OK()
    }
  }
  
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( ImageInfo )
    CCI_REGISTER_METHOD_PP( Document, GetSize )
    CCI_REGISTER_METHOD_NP( Document, Update  )

#ifndef NO_DAIM_USER_INTERFACE

    CCI_REGISTER_METHOD_PP( Document, GetManager  )
    CCI_REGISTER_METHOD_PP( Document, Draw        )
    CCI_REGISTER_METHOD_PP( Document, OnCommand   ) 
    CCI_REGISTER_METHOD_PP( Document, OnCmdUpdate )

    CCI_REGISTER_METHOD_PP( ImageInfo, SelectManager )

    CCI_REGISTER_METHOD_NP( ImageInfo, ApplyLUT     )
    CCI_REGISTER_METHOD_PP( ImageInfo, EqualizeLUT  )
    CCI_REGISTER_METHOD_PP( ImageInfo, SetCustomLUT )    

#endif //NO_DAIM_USER_INTERFACE

    CCI_REGISTER_METHOD_PP( Document, SetDisplay    )
    CCI_REGISTER_METHOD_PP( Document, OpenDocument  )    
    CCI_REGISTER_METHOD_PP( Document, SaveDocument  )    
    CCI_REGISTER_METHOD_NP( Document, CloseDocument )

    CCI_REGISTER_METHOD_NP( ImageInfo, Refresh  )
    CCI_REGISTER_METHOD_PP( ImageInfo, SetMask  )
    CCI_REGISTER_METHOD_PP( ImageInfo, GetMask  )
    CCI_REGISTER_METHOD_PP( ImageInfo, SetROI   )
    CCI_REGISTER_METHOD_PP( ImageInfo, GetROI   )

    CCI_REGISTER_METHOD_NP( ImageInfo, RoiToMask )
    CCI_REGISTER_METHOD_NP( ImageInfo, MaskToRoi )

    CCI_REGISTER_METHOD_NP( ImageInfo, RestoreFromBuffer )
    CCI_REGISTER_METHOD_NP( ImageInfo, SaveToBuffer      )

    CCI_REGISTER_METHOD_PP( ImageInfo, SetDensityRange )
    CCI_REGISTER_METHOD_PP( ImageInfo, GetDensityRange )

    CCI_REGISTER_METHOD_PP( ImageInfo, MarkModified  )
    CCI_REGISTER_METHOD_NP( ImageInfo, GetInfo      )
    CCI_REGISTER_METHOD_PP( ImageInfo, Select       )
    CCI_REGISTER_METHOD_PP( ImageInfo, GetImageData )
    CCI_REGISTER_METHOD_PP( ImageInfo, SetImageData )
    CCI_REGISTER_METHOD_PP( ImageInfo, NewDocument  )

    CCI_REGISTER_METHOD_PP( Document, GetProperties )

    CCI_REGISTER_METHOD_PP( ImageInfo, SetImageFormat)
    CCI_REGISTER_METHOD_PP( ImageInfo, SaveSelection )

#ifndef NO_DAIM_USER_INTERFACE

    CCI_REGISTER_METHOD_PP( ImageInfo, AddListener   )
    CCI_REGISTER_METHOD_PP( ImageInfo, RemoveListener)
    CCI_REGISTER_METHOD_PP( ImageInfo, DoMaskCommand )
    CCI_REGISTER_METHOD_PP( ImageInfo, CreateLayer   )

#endif //NO_DAIM_USER_INTERFACE

    CCI_REGISTER_METHOD_PP( ImageInfo, GetPixelInfo )
    CCI_REGISTER_METHOD_PP( ImageInfo, SetUnits     )

    CCI_REGISTER_METHOD_PP( ImageInfo,  SelectImage )
    CCI_REGISTER_METHOD_PP( ImageInfo,  StoreImage  )
    CCI_REGISTER_METHOD_PP( ImageInfo,  Duplicate   )

    CCI_REGISTER_METHOD_PP( ImageInfo,  SetRectRoi )
    CCI_REGISTER_METHOD_PP( ImageInfo,  CopyRect   )

    CCI_REGISTER_METHOD_PP( Document, ExecCommand        )
    CCI_REGISTER_METHOD_PP( Document, IsCommandSupported )
    
    CCI_REGISTER_METHOD_PP( Document, SetCursorType      )

    CCI_REGISTER_METHOD_PP( ImageInfo, GetImageA )
 
CCI_NEXT_DISPATCH_MAP()

//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( ImageInfo )
{  
  CCI_INVOKE_FACTORY(ImageInfo)
}
//---------------------------------------------------------------------
