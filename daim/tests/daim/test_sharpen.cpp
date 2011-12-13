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
// File         : test_sharpen.cpp
// Date         : 17 déc. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "test_common.h"

#include "cciCOM.h"
#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"
#include "cciIComponentRegistrar.h"
#include "cciIImageShell.h"
#include "cciISharpenFilter.h"
#include "cciString.h"

#include "utils/dmTimer.h"

#include <stdlib.h>


CCI_USE_NS

class test_message
{
  public:
    test_message()
      {
        printf("BEGIN unit tests for sharpen filter, compiled " __DATE__ "\n");
      }

   ~test_message()
      {
        printf("END unit tests for sharpen filter.\n");
      }
};

test_message gTestMessage;

char  sampleLocation[MAX_PATH] = "\0";
char  saveLocation[MAX_PATH]   = "\0";

int main( int argc, char ** argv )
{
  DM_BEGIN_TEST_BLOCK

    printf(">>main()\n");

    {
      cci_result rv;

      // Get the DAIM_BIN environment variable
      const char* location = getenv("DAIM_BIN");

      DM_TEST_EXPECT_RESULT(location && location[0]!='\0');

      printf("\n### DAIM_BIN set to %s\n",location);

      snprintf(sampleLocation,MAX_PATH-1,"%s/samples/exif/camera.jpg",location);

      printf("\n### Can I create an image shell ?\n");
      cci_Ptr<cciIImageShell> shell = do_CreateInstance("@daim.org/image-shell;1",&rv);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      printf("\n### Can I open an image at %s ?\n",sampleLocation);
      rv  = shell->LoadImage(sampleLocation);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      printf("\n### Can I create a sharpen filter ?\n");
      cci_Ptr<cciISharpenFilter> sharpen = do_CreateInstance("@daim.org/filters/sharpen;1",&rv);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      dm_uint32 radius   = 5;
      double    strength = 0.8;

      printf("\n### Can I set parameters ?\n");
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(sharpen->SetRadius(radius)));
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(sharpen->SetStrength(strength)));

      printf("\n### Can I Apply filter ?\n");

      dmTimer timer;

      rv = shell->ApplyFilter(sharpen,dm_null);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      timer.Stop();

      printf("Duration : %lf\n",timer.Duration());

      snprintf(saveLocation,MAX_PATH-1,"%s/samples/sharpen_r%d_s%0.2lf.jpg",location,radius,strength);

      printf("\n### Can I save image to %s ?\n",saveLocation);
      rv  = shell->SaveImage(saveLocation,"JPEG",0);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));
    }


  DM_END_TEST_BLOCK

  DM_EXIT_TEST
}




