
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
// File         : dmCCIModule.h
// Date         : 7/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------
#include "dmCCIHook.h"
//-----------------------------------------------
#ifndef CCI_VERSION
#define CCI_VERSION  0
#endif
#ifndef CCI_REVISION
#define CCI_REVISION 0
#endif
#ifndef CCI_IDSTRING
#define CCI_IDSTRING _TXT("CCI User Class Extension")
#endif

//-----------------------------------------------
CCI_METHOD CCI_Init(struct CCI_Custom_Hook*);
void       CCI_Close();
cci_Class *CCI_main();

void **_CCI_LibVector = NULL;


#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------
struct CCI_Custom_Tag
__DLL_EXPORT_TAG _CCI_Tag = {
    CCI_MAKE_ID('C','C','I','C'),  /* ccih_Magic       */
    CCI_SYSTEM_VERSION,            /* ccih_CCI_VER     */
    CCI_VERSION,                   /* ccih_Version     */
    CCI_REVISION,                  /* ccih_Revision    */
    CCI_IDSTRING,                  /* ccih_Message     */
    (void*)CCI_Init,               /* ccih_Init        */
    (void*)CCI_Close,              /* ccih_Close       */
    (void*)CCI_main,               /* ccih_main        */
    &_CCI_LibVector,               /* ccih_LibVector   */
};

//---------------------------------------------------------
#ifdef __cplusplus
}
#endif


#ifndef CCI_CUSTOM_INIT_HANDLER

CCI_METHOD CCI_Init(struct CCI_Custom_Hook* cci_Hook)
{
  return CCI_INIT_API();
}
//
void CCI_Close() { CCI_CLOSE_API(); }

#endif // CCI_CUSTOM_INIT_HANDLER
