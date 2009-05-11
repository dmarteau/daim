#ifndef _daim_tools_h
#define _daim_tools_h

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

#error "Er... No more tools in Daim2 !"

#ifdef dmUseToolPrefs
#define dmUseServicesMngr
#define dmUseExtendedTools
#endif // dmUseToolPrefs

#include "daim_kernel.h"

#define dmEventTarget dmTool

#ifdef dmUseExtendedTools

#ifndef dmUseAreaEdit
#define dmUseAreaEdit
#endif

#endif // dmUseExtendedTools

#include "dmToolsBase.h"

#include "daim_tools/dmToolKit.h"
#include "daim_tools/dmEventTypes.h"

using namespace dmTk;

#include "dmEvents.h"
#include "dmEventNotifier.h"
#include "dmEventHandler.h"
#include "dmGraphics.h"

//------------------ 
// Shapes & Tools
//------------------ 
#include "dmCarrets.h"
#include "dmTools.h"
#include "dmToolManips.h"
#include "dmToolGroup.h"
#include "dmToolVisitor.h"
#include "dmShapeVisitor.h"

#ifdef dmUseAreaEdit
#include "dmAreaEdit.h"
#endif // dmUserAreaEdit

#ifdef dmUseExtendedTools
#include "dmArrows.h"
#include "dmLineExt.h"
#include "dmImageFrame.h"
#include "dmText.h"
#endif // dmUseExtendedTools

#ifdef dmUseToolManager
#include "dmToolGroup.h"
#include "dmToolManager.h"
#endif // dmUseToolManager

#ifdef dmUseToolPrefs
#include "dmToolPrefs.h"
#include "dmToolTextPrefs.h"
#include "dmToolImagePrefs.h"
#include "dmToolArrowPrefs.h"
#include "dmToolPrefsVisitor.h"
#include "dmStyleManager.h"
#endif // dmUseToolPrefs

#ifdef dmUseToolFactory
#include "dmToolFactory.h"
#include "dmToolClasses.h"
#endif // dmUseToolFactory


#endif // _daim_tools_h
