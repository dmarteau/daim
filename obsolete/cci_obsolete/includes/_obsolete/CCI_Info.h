#ifndef CCI_Info_h
#define CCI_Info_h

/* ::: BEGIN LICENSE BLOCK:::
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
 *  ::: END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : CCI_Info.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_ImageInfo  _TXT("imageinfo.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( ImageInfo, Document )
  
  CCI_MODULE_UUID( ImageInfo, a652e6d1-d960-4edd-9f05-6b9ace549db8)

  CCI_MODULE_VERSION( ImageInfo, 3 ) 

  /* Methods */
  CCI_DECL_METHOD_NP(ImageInfo, 1,  Refresh           ) 
  CCI_DECL_METHOD_PP(ImageInfo, 2,  SetMask           , _in CONST dmRegion* region _in dm_int operation )
  CCI_DECL_METHOD_PP(ImageInfo, 3,  GetMask           , _in dmRegion* region        )
  CCI_DECL_METHOD_PP(ImageInfo, 5,  SetROI            , _in CONST dmRegion* region  )
  CCI_DECL_METHOD_PP(ImageInfo, 6,  GetROI            , _in dmRegion* region        )
  CCI_DECL_METHOD_NP(ImageInfo, 7,  RestoreFromBuffer )
  CCI_DECL_METHOD_NP(ImageInfo, 8,  SaveToBuffer      )
  CCI_DECL_METHOD_NP(ImageInfo,10,  ApplyLUT          )
  CCI_DECL_METHOD_PP(ImageInfo,11,  GetDensityRange   , _out dm_real rmin _out dm_real rmax )
  CCI_DECL_METHOD_PP(ImageInfo,12,  SetDensityRange   , _in  dm_real rmin _in  dm_real rmax )
  CCI_DECL_METHOD_PP(ImageInfo,14,  EqualizeLUT       , _in  dm_uint  bEqualize )
  CCI_DECL_METHOD_PP(ImageInfo,15,  SetCustomLUT      , _in  dmLUT* lut )
  CCI_DECL_METHOD_NP(ImageInfo,16,  GetInfo           )
  CCI_DECL_METHOD_PP(ImageInfo,17,  SelectManager     , _out dmToolManager* toolmngr _in CONST char_t* name _in dm_uint flags )
  CCI_DECL_METHOD_PP(ImageInfo,18,  MarkModified      , _in  dm_uint bModified )
  CCI_DECL_METHOD_PP(ImageInfo,19,  Select            , _in  dm_uint bActive   )

  CCI_DECL_METHOD_PP(ImageInfo,21,  GetImageData      , 
      _out dm_int  format 
      _out dm_uint width  
      _out dm_uint height 
      _out dm_int  stride 
      _out dm_ptr  scan0
  )

  CCI_DECL_METHOD_PP(ImageInfo,22,  SetImageData      , 
      _in dm_int  format 
      _in dm_uint width 
      _in dm_uint height 
      _in dm_int  stride 
      _in dm_ptr  scan0 
      _in dm_bool create_copy
  )

  CCI_DECL_METHOD_NP(ImageInfo,23,  RoiToMask )
  CCI_DECL_METHOD_NP(ImageInfo,24,  MaskToRoi )

  CCI_DECL_METHOD_PP(ImageInfo,25,  NewDocument, 
      _in EPixelFormat format
      _in dm_uint      width
      _in dm_uint      height 
  )

  CCI_DECL_METHOD_PP( ImageInfo,26, SetImageFormat, _in EPixelFormat format )
  CCI_DECL_METHOD_PP( ImageInfo,27, SaveSelection , _in CONST char_t* path  )

  CCI_DECL_METHOD_PP( ImageInfo, 28, AddListener, 
     _in dm_cci(EventListener) listener 
     _in dm_uint               evtMask 
     _in dm_uint               flags 
  )

  CCI_DECL_METHOD_PP( ImageInfo, 29, RemoveListener , 
     _in dm_cci(EventListener)  listener 
     _in dm_uint                evtMask 
     _in dm_uint                flags 
  )


  CCI_DECL_METHOD_PP( ImageInfo, 30, DoMaskCommand , 
     _in dm_cci(MaskBuffer)  maskbuffers 
     _in dm_int              command 
  )

  /* Never use it to loop over image pixels : it's slow !!! */
  CCI_DECL_METHOD_PP( ImageInfo, 31, GetPixelInfo , 
     _in  dm_uint  channel         
     _in  dm_int   x 
     _in  dm_int   y 
     _out dm_real  value
  )

  /* This method can be invoked as service to set defaults units */
  CCI_DECL_METHOD_PP( ImageInfo, 32, SetUnits , 
     _in  dm_real        unitsperpixel
     _in  dm_real        aspectratio
     _in  CONST char_t*  unitname         
     _in  CONST char_t*  unitlabel         
     _in  dm_bool        setdefaults        
  )


  /* Image will be shared width the original list */
  CCI_DECL_METHOD_PP( ImageInfo, 33, SelectImage , 
     _in dm_cci(ImageList) imagelist 
     _in dm_uint           index
  )

  CCI_DECL_METHOD_PP( ImageInfo, 34, Duplicate , 
     _in dm_cci(ImageInfo) sourceinfo 
     _in dm_bool           useselection
  )

  CCI_DECL_METHOD_PP( ImageInfo, 35, StoreImage , 
     _in dm_cci(ImageList) imagelist 
     _in dm_uint           index
     _in dm_int            format
  )

  CCI_DECL_METHOD_PP( ImageInfo, 36, SetRectRoi , 
     _in dm_int left
     _in dm_int top
     _in dm_int right
     _in dm_int bottom
  )

  CCI_DECL_METHOD_PP( ImageInfo, 37, CopyRect , 
     _in dm_cci(ImageInfo) sourceinfo 
     _in dm_int left
     _in dm_int top
     _in dm_int right
     _in dm_int bottom
  )

  CCI_DECL_METHOD_PP( ImageInfo, 38, CreateLayer , 
     _in CONST char_t* name
     _in dm_param      color
     _in cci_Object*   param
  )

  #ifdef DAIM_UTILITIES
  /* Use shared link for efficiency */
  CCI_DECL_METHOD_PP(ImageInfo,39, GetImageA, 
     _in dm_param         me
     _in dmLink<dmImage>* link
  )
  #endif // DAIM_UTILITIES

  /* Attributs */
  CCI_DECL_ATTR(ImageInfo, 1,  Title         , char_t*        ,[.SGN] )
  CCI_DECL_ATTR(ImageInfo, 2,  Image         , dmImage*       ,[..G.] )
  CCI_DECL_ATTR(ImageInfo, 3,  Activate      , dm_bool        ,[.SGN] )
  CCI_DECL_ATTR(ImageInfo, 4,  Buffer        , dmImageBuffer* ,[..G.] )
  CCI_DECL_ATTR(ImageInfo, 5,  MaskColor     , EColorType     ,[ISGN] )
  CCI_DECL_ATTR(ImageInfo, 6,  ExMaskColor   , EColorType     ,[ISGN] )
  CCI_DECL_ATTR(ImageInfo, 7,  MaskUpdate    , dm_bool        ,[.SGN] )
  CCI_DECL_ATTR(ImageInfo, 8,  MaskSize      , dm_uint        ,[..G.] )
  CCI_DECL_ATTR(ImageInfo, 9,  Path          , char_t*        ,[..G.] )
  CCI_DECL_ATTR(ImageInfo,10,  LUT           , dmLUT*         ,[..G.] )
  CCI_DECL_ATTR(ImageInfo,11,  DocumentType  , dm_uint        ,[I.G.] )
  CCI_DECL_ATTR(ImageInfo,12,  DocAttributs  , dm_uint        ,[..G.] )
  CCI_DECL_ATTR(ImageInfo,13,  LastDocError  , dm_int         ,[..G.] )
  CCI_DECL_ATTR(ImageInfo,14,  IsValidMask   , dm_bool        ,[..G.] )
  CCI_DECL_ATTR(ImageInfo,15,  ImageFormat   , dm_uint        ,[..G.] )
  CCI_DECL_ATTR(ImageInfo,16,  Channels      , dm_uint        ,[..G.] )
  CCI_DECL_ATTR(ImageInfo,17,  CurrentManager, char_t*        ,[..G.] )

  /* Special values */

  #define CCIV_ImageInfo_SimpleDocument 0L
  #define CCIV_ImageInfo_UserInterface  1L

  #define CCIV_ImageInfo_Info ((dmImage*)~0L)



CCI_END_MODULE_DECL( ImageInfo )

#endif // __doxygen
//----------------------------------------------------------------

#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Daim image document interface
 *
 *  Interface for image document.
 */
//------------------------------------------------------------------------------------
class dmIImageInfo : public dmIDocument
{    
  public:
   dmDECLARE_GENERIC_IINTERFACE(ImageInfo)

   //--------------------------------------------------------------
   /*! \brief Select the document as the default image document
    *  for processing instructions.
    */
   //-------------------------------------------------------
   bool Select( bool active )  { CCI_RETURN_IMETHOD_PP_OK(ImageInfo,Select, _in _BOOL(active) ) }

   //--------------------------------------------------------------
   /*! \brief Refresh the display (if any) 
    *
    * Note that this method only redraw the document. If more updating
    * is required ( like some processing instructions ) use
    * the dmIDocument::Update method.
    */
   //-------------------------------------------------------
   bool Refresh() { CCI_RETURN_IMETHOD_NP_OK(ImageInfo,Refresh) }

   //--------------------------------------------------------------
   /*! \brief Mark document as modified [Notify]                 */
   //--------------------------------------------------------------
   bool SetModified(bool modified) { 
     CCI_RETURN_IMETHOD_PP_OK(ImageInfo,MarkModified, _in _BOOL(modified) ) 
   }

   //--------------------------------------------------------------
   /*! \brief Kill/Remove the current Mask                        */
   //--------------------------------------------------------------
   bool KillMask() { CCI_RETURN_IMETHOD_PP_OK(ImageInfo,SetMask , _in NULL) }     

   //--------------------------------------------------------------
   /*! \brief Set the current mask associated to the image       */
   //--------------------------------------------------------------
   bool SetMask( const dmRegion& region, dm_int operation=0 ) { 
     CCI_RETURN_IMETHOD_PP_OK(ImageInfo,SetMask, _in &region _in operation )
   }

   //--------------------------------------------------------------
   /*! \brief Get a copy of the current mask                      */
   //--------------------------------------------------------------
   bool GetMask( dmRegion& region )   { CCI_RETURN_IMETHOD_PP_OK(ImageInfo,GetMask, _in &region ) }

   //--------------------------------------------------------------
   /*! \brief Check if there is an valid current mask             */
   //--------------------------------------------------------------
   bool HasMask() { CCI_RETURN_IMETHOD_PP_OK(ImageInfo,GetMask, _in NULL ) }

   //--------------------------------------------------------------
   /*! \brief Kill/Remove the current ROI                        */
   //--------------------------------------------------------------
   bool KillRoi() { CCI_RETURN_IMETHOD_PP_OK(ImageInfo,SetROI , _in NULL) }     

   //--------------------------------------------------------------
   /*! \brief Use \a rgn as the current ROI                      */
   //--------------------------------------------------------------
   bool SetROI( const dmRegion& rgn ) { CCI_RETURN_IMETHOD_PP_OK(ImageInfo,SetROI , _in &rgn) }

   //--------------------------------------------------------------
   /*! \brief Get a copy of current ROI                          */
   //--------------------------------------------------------------
   bool GetROI( dmRegion& rgn )  { CCI_RETURN_IMETHOD_PP_OK(ImageInfo,GetROI , _in &rgn) }

   //--------------------------------------------------------------
   /*! \brief Check if there is any ROI currently active         */
   //--------------------------------------------------------------
   bool HasROI() { CCI_RETURN_IMETHOD_PP_OK(ImageInfo,GetROI , _in NULL) }

   //--------------------------------------------------------------
   /*! \brief Transform the active roi into current mask         */
   //--------------------------------------------------------------
   bool RoiToMask() { CCI_RETURN_IMETHOD_NP_OK(ImageInfo,RoiToMask) }

   //--------------------------------------------------------------
   /*! \brief Transform the current mask into active roi         */
   //--------------------------------------------------------------
   bool MaskToRoi() { CCI_RETURN_IMETHOD_NP_OK(ImageInfo,MaskToRoi) }

   //--------------------------------------------------------------
   /*! \brief Return the image buffer
    *  See \ref daim_processing for description of buffers.
    */
   //--------------------------------------------------------------
   dmImageBuffer* GetImageBuffer() { return CCI_IGET(ImageInfo,Buffer,(dmImageBuffer*)NULL); }

   //--------------------------------------------------------------
   /*! \brief Return the current image */
   //--------------------------------------------------------------
   dmImage* GetImage() { return CCI_IGET(ImageInfo,Image ,(dmImage*)NULL);       } 

   //--------------------------------------------------------------
   /*! \brief Return the current title of the document */
   //--------------------------------------------------------------
   const char_t* GetTitle() { return CCI_IGET(ImageInfo,Title,(char_t*)NULL); } 

   //--------------------------------------------------------------
   /*! \brief Return the current url of the document.
    *  see dmIDocument::OpenDocument for a description of 
    *  url specification
    */
   //--------------------------------------------------------------
   const char_t* GetPath() { return CCI_IGET(ImageInfo,Path ,(char_t*)NULL); } 
     
   //--------------------------------------------------------------
   /*! \brief Restore the image with the content of the buffer
    *  See \ref daim_processing for description of buffers.
    */
   //--------------------------------------------------------------
   bool RestoreFromBuffer() { CCI_RETURN_IMETHOD_NP_OK(ImageInfo,RestoreFromBuffer) } 

   //--------------------------------------------------------------
   /*! \brief Save whole image to the buffer
    *  See \ref daim_processing for description of buffers.
    */
   //--------------------------------------------------------------
   bool SaveToBuffer() { CCI_RETURN_IMETHOD_NP_OK(ImageInfo,SaveToBuffer     ) } 

   dm_uint GetMaskSize()                      { return CCI_IGET(ImageInfo,MaskSize,0);   }
   dm_uint GetMaskColor()                     { return CCI_IGET(ImageInfo,MaskColor,-1); }
   bool    SetMaskColor( dmTk::EColorType c ) { return CCI_ISET(ImageInfo,MaskColor,c);  }

   bool SetDensityRange( dm_real  rmin, dm_real  rmax ) { 
     CCI_RETURN_IMETHOD_PP_OK(ImageInfo,SetDensityRange, _in rmin _in rmax ) 
   }

   bool GetDensityRange( dm_real& rmin, dm_real& rmax ) {
     IF_CCI_IMETHOD_RP(ImageInfo,GetDensityRange, _in 0 _in 0 ) THEN
       CCI_RETURN_P(rmin)
       CCI_RETURN_P(rmax)
     ENDIF_RETURN_BOOL()
   }

   //--------------------------------------------------------------
   /*! \brief Set image pixels data
    *  \param data         A dmImageData object holding pixels buffer infos. 
    *  \param create_copy  If set to \c true: create a copy of 
    *                      the passed data.
    *                      If set to \c false: do not create a copy of the input
    *                      data. Instead, create a handle pointing to 
    *                      the input pixels buffer. USE WITH CAUTION, as deallocating
    *                      the buffer while still in use by the dmIImageInfo object
    *                      may cause very weirds and bad things append !!
    *                      
    *  \note Any transformation involving buffer reallocation will break the link 
    *        between input data and the internal handle.               
    */ 
   //--------------------------------------------------------------
   bool SetImageData( const dmImageData& data , bool create_copy ) {
     CCI_RETURN_IMETHOD_PP_OK(ImageInfo,SetImageData, 
       _in static_cast<dm_int>(data.PixelFormat)
       _in data.Width 
       _in data.Height 
       _in data.Stride 
       _in data.Scan0 
       _in _BOOL(create_copy) 
     )
   }

   //--------------------------------------------------------------
   /*! \brief Get image pixels data
    *  \param data         A dmImageData object that will hold pixels buffer infos. 
    *                      
    *  \note Any transformation involving buffer reallocation will result
    *        in invalid data !
    */ 
   //--------------------------------------------------------------
   bool GetImageData( dmImageData& data ) {
     IF_CCI_IMETHOD_RP(ImageInfo,GetImageData,_out(format) 0 _out(width) 0 _out(height) 0 _out(stride) 0 _out(scan0) 0 )
     THEN
       data.Width       = CCI_RETVAL(width);
       data.Height      = CCI_RETVAL(height);
       data.Stride      = CCI_RETVAL(stride);
       data.Scan0       = CCI_RETVAL(scan0); 
       data.PixelFormat = static_cast<EPixelFormat>(CCI_RETVAL(format));
     ENDIF_RETURN_BOOL()
   }

   //--------------------------------------------------------------
   /*! \brief Create a new image document */
   //--------------------------------------------------------------
   bool NewDocument( EPixelFormat format, dm_uint width, dm_uint height ) {
     CCI_RETURN_IMETHOD_PP_OK(ImageInfo,NewDocument, _in format _in width _in height ) 
   }
  
   bool SetMaskSelection( int x, int y, int st);
   bool ClearMaskSelection( bool bExclude );

   dmLUT* GetLUT()  { return CCI_IGET(ImageInfo,LUT   ,(dmLUT*)NULL); } 
   
   bool ApplyLUT()  { CCI_RETURN_IMETHOD_NP_OK(ImageInfo,ApplyLUT) }

   bool EqualizeLUT ( bool   bEqu ) { CCI_RETURN_IMETHOD_PP_OK(ImageInfo,EqualizeLUT , _in _BOOL(bEqu)) }
   bool SetCustomLUT( dmLUT* pLut ) { CCI_RETURN_IMETHOD_PP_OK(ImageInfo,SetCustomLUT, _in pLut)  }

   //--------------------------------------------------------------
   /*! \brief Change the format of the image */
   //--------------------------------------------------------------
   bool SetImageFormat( EPixelFormat format ) {
      CCI_RETURN_IMETHOD_PP_OK(ImageInfo,SetImageFormat,_in format)
   }

   //--------------------------------------------------------------
   /*! \brief Select the active manager 
    *  \a flags is a combination of the following values:
    *    - DM_Manager_Select : activate the manager in the foreground 
    *    - DM_Manager_Create : create the manager if it does not exists
    */
   //--------------------------------------------------------------
   dmToolManager* SelectManager( const char_t* name, dm_uint flags ) {
     IF_CCI_IMETHOD_RP(ImageInfo,SelectManager,_out(toolmngr) 0 _in name _in flags )
     THEN return CCI_RETVAL(toolmngr);
     ELSE return NULL;
     ENDIF
   }

   //--------------------------------------------------------------
   /*! \brief Save image part corresponding to the current roi */
   //--------------------------------------------------------------
   bool SaveSelection( const char_t* path ) {
     CCI_RETURN_IMETHOD_PP_OK( ImageInfo,SaveSelection, _in path )
   }

  //-------------------------------------------------------
  /*! \brief Add an event listener to the attached manager.
   *
   * \see dmIEventListener::AddListener() 
   * for a description of the parameer
   */
  //-------------------------------------------------------
  bool AddListener( dm_cci(EventListener) listener, dm_uint evtMask, dm_uint flags ) {
    CCI_RETURN_IMETHOD_PP_OK(ImageInfo,AddListener,_in listener _in evtMask _in flags )
  }

  //-------------------------------------------------------
  /*! \brief Remove an event listener from he attached manager.
   *
   * \see dmIEventListener::RemoveListener() 
   * for a description of the parameters
   */
  //-------------------------------------------------------
  bool RemoveListener( dm_cci(EventListener) listener, dm_uint evtMask, dm_uint flags ) {
    CCI_RETURN_IMETHOD_PP_OK(ImageInfo,RemoveListener,_in listener _in evtMask _in flags )
  }

  //--------------------------------------------------------------
  /*! \brief Return the pixel format of the image */
  //--------------------------------------------------------------
  EPixelFormat GetImageFormat() {
     return CCI_IGET(ImageInfo,ImageFormat ,dmPixelFormatUndefined); 
  }

  //--------------------------------------------------------------
  /*! \brief Set units for the current document
   *
   *  This method can be invoked as service to set defaults units,
   *  in this case \a setdefaults is ignored.
   */
  //--------------------------------------------------------------
  bool SetUnits( dm_real        unitsperpixel,
                 dm_real        aspectratio,
                 CONST char_t*  unitname,
                 CONST char_t*  unitlabel,
                 dm_bool        setdefaults )     
  {
    CCI_RETURN_IMETHOD_PP_OK( ImageInfo, SetUnits , 
       _in unitsperpixel
       _in aspectratio
       _in unitname         
       _in unitlabel         
       _in setdefaults        
    )
  }

  //--------------------------------------------------------------
  /*! \brief Replace the current image with the buffer at \a index */
  //--------------------------------------------------------------
  bool SelectImage( dm_cci(ImageList) imagelist, dm_uint index ) {
    CCI_RETURN_IMETHOD_PP_OK(ImageInfo,SelectImage,_in imagelist _in index )
  }

  //--------------------------------------------------------------
  /*! \brief Store current image at buffer index \a index */
  //--------------------------------------------------------------
  bool StoreImage( dm_cci(ImageList) imagelist, dm_uint index, dm_int format = 0) {
    CCI_RETURN_IMETHOD_PP_OK(ImageInfo,StoreImage,_in imagelist _in index  _in format )
  }

  //--------------------------------------------------------------
  /*! \brief Duplicate image */
  //--------------------------------------------------------------
  bool Duplicate( dm_cci(ImageInfo) sourceinfo, dm_bool useselection ) {
    CCI_RETURN_IMETHOD_PP_OK(ImageInfo,Duplicate,_in sourceinfo _in useselection )
  }

  //--------------------------------------------------------------
  /*! \brief Set rectangular ROI */
  //--------------------------------------------------------------
  bool SetRectRoi( dm_int left, dm_int top, dm_int right, dm_int bottom ) {
    CCI_RETURN_IMETHOD_PP_OK(ImageInfo,SetRectRoi,_in left _in top _in right _in bottom )
  }

  //--------------------------------------------------------------
  /*! \brief Duplicate a rectangular area of the source image */
  //--------------------------------------------------------------
  bool CopyRect( dm_cci(ImageInfo) sourceinfo, 
                 dm_int top  , dm_int left, 
                 dm_int right, dm_int bottom ) 
  {
    CCI_RETURN_IMETHOD_PP_OK(ImageInfo,CopyRect,_in sourceinfo 
                     _in top _in left _in right _in bottom)
  }
  
  bool GetPixelInfo( dm_uint channel, dm_int x, dm_int y, dm_real& value )
  {
    IF_CCI_IMETHOD_RP(ImageInfo,GetPixelInfo, _in channel _in x _in y _out(value) 0 ) THEN
      CCI_RETURN_P(value)
    ENDIF_RETURN_BOOL()
  }  

   #ifdef DAIM_UTILITIES

   bool GetImageA( dmLink<dmImage>& link ) {
     CCI_RETURN_IMETHOD_PP_OK(ImageInfo,GetImageA, _in CCI_ME_GET  _in &link ) 
   }

   bool SetImageA(  dmLink<dmImage>& link ) {
     CCI_RETURN_IMETHOD_PP_OK(ImageInfo,GetImageA, _in CCI_ME_SET  _in &link ) 
   }

   #endif // DAIM_UTILITIES

};
//------------------------------------------------------------------------------------
// This class will get the current active info and embed it in a dmIImageInfo
//------------------------------------------------------------------------------------
class dmINFO : public dmIImageInfo
{
  public:
     dmINFO() {}
    ~dmINFO() {}

     bool GetInfo()  {
       if(IsValid()) Release();
       cci_Class*  cl  = CCI_FindPubClass(dmIImageInfo::CCI_CLSID(),dmIImageInfo::VERSION());   
       cci_Object* obj = reinterpret_cast<cci_Object*>(CCI_InvokeService_NP(cl,ImageInfo,GetInfo));
       CCI_UnLock(cl);
       if(obj) {
          Attach(obj); 
          return true; 
       }
       return false;
     }

    void ReleaseInfo() {
      if(IsValid()) Release();
      Detach();
    }
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Info.h */
