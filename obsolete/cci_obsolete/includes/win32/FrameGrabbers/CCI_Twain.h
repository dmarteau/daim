#ifndef CCI_Twain_h
#define CCI_Twain_h

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
// File         : CCI_Twain.h
// Date         : 9/2005
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_Twain  _TXT("framegrabbers/twain.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Twain, FrameGrabber )

  CCI_MODULE_UUID( Twain, b1ff08c5-0244-455a-adbf-7ab861666e5f)

  CCI_MODULE_VERSION( Twain, 1 )

CCI_END_MODULE_DECL( Twain )

#endif // __doxygen

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Twain_h */
