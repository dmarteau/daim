#ifndef CCI_Graphics_h
#define CCI_Graphics_h

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
// File         : CCI_Graphics.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_Graphics  _TXT("graphics.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Graphics, Notify )

  CCI_MODULE_UUID( Graphics, 0884f363-ec4e-4fba-b366-e57eb51999a3)

  CCI_MODULE_VERSION( Graphics, 1 )

  /* Methods */

  CCI_DECL_METHOD_PP(Graphics,1,  PropertyValue, 
         _in dm_param      me
         _in CONST char_t* propertyName 
         _in CONST char_t* value 
  )

  CCI_DECL_METHOD_PP(Graphics,2,  GetPropertyValues,
         _in CONST char_t*      propertyName 
         _in dm_cci(StringList) valueList
  )

  CCI_DECL_METHOD_PP(Graphics,3,  Draw, 
         _in  void*          nativeDeviceContext
         _in  CONST dm_rect* boundRect
         _in  dm_uint        paintFlags
  )

  CCI_DECL_METHOD_NP(Graphics,4,  UpdateCursor )
  CCI_DECL_METHOD_PP(Graphics,5,  Document,
         _in dm_param         me 
         _in dm_cci(Document) document 
  )

  CCI_DECL_METHOD_PP(Graphics,6, InitGraphics, 
         _in void*       nativeWindow 
         _in dmGraphics* nativeGraphics 
  )

  CCI_DECL_METHOD_PP(Graphics,7, ProcessSystemEvent,
    _in dm_uint strType
    _in dm_uint evtType
    _in dm_uint button
    _in dm_uint keyQual
    _in dm_uint keyCode
    _in dm_int  cursorX
    _in dm_int  cursorY
  )

  CCI_DECL_METHOD_PP(Graphics, 8,  SetDisplay, 
    _in  dm_int  left 
    _in  dm_int  top
    _in  dm_uint width 
    _in  dm_uint height 
  )


  CCI_DECL_METHOD_PP(Graphics, 9,  SetCursorType, _in dm_param cursor )

  CCI_DECL_METHOD_PP(Graphics, 10,  PrintDocument, 
    _in  CONST char_t*  output 
    _in  dm_int         left 
    _in  dm_int         top
    _in  dm_int         right 
    _in  dm_int         bottom 
  )

  /* Not implemented : interface description only */

  CCI_DECL_METHOD_PP(Graphics,21,  SetPropertyValue, 
         _in CONST char_t* propertyName 
         _in CONST char_t* value 
  )
  /* Not implemented : interface description only */

  CCI_DECL_METHOD_PP(Graphics,22,  GetPropertyValue, 
         _in  CONST char_t* propertyName 
         _out CONST char_t* value 
  )


  /* Attributs */
  
  CCI_DECL_ATTR(Graphics,1, NativeWindow , void*      , [..G.] )
  CCI_DECL_ATTR(Graphics,2, PageScale    , dm_param   , [.SGN] )
  CCI_DECL_ATTR(Graphics,3, Graphics     , dmGraphics*, [..G.] )
  CCI_DECL_ATTR(Graphics,4, ScreenX      , dm_int     , [..G.] )
  CCI_DECL_ATTR(Graphics,5, ScreenY      , dm_int     , [..G.] )
  CCI_DECL_ATTR(Graphics,6, StopCapture  , dm_bool    , [.SGN] )
  CCI_DECL_ATTR(Graphics,7, CursorType   , dm_param   , [..G.] )


CCI_END_MODULE_DECL( Graphics )

#endif // __doxygen
//----------------------------------------------------------------
#ifdef __cplusplus

#include "CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Graphic base interface
 *
 *  This interface is the base class for implementors of native image factory
 *  and display methods.
 */
//------------------------------------------------------------------------------------
class dmIGraphics : public dmINotify
{    
  public:
   dmDECLARE_GENERIC_IINTERFACE(Graphics)

   //----------------------------------------------------------------------------
   /*! \brief Set the scale factor used for rendering.
    *
    *  \see SetDisplay()
    */
   //-----------------------------------------------------------------------------
   bool SetPageScale( dm_uint pageScale ) { return CCI_ISET(Graphics,PageScale,pageScale); } 

   //----------------------------------------------------------------------------
   /*! \brief Return the scale factor used for rendering */
   //-----------------------------------------------------------------------------
   dm_uint GetPageScale() { return CCI_IGET(Graphics,PageScale,(dm_uint)0);   } 

   //----------------------------------------------------------------------------
   /*! \brief Return a handle to the native window associated to this Graphics object */
   //-----------------------------------------------------------------------------
   void*  GetNativeWindow() { return CCI_IGET(Graphics,NativeWindow,(void*)NULL); } 

   //----------------------------------------------------------------------------
   /*! \brief Return the internal Daim graphics object */
   //-----------------------------------------------------------------------------
   dmGraphics* GetGraphics() { return CCI_IGET(Graphics,Graphics    ,(dmGraphics*)NULL); }

   //----------------------------------------------------------------------------
   /*! \brief Ask the graphics object to render its content (i.e its attached document) 
    *
    *  This method will call the dmIDocument::Draw function of the attached document
    *  \param nativeDC   is a pointer to a native device context handle 
    *  \param boundRect  is a pointer to the display rectangle to be updated 
    *  \param paintFlags are passed directly to dmIDocument::Draw 
    */
   //-----------------------------------------------------------------------------
   bool Draw( void* nativeDC, const dm_rect* boundRect, dm_uint paintFlags ) {
     CCI_RETURN_IMETHOD_PP_OK(Graphics,Draw, _in nativeDC _in boundRect _in paintFlags )
   }

   bool GetPropertyValues( CONST char_t* propertyName, dm_cci(StringList) valueList ) {
     CCI_RETURN_IMETHOD_PP_OK(Graphics,GetPropertyValues, _in propertyName _in valueList )  
   }

   bool SetPropertyValue( CONST char_t* propertyName,CONST char_t* value ) {
     CCI_RETURN_IMETHOD_PP_OK(Graphics,PropertyValue, _in CCI_ME_SET _in propertyName _in value );
   }

   const char_t* GetPropertyValue( CONST char_t* propertyName ) {
     IF_CCI_IMETHOD_RP(Graphics,PropertyValue, _in CCI_ME_GET _in propertyName _out(value) NULL )
     THEN return CCI_RETVAL(value);
     ELSE return NULL;
     ENDIF
   }

   bool UpdateCursor() { CCI_RETURN_IMETHOD_NP_OK(Graphics,UpdateCursor) }

   //----------------------------------------------------------------------------
   /*! \brief Return the document object associated with the graphic  object.
    *
    *  The returned object should be compatible with 
    *  the dmIDocument interface.  This method AddRef() the returned object. 
    *  \sa SetDocument. 
    */
   //-----------------------------------------------------------------------------
   dm_cci(Document) GetDocument() {
     IF_CCI_IMETHOD_RP(Graphics,Document, _in CCI_ME_GET _out(document) NULL )
     THEN return CCI_RETVAL(document); ENDIF
     return NULL;
   }

   //----------------------------------------------------------------------------
   /*! \brief Set the document object associated with the graphic object
    *  Note that \a document must implement the dmIDocument interface
    */
   //-----------------------------------------------------------------------------
   bool SetDocument( dm_cci(Document) document ) {
     CCI_RETURN_IMETHOD_PP_OK(Graphics,Document, _in CCI_ME_SET _in document )
   }

   //----------------------------------------------------------------------------
   /*! \brief Remove the document associated with the graphic object */
   //-----------------------------------------------------------------------------
   bool ClearDocument() {
     CCI_RETURN_IMETHOD_PP_OK(Graphics,Document, _in CCI_ME_DISPOSE _in NULL )
   }

   //----------------------------------------------------------------------------
   /*! \brief Initialise the graphic object
    *
    *  \param[in] nativeWindow  pointer to a native window handle.
    */ 
   //-----------------------------------------------------------------------------
   bool InitGraphics( void* nativeWindow ) {
     CCI_RETURN_IMETHOD_PP_OK(Graphics,InitGraphics, _in nativeWindow _in NULL )
   }

   //----------------------------------------------------------------------------
   /*! \brief Process an event initiated from the native widgets.
    *
    *  \param structType the event struct type ( a dmTk::dmEventStructType value )
    *  \param eventType  the event type ( a dmTk::dmEventType value )
    *  \param button     the mouse button qualifier ( a dmTk::dmButtonType value )
    *  \param keyQual    the key qualifier ( a dmTk::dmKeyType value )
    *  \param keyCode    the key code
    *  \param cursorX    the position X of the graphical cursor
    *  \param cursorY    the position Y of the graphical cursor
    */ 
   //-----------------------------------------------------------------------------
   bool ProcessSystemEvent( dm_uint structType, dm_uint eventType,
                            dm_uint  button   , dm_uint keyQual, dm_uint keyCode,
                            dm_int  cursorX   , dm_int cursorY ) 
   {
     CCI_RETURN_IMETHOD_PP_OK(Graphics,ProcessSystemEvent,
                              _in structType _in eventType
                              _in button     _in keyQual _in keyCode
                              _in cursorX    _in cursorY)
   }

   //----------------------------------------------------------------------------
   /*! \brief Set the size of the display output area
    *
    *  This method pass informations to the graphic context handler about
    *  the (logical) size of the dimensions of the output area.
    *  These informations are used in conjonction with the \a PageScale in order 
    *  to properly compute cursor position and display scaling.
    */ 
   //-----------------------------------------------------------------------------
   bool SetDisplay( dm_int left, dm_int top, dm_uint  width  , dm_uint height ) { 
     CCI_RETURN_IMETHOD_PP_OK(Graphics,SetDisplay,_in left _in top _in width _in height ); 
   }

   bool SetDisplayEx( dm_int left, dm_int top, dm_uint&  width, dm_uint& height )
   { 
     IF_CCI_IMETHOD_RP(Graphics,SetDisplay,_in left _in top _in width _in height )
     THEN
       CCI_RETURN_P(width)
       CCI_RETURN_P(height)
     ENDIF_RETURN_BOOL()
   }
   
   bool SetCursorType( dm_param cursor  ) {
     CCI_RETURN_IMETHOD_PP_OK(Graphics,SetCursorType, _in cursor )
   }

   dm_param GetCursorType() {
      return CCI_IGET(Graphics,CursorType,(dm_uint)0);
   }

   //----------------------------------------------------------------------------
   /*! \brief Return the current screen-X position of the mouse cursor */
   //-----------------------------------------------------------------------------
   dm_int ScreenX() { return CCI_IGET(Graphics,ScreenX,(dm_int)0); }

   //----------------------------------------------------------------------------
   /*! \brief Return the current screen-Y position of the mouse cursor */
   //-----------------------------------------------------------------------------
   dm_int ScreenY() { return CCI_IGET(Graphics,ScreenY,(dm_int)0); }

   //----------------------------------------------------------------------------
   /*  \brief Render the document content into an image
    *
    *  This method will draw the content of document to the specified \a output 
    *  Setting /a left >= /a top (resp. /a right => /a bottom) will tell the method
    *  to use the default document values for the width (resp. the height) of the 
    *  document.
    */ 
   //-----------------------------------------------------------------------------
   bool PrintDocument( CONST char_t* output, dm_int left, dm_int top, dm_int right, dm_int bottom )
   {
     CCI_RETURN_IMETHOD_PP_OK(Graphics,PrintDocument, 
       _in output 
       _in left 
       _in top 
       _in right 
       _in bottom 
     )
   }
  
   bool PrintDocument( CONST char_t* output, CONST dm_rect& boundRect)
   {
     CCI_RETURN_IMETHOD_PP_OK(Graphics,PrintDocument, 
       _in  output
       _in  boundRect.top_left.x      
       _in  boundRect.top_left.y
       _in  boundRect.bottom_right.x  
       _in  boundRect.bottom_right.y 
     )
   }
   
   bool PrintDocument( CONST char_t* output ) {
     CCI_RETURN_IMETHOD_PP_OK(Graphics,PrintDocument, 
      _in output _in 0 _in 0 _in 0 _in 0 )
   }
   
   bool StopCapture( bool stop ) {
      return CCI_ISET(Graphics,StopCapture,(stop?dm_true:dm_false));
   }
   
   bool CanCapture() {
      return CCI_IGET(Graphics,StopCapture,(dm_uint)dm_false)!=dm_false;
   }
  
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Graphics_h */
