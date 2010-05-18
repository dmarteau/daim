#ifndef grszdefs_h
#define grszdefs_h
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code 3liz code.
 *
 * The Initial Developer of the Original Code is 3Liz SARL.
 * Portions created by the Initial Developer are Copyright (C) 2010
 * 3Liz SARL. All Rights Reserved.
 *
 * Contributor(s):
 *   David Marteau  (original author)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

/* $Id: grszdefs.h 17 mai 2010 15:18:45 */

/////////////////////////////////////////////////////////////////////////////
typedef enum { eGRSZCircular=1, eGRSZLinear=2 } EGRSZProfilType;
//---------------------------------------------------------

// Structure for results on a profile's measure
struct GRSZIntercepts 
{
   float    fSegments;          // Numbers of intercepted segments
   float    fProfilThreshold;   // Threshold value
   float    fComputedThreshold; // Threshold value (computed) 
   float    fMinGrainSize;      // Minimum size of grains
   int      nMaxBorderLength;   // Maximum border size of object border to be rejected
   bool     bRelThreshold;      // Threshold is relative
   bool     bInvert;
   EGRSZProfilType eProfilType; // Profile's type 
};

#endif /* grszdefs_h */
