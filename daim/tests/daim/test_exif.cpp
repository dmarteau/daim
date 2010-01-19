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
// File         : test_loader.cpp
// Date         : 28 nov. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "test_common.h"

#include "cciCOM.h"
#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"
#include "cciIComponentRegistrar.h"
#include "cciILoaderService.h"
#include "cciIRemoteSurface.h"
#include "cciIStringEnumerator.h"
#include "cciIMetaDataContainer.h"
#include "cciString.h"

#include <stdlib.h>


CCI_USE_NS

class test_message
{
  public:
    test_message()
      {
        printf("BEGIN unit tests for loaders, compiled " __DATE__ "\n");
      }

   ~test_message()
      {
        printf("END unit tests for loaders.\n");
      }
};

test_message gTestMessage;

const char  sampleLocation[] = "../samples/exif/camera.jpg";

int main( int argc, char ** argv )
{
  DM_BEGIN_TEST_BLOCK

    printf(">>main()\n");

    {
      printf("\n### Can I get the  the loader service (which whould be registered) ?\n");
      cci_Ptr<cciILoaderService> ldrSrvc = do_GetService("@daim.org/contrib/loader-service;1",&rv);

      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      cci_Ptr<cciISurface> surface;

      printf("\n### Can I load %s ?\n",sampleLocation);

      rv = ldrSrvc->OpenSurface(sampleLocation,cciIDriverProxy::IO_READONLY,getter_AddRefs(surface));
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      printf("\n### width = %d, height = %d, bands = %d\n",
             surface->Width(),surface->Height(),surface->Bands());

      cci_Ptr<cciIMetaDataContainer> mdcontainer = do_QueryInterface(surface,&rv);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      dmCString data;
      cci_Ptr<cciIUTF8StringEnumerator> enumerator;

      printf("\n### Can I Get default Metadata ?\n");
      rv = mdcontainer->GetMetadata("",getter_AddRefs(enumerator));
      if(rv == CCI_ERROR_NOT_AVAILABLE)
        printf("==> No metadata\n");
      else
      {
        while(enumerator->HasMore())
        {
          rv = enumerator->GetNext(data);
          if(CCI_SUCCEEDED(rv))
             printf(" item : %s\n",data.get());
        }
      }

      printf("\n### Can I Get IMAGE_STRUCTURE Metadata ?\n");
      rv = mdcontainer->GetMetadata("IMAGE_STRUCTURE",getter_AddRefs(enumerator));
      if(rv == CCI_ERROR_NOT_AVAILABLE)
        printf("==> No metadata\n");
      else
      {
        while(enumerator->HasMore())
        {
          rv = enumerator->GetNext(data);
          if(CCI_SUCCEEDED(rv))
             printf(" item : %s\n",data.get());
        }
      }

    }

  DM_END_TEST_BLOCK

  DM_EXIT_TEST
}
