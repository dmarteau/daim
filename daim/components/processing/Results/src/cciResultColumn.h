#ifndef cciResultColumn_h
#define cciResultColumn_h
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

/* $Id: cciResultColumn.h 2 févr. 2010 22:35:47 */

#define dmUseUtilitiesExtra
#include "cciCOMPtr.h"
#include "cciIResultColumn.h"
#include "daim_utilities.h"

typedef dmVector<dm_real> dmStoreVector;

 /* 67ac9bde-efd8-497f-ba6e-8c4e981d7b5b */
#define CCI_RESULTCOLUMN_PRIVATE_IID \
    { 0x67ac9bde, 0xefd8, 0x497f, { 0xba, 0x6e, 0x8c, 0x4e, 0x98, 0x1d, 0x7b, 0x5b } }
class cciResultColumn : public cciIResultColumn
{
public:
  CCI_DECLARE_STATIC_IID_ACCESSOR(CCI_RESULTCOLUMN_PRIVATE_IID)

  CCI_DECL_ISUPPORTS
  CCI_DECL_IRESULTCOLUMN

  cciResultColumn();

  typedef dmStoreVector::iterator iterator;

  iterator Begin() { return mData.Begin(); }
  iterator End()   { return mData.End();   }
  
  dm_real*  Data()  { return mData.Begin(); }
  
  dm_uint32 Size() const { return mData.Size(); }

protected:
  dmStoreVector   mData;  
};



#endif /* cciResultColumn_h */
