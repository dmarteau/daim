#ifndef cciImageListContainer_h
#define cciImageListContainer_h
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

#include "ccidaim.h"

/* starting interface:    cciIComponentManager */
#define CCI_IIMGLISTCONTAINER_IID_STR "D4657836-C375-4828-90A1-0F6EC6D240DD"

#define CCI_IIMGLISTCONTAINER_IID \
  {0xD4657836, 0xC375, 0x4828, \
    { 0x90, 0xA1, 0x0F, 0x6E, 0xC6, 0xD2, 0x40, 0xDD }}

class CCI_NO_VTABLE cciIImageListContainer : public cciISupports
{
  public:
    CCI_DECLARE_STATIC_IID_ACCESSOR(CCI_IIMGLISTCONTAINER_IID)

    CCI_IMETHOD GetImageLink( dmLink<dmImage>& imglink, dm_uint32 index = 0 ) = 0;
    CCI_IMETHOD SetImageLink( dmLink<dmImage>& imglink, dm_uint32 index = 0 ) = 0;
};

CCI_DEFINE_STATIC_IID_ACCESSOR(cciIImageListContainer, CCI_IIMGLISTCONTAINER_IID)

#endif // cciImageListContainer_h

