#ifndef dmImagesErrors_h
#define dmImagesErrors_h

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
// File         : dmImagesErrors.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

//--------------------------------------------------
#define DM_EIMAGE_ERROR          dmBASE_ERROR(-100)
#define DM_ECOORD_OUT_OF_RANGE   (DM_EIMAGE_ERROR - 1)
#define DM_EIMAGE_NO_INITIALIZED (DM_EIMAGE_ERROR - 2)
#define DM_EINVALID_BASE_POINTER (DM_EIMAGE_ERROR - 3)
#define DM_EINVALID_IMAGE_SIZE   (DM_EIMAGE_ERROR - 4)
#define DM_ECORRUPT_SHARED_IMAGE (DM_EIMAGE_ERROR - 5)
#define DM_ECONVERSION_ERROR     (DM_EIMAGE_ERROR - 6)
#define DM_EIO_ERROR             (DM_EIMAGE_ERROR - 7)
#define DM_EDISPLAY_ERROR        (DM_EIMAGE_ERROR - 8)
#define DM_EUNKNOWN_IMAGE_FORMAT (DM_EIMAGE_ERROR - 9)
//--------------------------------------------------
#define dmImageError( msg, code ) dmCustomError( msg, code )

#define EImageError( code )     dmImageError(  #code ,code )

#define ECoordOutOfRange()     EImageError( DM_ECOORD_OUT_OF_RANGE   )
#define EImageNotInitialized() EImageError( DM_EIMAGE_NO_INITIALIZED )
#define EInvalidBasePointer()  EImageError( DM_EINVALID_BASE_POINTER )
#define EInvalidImageSize()    EImageError( DM_EINVALID_IMAGE_SIZE   )
#define ECorruptSharedImage()  EImageError( DM_ECORRUPT_SHARED_IMAGE )
#define EConversionError()     EImageError( DM_ECONVERSION_ERROR     )
#define EIOError()             EImageError( DM_EIO_ERROR             )
#define EDisplayError()        EImageError( DM_EDISPLAY_ERROR        )
#define EUnknownImageFormat()  EImageError( DM_EUNKNOWN_IMAGE_FORMAT )

//--------------------------------------------------

#endif // dmImagesErrors_h
