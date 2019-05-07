#ifndef CCI_Draw2d_h
#define CCI_Draw2d_h

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
// File         : CCI_Draw2d.h
// Date         : 11/2006
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_Draw2d  _TXT("processing/draw2d.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Draw2d, Notify )

  CCI_MODULE_UUID( Draw2d, e63750dc-55f8-4c79-9d74-f06d7370da79)

  CCI_MODULE_VERSION( Draw2d, 2 ) 

  /* Methods */
  CCI_DECL_METHOD_PP(Draw2d, 1,  Initialize, 
   _in dm_cci(Parameters) params
   _in dm_uint pixelformat )

  CCI_DECL_METHOD_PP( Draw2d, 2, SetColor, 
    _in dm_real   c1 
    _in dm_real   c2 
    _in dm_real   c3 
  )

  CCI_DECL_METHOD_PP( Draw2d, 3, DrawMask, 
    _in dm_cci(Parameters) params
  )

  CCI_DECL_METHOD_PP( Draw2d, 4, DrawLine, 
    _in dm_int x0
    _in dm_int y0
    _in dm_int x1
    _in dm_int y1
  )

  CCI_DECL_METHOD_PP( Draw2d, 5, DrawRectangle, 
    _in dm_int x0
    _in dm_int y0
    _in dm_int x1
    _in dm_int y1
  )
 
  CCI_DECL_METHOD_PP( Draw2d, 6, DrawEllipse, 
    _in dm_int  x0
    _in dm_int  y0
    _in dm_real r1
    _in dm_real r2
    _in dm_real ru
    _in dm_real rv
  )

  CCI_DECL_METHOD_PP( Draw2d, 7, DrawCircle, 
    _in dm_int  x0
    _in dm_int  y0
    _in dm_real r0
  )

  CCI_DECL_METHOD_PP( Draw2d, 8, Opacity, 
    _in dm_real  opacity 
    _in dm_param me
  )

  CCI_DECL_METHOD_PP( Draw2d, 9, DrawPoly, 
    _in dm_array(dm_point,count) points 
    _in dm_uint  count
  )

  CCI_DECL_METHOD_PP( Draw2d, 10, InitPath,  
    _in dm_uint  flags
  )

  CCI_DECL_METHOD_NP( Draw2d, 11, ClosePath )

  /* Attributs */

  CCI_DECL_ATTR(Draw2d, 1,  PenSize,  dm_uint  , [ISGN] )
  CCI_DECL_ATTR(Draw2d, 2,  Pattern,  dm_uint  , [ISGN] )
  CCI_DECL_ATTR(Draw2d, 3,  DrawMode, dm_uint  , [ISGN] )
  CCI_DECL_ATTR(Draw2d, 4,  Width,    dm_uint  , [..G.] )
  CCI_DECL_ATTR(Draw2d, 5,  Height,   dm_uint  , [..G.] )
  CCI_DECL_ATTR(Draw2d, 6,  Format,   dm_uint  , [..G.] )

  /* Special values */
  
  #define CCIV_Draw2d_DRAW_OUTLINE 0x1L

CCI_END_MODULE_DECL( Draw2d )

#endif // __doxygen

#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Daim Draw2d interface
 *
 *  2d drawing helper
 */
//------------------------------------------------------------------------------------
class dmIDraw2d : public dmINotify
{    
  public:
   dmDECLARE_GENERIC_IINTERFACE(Draw2d)

   //-------------------------------------------------------
   /*! \brief Initialize destination image and ROI as clipping area */
   //-------------------------------------------------------
   bool Initialize( dm_cci(Parameters) params, dm_uint pixelformat ) { 
      CCI_RETURN_IMETHOD_PP_OK(Draw2d,Initialize, _in params _in pixelformat ) 
   }

   //-------------------------------------------------------
   /*! \brief Set color triplet 
    *
    * If the destination image is scalar, the value will
    * be the mean of c1, c2 and c3. 
    */
   //-------------------------------------------------------
   bool SetColor( dm_real c1,  dm_real c2, dm_real c3 ) {
      CCI_RETURN_IMETHOD_PP_OK(Draw2d,SetColor, _in c1 _in c2 _in c3 ) 
   }

   //-------------------------------------------------------
   /*! \brief Draw the current mask */
   //-------------------------------------------------------
   bool DrawMask( dm_cci(Parameters) params ) { 
     CCI_RETURN_IMETHOD_PP_OK(Draw2d, DrawMask, _in params ) 
   }

   //-------------------------------------------------------
   /*! \brief Draw Line  */
   //-------------------------------------------------------
   bool DrawLine( dm_int x0, dm_int y0, dm_int x1, dm_int y1 ) { 
     CCI_RETURN_IMETHOD_PP_OK(Draw2d, DrawLine, 
        _in x0 _in y0 _in x1 _in y1 )
   }
   
   //-------------------------------------------------------
   /*! \brief Draw Rectangle  */
   //-------------------------------------------------------
   bool DrawRectangle( dm_int x0, dm_int y0, dm_int x1, dm_int y1 ) { 
     CCI_RETURN_IMETHOD_PP_OK(Draw2d, DrawRectangle, 
        _in x0 _in y0 _in x1 _in y1 )
   }

   //-------------------------------------------------------
   /*! \brief Draw Ellipse  */
   //-------------------------------------------------------
   bool DrawEllipse( dm_int   x0, dm_int   y0,
                     dm_float r1, dm_float r2,
                     dm_float ru, dm_float rv )
   { 
     CCI_RETURN_IMETHOD_PP_OK(Draw2d, DrawEllipse, 
          _in x0 _in y0
          _in r1 _in r2
          _in ru _in rv )
   }

   //-------------------------------------------------------
   /*! \brief Draw Circle  */
   //-------------------------------------------------------
   bool DrawCircle( dm_int x0, dm_int y0, dm_real r0 )
   {
      CCI_RETURN_IMETHOD_PP_OK( Draw2d, DrawCircle, 
                             _in x0 _in y0 _in r0 )
   }

   //-------------------------------------------------------
   /*! \brief Set Pen size  */
   //-------------------------------------------------------
   bool SetPenSize( dm_uint size ) { 
      return CCI_ISET(Draw2d,PenSize,size); 
   }

   //-------------------------------------------------------
   /*! \brief Get Pen size  */
   //-------------------------------------------------------
   dm_uint GetPenSize()  { return CCI_IGET(Draw2d,PenSize,(dm_uint)0); } 
   
   //-------------------------------------------------------
   /*! \brief Set Pen opacity  */
   //-------------------------------------------------------
   bool SetOpacity( dm_real opacity ) {
     CCI_RETURN_IMETHOD_PP_OK(Draw2d,Opacity,_in opacity _in CCI_ME_SET );
   }

   //-------------------------------------------------------
   /*! \brief Get Pen opacity  */
   //-------------------------------------------------------
   dm_real GetOpacity()  { 
      IF_CCI_IMETHOD_RP(Draw2d,Opacity, _out(opacity) 0 _in CCI_ME_GET ) 
        THEN return CCI_RETVAL(opacity);
        ELSE return 0;
      ENDIF
   }
   //-------------------------------------------------------
   /*! \brief Set Pen Pattern  */
   //-------------------------------------------------------
   bool SetPattern( dm_uint pattern ) { return CCI_ISET(Draw2d,Pattern,pattern); }

   //-------------------------------------------------------
   /*! \brief Get Pen Pattern  */
   //-------------------------------------------------------
   dm_uint GetPattern()  { return CCI_IGET(Draw2d,Pattern,(dm_uint)0); } 

   //-------------------------------------------------------
   /*! \brief Set DrawMode  */
   //-------------------------------------------------------
   bool SetDrawMode( dm_uint drMode ) { return CCI_ISET(Draw2d,DrawMode,drMode); }

   //-------------------------------------------------------
   /*! \brief Get Draw Mode  */
   //-------------------------------------------------------
   dm_uint GetDrawMode()  { return CCI_IGET(Draw2d,DrawMode,(dm_uint)0); } 

   //-------------------------------------------------------
   /*! \brief Draw Poly  */
   //-------------------------------------------------------
   bool DrawPoly( dm_array(dm_point,count) points, dm_uint count ) {
     CCI_RETURN_IMETHOD_PP_OK( Draw2d, DrawPoly, _in points _in count )
   }
   
   //-------------------------------------------------------
   /*! \brief Get Raster width  */
   //-------------------------------------------------------
   dm_uint GetWidth()  { return CCI_IGET(Draw2d,Width,(dm_uint)0); } 

   //-------------------------------------------------------
   /*! \brief Get Raster height  */
   //-------------------------------------------------------
   dm_uint GetHeight() { return CCI_IGET(Draw2d,Height,(dm_uint)0); } 

   //-------------------------------------------------------
   /*! \brief Get Raster height  */
   //-------------------------------------------------------
   EPixelFormat GetFormat() { 
      return CCI_IGET(Draw2d,Format,(EPixelFormat)0); 
   } 

   //-------------------------------------------------------
   /*! \brief Start path accumulator  */
   //-------------------------------------------------------
   bool InitPath( dm_uint flags ) {
     CCI_RETURN_IMETHOD_PP_OK( Draw2d, InitPath, _in flags )
   }

   //-------------------------------------------------------
   /*! \brief Close path accumulator and draw region */
   //-------------------------------------------------------
   bool ClosePath() {
     CCI_RETURN_IMETHOD_NP_OK( Draw2d, ClosePath )
   }

};

#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

//-------------------------------------------------------------
#endif /* CCI_Document_h */
