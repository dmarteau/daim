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
// File         : test_downsampling.cpp
// Date         : 16 déc. 2008
// Author       : David Marteau
//--------------------------------------------------------

#include "test_common.h"

#include "cciCOM.h"
#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"
#include "cciIComponentRegistrar.h"
#include "cciIImageShell.h"
#include "cciIImageList.h"
#include "cciIPyramid.h"
#include "cciIImage.h"
#include "cciIImageListImage.h"
#include "cciISurfaceDriver.h"
#include "cciIFilterContext.h"
#include "cciImageUtils.h"
#include "cciString.h"

#include "utils/dmTimer.h"

#include <stdlib.h>


CCI_USE_NS

class test_message
{
  public:
    test_message()
      {
        printf("BEGIN unit tests for image shell, compiled " __DATE__ "\n");
      }

   ~test_message()
      {
        printf("END unit tests for image shell.\n");
      }
};

test_message gTestMessage;

const char  sampleLocation[]     = "../samples/exif/camera.jpg";
const char  saveLocationBase[]   = "../samples/down_sample%d.jpg";

int main( int argc, char ** argv )
{
  DM_BEGIN_TEST_BLOCK

    printf(">>main()\n");

    {
      cci_result rv;

      printf("\n### Can I create an image shell ?\n");
      cci_Ptr<cciIImageShell> shell = do_CreateInstance("@daim.org/image-shell;1",&rv);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      printf("\n### Can I open an image at %s ?\n",sampleLocation);
      rv  = shell->LoadImage(sampleLocation);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      cci_Ptr<cciIImageList> imgList = do_CreateInstance("@daim.org/image/list;1",&rv);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      cci_Ptr<cciIPyramid> pyramid = do_CreateInstance("@daim.org/processing/pyramid;1",&rv);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      cci_Ptr<cciIFilterContext> context;
      rv = shell->GetFilterContext(getter_AddRefs(context));
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      cci_Ptr<cciIImage> image;
      rv = shell->GetImage(getter_AddRefs(image));
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      dmTimer timer;
      
      rv = pyramid->Generate(image,dm_null,imgList,16,context);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

      timer.Stop();

      printf("Duration : %lf\n",timer.Duration());

      dm_uint32 size;
      imgList->GetSize(&size);

      char buf[64];

      cci_Ptr<cciIImageListImage> listimage = do_CreateInstance("@daim.org/imagelist-image;1",&rv);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));
      
      rv = listimage->Init(imgList,0);
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));
      
      cci_Ptr<cciISurfaceDriver> driver;
      rv = CCI_GetLoader(dm_null,"JPEG",true,getter_AddRefs(driver));
      DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));
      
      for(unsigned int i=0;i<size;++i)
      {
        sprintf(buf,saveLocationBase,i+1);

        rv = listimage->Select(i);
        DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));

        printf("\n### Can I save an image to %s ?\n",buf);
        rv  = CCI_SaveImage(driver,buf,listimage);
        DM_TEST_EXPECT_RESULT(CCI_SUCCEEDED(rv));
      }
    }


  DM_END_TEST_BLOCK

  DM_EXIT_TEST
}


