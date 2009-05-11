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
// File         : test_comptr.cpp
// Date         : 28 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "test_common.h"

#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"
#include "cciIComponentRegistrar.h"
#include "cciIKernelService.h"
#include "cciIDriverProxy.h"

#include "ccidaim.h"

CCI_USE_NS

class test_message
{
  public:
    test_message()
      {
        printf("BEGIN unit tests for kernel manager, compiled " __DATE__ "\n");
      }

   ~test_message()
      {
        printf("END unit tests for kernel manager.\n");
      }
};

test_message gTestMessage;

const char gKernelModuleLocation[] = "daim/kernelmngr.cci";

int main( int argc, char ** argv )
{
  DM_BEGIN_TEST_BLOCK

    printf(">>main()\n");

    {
      printf("\n### Test  1: Can I get the component registrar ?\n");
      cci_Ptr<cciIComponentRegistrar> regMngr;
      cci_result rv = CCI_GetComponentRegistrar(getter_AddRefs(regMngr));

      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      printf("\n### Test  2: Can I register the kernel module ?\n");
      rv = regMngr->AutoRegister(gKernelModuleLocation);

      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      printf("\n### Test  3: Can I get the  the kernel service ?\n");
      cci_Ptr<cciIKernelService> kernelSrvc = do_GetService("@daim.org/kernel-service;1",&rv);

      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

    }

  DM_END_TEST_BLOCK

  DM_EXIT_TEST
}
