#ifndef cciScriptableImage_h
#define cciScriptableImage_h
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
 * Portions created by the Initial Developer are Copyright (C) 2011
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

/* $Id: cciScriptableImage.h 4 févr. 2011 23:26:42 */


#include "cciCOMPtr.h"
#include "cciImageContainerUtils.h"
#include "cciIImage.h"
#include "cciIColorTable.h"

/**
 * @startuml
 * 
 * interface cciIImage
 * interface cciIImageContainer
 * 
 * cciIImageContainer  <|.. cciImageWrapper
 * cciIImageContainer  <|-- cciIImage
 * cciImageWrapper     <|-- cciScriptableImage
 * cciIImage           <|.. cciScriptableImage
 *
 * @enduml
 */


/* Header file */
class cciScriptableImage :  public cciIImage,
                            public cciImageWrapper
                           
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IIMAGE

  CCI_FORWARD_IIMAGECONTAINER(cciImageWrapper::)
  
  cciScriptableImage();
  cciScriptableImage( dmLink<dmImage>& imglink );
  cciScriptableImage( dm_uint32 width, dm_uint32 height, EPixelFormat );
  cciScriptableImage( dmImageData& imData );

  dm_bool IsValid() const { return !mImage.IsNull(); }
  
protected:
  ~cciScriptableImage();
  
  cci_result Initialize( dm_uint32 width, dm_uint32 height, EPixelFormat );
  cci_result Initialize( dmImageData& imData );
 
protected:
  dm_bool mLock;
  dm_bool mEnableAlpha;

  cci_Ptr<cciIColorTable> mColorTable;
};




#endif /* cciScriptableImage_h */
