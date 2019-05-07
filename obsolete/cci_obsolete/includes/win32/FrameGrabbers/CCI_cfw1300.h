#ifndef CCI_Cfw1300_h
#define CCI_Cfw1300_h

/* ::: BEGIN LICENSE BLOCK:::
 *
 * Copyright (c) 2004-2005 Clara Vision
 *
 *  ::: END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : CCI_Cfw1300.h
// Date         : 9/2005
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_Cfw1300  _TXT("framegrabbers/cfw1300.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( Cfw1300, FrameGrabber )

  CCI_MODULE_UUID( Cfw1300, 310dab27-a617-4f6b-9fef-c47c1bf7bea8)

  CCI_MODULE_VERSION( Cfw1300, 1 )


CCI_END_MODULE_DECL( Cfw1300 )

#endif // __doxygen

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_Cfw1300_h */
