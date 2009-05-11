#ifndef CCI_uEye_h
#define CCI_uEye_h

/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2005 Clara Vision
 *
 *  ::: END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : CCI_uEye.h
// Date         : 3/2006
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_uEye  _TXT("framegrabbers/uEye.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( uEye, FrameGrabber )

  CCI_MODULE_UUID( uEye, acab1394-9f51-4c2a-a980-e0b66ddab123)

  CCI_MODULE_VERSION( uEye, 1 )


CCI_END_MODULE_DECL( uEye )

#endif // __doxygen

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_uEye */
