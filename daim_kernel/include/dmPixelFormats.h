#ifndef dmPixelFormat_h
#define dmPixelFormat_h

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
// File         : dmPixelFormat.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

// The following enumerate daim storage pixel types
// Implementors of loaders should convert data into these types

/*! \addtogroup daim_fmt Daim pixel formats
 *  @{
 */

//-----------------------------------------------------
/*! Pixel formats/
 *
 * Note that \c dmPixelFormat32bppGrayScale is mainly used for
 * input/output and is not supported by most of the image processing
 * operations.
 */
//-----------------------------------------------------
enum EPixelFormat
{
  dmPixelFormatIndexed     = 0x00010000, // Indexes into a palette
  dmPixelFormatDisplayable = 0x00020000, // Is a displayable format ?
  dmPixelFormatScalar      = 0x00040000, // Is a scalar format ?
  dmPixelFormatSigned      = 0x00080000, // Is a signed format ?

  dmPixelFormatUndefined       =  0,
  dmPixelFormat8bppIndexed     =  1 | ( 8 << 8) | dmPixelFormatIndexed       /*!< 8 bits indexed pixel format         */
                                                | dmPixelFormatDisplayable
                                                | dmPixelFormatScalar,

  dmPixelFormat24bppRGB        =  2 | (24 << 8) | dmPixelFormatDisplayable,  /*!< 24 bits RGB pixel format            */
  dmPixelFormat16bppGrayScale  =  3 | (16 << 8) | dmPixelFormatScalar,       /*!< 16 bits gray scale pixel format     */
  dmPixelFormat32bppGrayScale  =  4 | (32 << 8) | dmPixelFormatScalar,       /*!< 32 bits gray scale pixel format     */
  dmPixelFormat32bppFloat      =  5 | (32 << 8) | dmPixelFormatScalar        /*!< 32 bits floating point pixel format */
                                                | dmPixelFormatSigned,
  dmPixelFormat32bppARGB       =  6 | (32 << 8),
  dmPixelFormatMax             =  7
};

/*@}*/

inline int dmGetPixelFormatIndex      ( EPixelFormat _F ) { return (_F & 0x0F);        }
inline int dmGetPixelFormatBits       ( EPixelFormat _F ) { return (_F & 0xFF00) >> 8; }
inline int dmIsPixelFormatIndexed     ( EPixelFormat _F ) { return (_F & dmPixelFormatIndexed)    ?1:0; }
inline int dmIsPixelFormatDisplayable ( EPixelFormat _F ) { return (_F & dmPixelFormatDisplayable)?1:0; }
inline int dmIsPixelFormatScalar      ( EPixelFormat _F ) { return (_F & dmPixelFormatScalar)     ?1:0; }
inline int dmIsPixelFormatSigned      ( EPixelFormat _F ) { return (_F & dmPixelFormatSigned)     ?1:0; }

#endif // dmImageFormat_h
