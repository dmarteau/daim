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

#include "cciCOM.h"
#include "cciCOMPtr.h"
#include "cciComponentManagerUtils.h"
#include "cciServiceManagerUtils.h"
#include "cciILoaderService.h"
#include "cciIRemoteSurface.h"
#include "cciIStringIterator.h"
#include "cciIMetaDataContainer.h"
#include "cciString.h"

#include "daim.h"
#include "dmCrt.h"

#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdarg.h>
#include <getopt.h>



CCI_USE_NS


void PrintError( const char* fmt, ... )
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr,fmt,ap);
  va_end(ap);
}


static char gDomain[MAX_PATH] = "\0";
static char gInputFile[MAX_PATH] = "\0";


//////////////////////////////////////////////////
// main

static char usage[] =
"PrintImageData (C) 2009 Zoomorama\n\n"
"usage: printimagedata [options] file\n"
"where options are:\n"
"--domain=<domain name>         : output metadata associated to 'domain name'\n"
"--help                         : print this help\n";

void PrintUsage()
{
  printf(usage);

};

cci_result getOptions( int argc, char ** argv )
{
   static struct option long_options[] =
   {
     // These options don't set a flag.
     // We distinguish them by their indices.
     {"domain"           ,  required_argument, 0, 'd'},
     {"help"             ,  no_argument      , 0, 'h'},
     {0, 0, 0, 0}
   };

   // getopt_long stores the option index here.
   int option_index = 0;

   while(1)
   {
     int c = getopt_long (argc,argv,"d:h",long_options,&option_index);

     if(c == -1)
        break;

     switch(c)
     {
       case 'd':
         strncpy(gDomain,optarg,MAX_PATH);
         break;

       case 'h':
         PrintUsage();
         exit(0);
     }
   }

   /* Read any remaining command as input file name. */
   if (optind < argc)
   {
     strncpy(gInputFile,argv[optind],MAX_PATH);
   }

   if(gInputFile[0]=='\0') {
     PrintError("printimagedata: missing input image\n");
     return CCI_ERROR_INVALID_ARG;
   }

   return CCI_OK;
}


cci_result PrintImagedata( const char* inputFile, const char* domain )
{
  cci_result rv;

  cci_Ptr<cciILoaderService> ldrSrvc = do_GetService("@daim.org/contrib/loader-service;1",&rv);
  if(CCI_FAILED(rv))
     return rv;

  cci_Ptr<cciISurface> surface;

  rv = ldrSrvc->OpenSurface(inputFile,cciIDriverProxy::IO_READONLY,getter_AddRefs(surface));
  if(CCI_FAILED(rv))
     return rv;

  cci_Ptr<cciIMetaDataContainer> mdcontainer = do_QueryInterface(surface,&rv);
  if(CCI_FAILED(rv))
     return rv;

  const char* data;
  cci_Ptr<cciIUTF8StringIterator> enumerator;

  rv = mdcontainer->GetMetadata(domain,getter_AddRefs(enumerator));
  if(rv == CCI_ERROR_NOT_AVAILABLE)
  {
    printf("\n");
    rv = CCI_OK; // No meta data is not an error
  }
  else
  {
    dm_bool hasMore;
    while(CCI_SUCCEEDED(enumerator->HasMore(&hasMore)) && hasMore)
    {
      rv = enumerator->GetNext(&data);
      if(CCI_SUCCEEDED(rv))
         printf("%s\n",data);
    }
  }

  return rv;
}

int main( int argc, char ** argv )
{
  cci_result rv = getOptions(argc,argv);
  if(CCI_FAILED(rv))
     return 5;

  rv = DM_Initialize(dm_null,0,0);
  if(CCI_SUCCEEDED(rv))
  {
    rv = PrintImagedata(gInputFile,gDomain);

    DM_Finalize(false);
  } else {
    PrintError("printimagedata: urggh, failed to initialize DAIM library with error %d.\n",rv);
    rv = CCI_ERROR_FAILURE;
  }

  if(CCI_FAILED(rv))
     PrintError("printimagedata: command failed with error %d.\n",rv);

  return (CCI_FAILED(rv)?2:0);
}

