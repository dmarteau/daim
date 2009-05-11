
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

#define dmUseKernelTemplates
#include "daim_kernel.h"
#include "templates/processing/dmNagaoFilter.h"


namespace daim_nagao_impl {

__Nagao_A _A = {
  { 2,0 }, // right top 0
  { 1,0 }, //       top
  { 0,0 }, // left, top 2
  { 0,1 }, // left
  { 0,2 }, // left, bottom 4
  { 1,2 }, //       bottom
  { 2,2 }, // right,bottom 6
  { 2,1 }, // right,
};

__Nagao_K _K[] = {
{ 0,1,1, 1,1,1, 1,1,0 },
{ 0,1,0, 1,1,1, 1,1,1 },
{ 1,1,0, 1,1,1, 0,1,1 },
{ 0,1,1, 1,1,1, 0,1,1 },
{ 0,1,1, 1,1,1, 1,1,0 },
{ 1,1,1, 1,1,1, 0,1,0 },
{ 1,1,0, 1,1,1, 0,1,1 },
{ 1,1,0, 1,1,1, 1,1,0 },
};

} // daim_nagao_impl
