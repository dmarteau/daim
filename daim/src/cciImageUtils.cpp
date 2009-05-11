/*
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
#
# The Original Code is Daim code.
#
# The Initial Developer of the Original Code is
#   David Marteau
# Portions created by the Initial Developer are Copyright (C) 2008
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#   David Marteau (dhmarteau@gmail.com)
#
# Alternatively, the contents of this file may be used under the terms of
# either of the GNU General Public License Version 2 or later (the "GPL"),
# or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
# in which case the provisions of the GPL or the LGPL are applicable instead
# of those above. If you wish to allow use of your version of this file only
# under the terms of either the GPL or the LGPL, and not to allow others to
# use your version of this file under the terms of the MPL, indicate your
# decision by deleting the provisions above and replace them with the notice
# and other provisions required by the GPL or the LGPL. If you do not delete
# the provisions above, a recipient may use your version of this file under
# the terms of any one of the MPL, the GPL or the LGPL.
#
# ***** END LICENSE BLOCK *****
*/

#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciImageUtils.h"

#include <string.h> // for strncat

#ifdef DAIM_GLUE
CCI_USE_NS

CCI_COM_GLUE
cci_result CCI_NewImageList( cciIImageList** _result )
{
  cci_result rv;
  cci_Ptr<cciIImageList> imagelist = do_CreateInstance("@daim.org/image/list;1",&rv);
  if(CCI_SUCCEEDED(rv))
    imagelist.forget(_result);
    //CCI_ADDREF(*_result = buffer);

  return rv;
}


#else // !DAIM_GLUE

#include "cciImageList.h"

CCI_COM_GLUE
cci_result CCI_NewImageList( cciIImageList** _result )
{
  cciIImageList* newList = new cciImageList();
  CCI_ADDREF(*_result = newList);
  return CCI_OK;
}


#endif // DAIM_GLUE

#define COLORSPACE_MAX_CHAR 10

CCI_COM_GLUE
cci_result CCI_NewColorSpace( const char* colorSpace, cciIColorSpace** _result )
{
  char
  contractid[sizeof(CCI_COLORSPACE_CONTRACTID_)+COLORSPACE_MAX_CHAR+1] = CCI_COLORSPACE_CONTRACTID_;

  strncat(contractid,colorSpace,COLORSPACE_MAX_CHAR);

  cci_result rv;
  cci_Ptr<cciIColorSpace> buffer = do_CreateInstance(contractid,&rv);
  if(CCI_SUCCEEDED(rv))
     CCI_ADDREF(*_result = buffer);

  return rv;
}

