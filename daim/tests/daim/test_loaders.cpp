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

// The registry should have been created for this test to succeed

#include "test_common.h"

#include "cciCOM.h"
#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"
#include "cciIComponentRegistrar.h"
#include "cciILoaderService.h"
#include "cciIRemoteSurface.h"
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


char  sampleLocation[MAX_PATH] = "\0";
const char* samples[] = {
  "tests/Billes.jpg",
  "tests/Billes.png",
  "tests/Billes16bits.png",
  "grains/grains.bmp",
  "big/fields.jpeg",
  NULL
};

int main( int argc, char ** argv )
{
  DM_BEGIN_TEST_BLOCK

    printf(">>main()\n");

    {
      // Get the DAIM_BIN environment variable
      const char* location = getenv("DAIM_BIN");

      DM_TEST_EXPECT_RESULT(location && location[0]!='\0');

      printf("\n### DAIM_BIN set to %s\n",location);

      snprintf(sampleLocation,MAX_PATH-1,"%s/samples/",location);

      printf("\n### Can I get the  the loader service (which whould be registered) ?\n");
      cci_Ptr<cciILoaderService> ldrSrvc = do_GetService("@daim.org/contrib/loader-service;1",&rv);

      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      cci_Ptr<cciISurface> surface;
      dmCString path;

      printf("\n### Loading images...\n");
      const char** src = samples;
      for(;*src;++src)
      {
        path = sampleLocation;
        path.Append(*src);
        printf("\n### Can I load %s ?\n",path.get());

        rv = ldrSrvc->OpenSurface(path.get(),cciIDriverProxy::IO_READONLY,getter_AddRefs(surface));
        DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

        printf("\n### width = %d, height = %d, bands = %d\n",
               surface->Width(),surface->Height(),surface->Bands());

        dmImageData imData;

        printf("\n### Can I Lock data for reading %s ?\n",path.get());
        rv = surface->LockBits(dmPixelFormatUndefined,imData,cciISurface::ELockRead);
        DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

        printf("\n width = %d\n height = %d\n stride = %d\n Scan0 = @%p\n Depth = %d\n",
               imData.Width,imData.Height,imData.Stride,imData.Scan0,
               dmGetPixelFormatBits(imData.PixelFormat));

        printf("\n### Can I UnLock data ?\n");
        rv = surface->UnlockBits(imData);
        DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));
      }
    }

  DM_END_TEST_BLOCK

  DM_EXIT_TEST
}
