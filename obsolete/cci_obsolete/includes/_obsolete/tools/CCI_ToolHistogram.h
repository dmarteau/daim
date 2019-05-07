#ifndef CCI_ToolHistogram_h
#define CCI_ToolHistogram_h

/* ::: BEGIN LICENSE BLOCK:::
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
 *  ::: END LICENSE BLOCK::: */

//--------------------------------------------------------
// File         : CCI_ToolHistogram.h
// Date         : 5/2005
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------
#define CCIC_ToolHistogram  _TXT("tools/tlhistogram.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( ToolHistogram, Manager )

  CCI_MODULE_UUID( ToolHistogram,515c91ef-ce20-45ef-9068-b0b5958cd826)

  CCI_MODULE_VERSION( ToolHistogram, 1 ) 

  /* Methods */
  CCI_DECL_METHOD_PP(ToolHistogram,1,  Scan , 
     _in dm_cci(Parameters) params
     _in dm_uint            index
     _in dm_uint            flags 
  )

  CCI_DECL_METHOD_PP(ToolHistogram, 2, GetRange, 
       _out dm_real lower
       _out dm_real upper
  )

  CCI_DECL_METHOD_PP(ToolHistogram, 3, SetRange, 
       _in dm_real lower
       _in dm_real upper
  )

  CCI_DECL_METHOD_PP(ToolHistogram, 4, GetValues, 
       _in  dm_int  cursor
       _out dm_real value
       _out dm_real density
  )

  CCI_DECL_METHOD_PP(ToolHistogram, 5, Index, 
       _in  dm_uint  index
       _in  dm_param me
  )

  CCI_DECL_METHOD_PP(ToolHistogram ,6,  Data, 
     _out dm_array(dm_real,count) data
     _out dm_uint                 count 
  )

  CCI_DECL_METHOD_PP(ToolHistogram, 7, Threshold, 
       _in  dm_param me
       _out dm_real lower
       _out dm_real upper
  )

  CCI_DECL_METHOD_PP(ToolHistogram, 8,  GetStats, 
      _out dm_real count 
      _out dm_real mean 
      _out dm_real stddev 
      _out dm_real rms 
      _out dm_real min
      _out dm_real max
  )

  /* Attributs */
  CCI_DECL_ATTR(ToolHistogram,1,  LogScale      , dm_bool , [ISGN] )
  CCI_DECL_ATTR(ToolHistogram,2,  ReadOnly      , dm_bool , [ISGN] )
  CCI_DECL_ATTR(ToolHistogram,3,  Thresholding  , dm_bool , [ISGN] )
  CCI_DECL_ATTR(ToolHistogram,4,  Channels      , dm_uint , [.SGN] )
  CCI_DECL_ATTR(ToolHistogram,5,  Histograms    , dm_cci(Histograms) , [..G.] )

CCI_END_MODULE_DECL( ToolHistogram )

#endif // __doxygen

#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup ToolManagers
 *  \brief Daim ToolHistogram interface
 *
 */
//------------------------------------------------------------------------------------
class dmIToolHistogram : public dmIManager
{    
  public:
   dmDECLARE_GENERIC_IINTERFACE(ToolHistogram)


};

#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

//-------------------------------------------------------------
#endif /* CCI_ToolHistogram_h */
