#ifndef cciFilterContext_h
#define cciFilterContext_h
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

#include "daim_kernel.h"
#include "cciCOMPtr.h"
#include "cciString.h"
#include "cciIFilterContext.h"
#include "cciIInterfaceRequestor.h"
#include "cciIColorSpace.h"

/* Header file */
class cciFilterContext final : public cciIFilterContext,
                               public cciIInterfaceRequestor
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IFILTERCONTEXT
  CCI_DECL_IINTERFACEREQUESTOR

  cciFilterContext();

private:
 ~cciFilterContext();

protected:
  dm_real                  mMinRange;
  dm_real                  mMaxRange;
  dmLink<dmImageBuffer>    mBuffer;

};

#endif // cciFilterContext_h
