/* :::BEGIN LICENSE BLOCK:::
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
 *  :::END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : test_registry.cpp
// Date         : 28 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "test_common.h"

#include "cciCOM.h"
#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"
#include "cciIComponentRegistrar.h"
#include "cciIKernelService.h"
#include "cciICategoryManager.h"
#include "cciString.h"

#include "ccidaim.h"

#include <stdlib.h>


CCI_USE_NS

class test_message
{
  public:
    test_message()
      {
        printf("BEGIN unit tests for registry, compiled " __DATE__ "\n");
      }

   ~test_message()
      {
        printf("END unit tests for registry.\n");
      }
};

test_message gTestMessage;

int main( int argc, char ** argv )
{
  DM_BEGIN_TEST_BLOCK

    printf(">>main()\n");

    {
      printf("\n### Can I get the component registrar ?\n");
      cci_Ptr<cciIComponentRegistrar> compReg;
      cci_result rv = CCI_GetComponentRegistrar(getter_AddRefs(compReg));

      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      printf("\n### Can I get the  the kernel service (which whould be registered) ?\n");
      cci_Ptr<cciIKernelService> kernelSrvc = do_GetService("@daim.org/kernel-service;1",&rv);

      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

    }

    {
      printf("### Test Do I have a category manager ?\n");
      cci_Ptr<cciICategoryManager> catmngr = do_GetService("@daim.org/category-manager;1",&rv);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      printf("### Test 1.2: Can I enumerate categories ?\n");
      cci_Ptr<cciIStringEnumerator> categories;

      rv = catmngr->EnumerateCategories(getter_AddRefs(categories));
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      dmCString contractid;
      while(categories->HasMore())
      {
        rv = categories->GetNext(contractid);
        DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

        printf("==> Found Category: %s\n",contractid.get());
      }

    }

  DM_END_TEST_BLOCK

  DM_EXIT_TEST
}
