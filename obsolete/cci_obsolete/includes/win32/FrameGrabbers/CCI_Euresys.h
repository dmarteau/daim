#ifndef CCI_Euresys_h
#define CCI_Euresys_h

/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2005 Clara Vision
 *
 *  ::: END LICENSE BLOCK::: */
 
//--------------------------------------------------------
// File         : CCI_Euresys.h
// Date         : 9/2005
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_Euresys  _TXT("framegrabbers/euresys.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Euresys, FrameGrabber )

  CCI_MODULE_UUID( Euresys, 36d39bda-2354-4193-9f2d-a9c317affaf7)

  CCI_MODULE_VERSION( Euresys, 1 )


CCI_END_MODULE_DECL( Euresys )

#endif // __doxygen

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Euresys_h */
