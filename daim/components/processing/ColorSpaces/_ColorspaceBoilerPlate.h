#ifndef _ColorSpaceBoilerPlate_h
#define _ColorSpaceBoilerPlate_h
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

#define COLORSPACE_EXTRACTOR( n ) \
     COLORSPACE_NAMESPACE::channels::get<n,integer_type>()

#define COLORSPACE_EXTRACT( n ) \
  case n: \
     daim::transform(rgn,dmPoint(0,0),_src->Gen(),_dst->Gen(),COLORSPACE_EXTRACTOR(n));\
     return CCI_OK;


#define COLORSPACE_CHANNEL( n ) *ColorSpace_GetChannel(n-1,mImageList)

#define DEFINE_COLORSPACE_CLASSNAME_(name) #name " Color Space"
#define DEFINE_COLORSPACE_CID_(name) CCI_COLORSPACE_##name##_CID
#define DEFINE_COLORSPACE_CONTRACTID_(name) CCI_COLORSPACE_##name##_CONTRACTID
#define DEFINE_COLORSPACE_CONSTRUCTOR_(_InstanceName) _InstanceName##Constructor
#define DEFINE_COLORSPACE_MODULE_(name) name##ColorSpaceModule

#define DECLARE_COLORSPACE_CLASSNAME(name) DEFINE_COLORSPACE_CLASSNAME_(name)
#define DECLARE_COLORSPACE_CID(name) DEFINE_COLORSPACE_CID_(name)
#define DECLARE_COLORSPACE_CONTRACTID(name) DEFINE_COLORSPACE_CONTRACTID_(name)
#define DECLARE_COLORSPACE_CONSTRUCTOR(_InstanceName) DEFINE_COLORSPACE_CONSTRUCTOR_(_InstanceName)
#define DECLARE_COLORSPACE_MODULE(name) DEFINE_COLORSPACE_MODULE_(name)

#endif // _ColorSpaceBoilerPlate_h
