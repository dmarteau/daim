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
// File         : cciMeasurements.cpp
// Date         : 14 févr. 2009
// Author       : David Marteau
//--------------------------------------------------------

#include "cciMeasures.h"

/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciMeasurements, cciIMeasurements)

cciMeasurements::cciMeasurements()
{
  /* member initializers and constructor code */
}

cciMeasurements::~cciMeasurements()
{
  /* destructor code */
}

//=================================
// cciIMeasures implementation
//=================================

/* void build (in cciImage image, in cciRegion rgn, in dm_uint32 connect, in dm_uint32 flags); */
CCI_IMETHODIMP cciMeasurements::Build(cciImage image, cciRegion rgn, dm_uint32 connect, dm_uint32 flags)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void createRegion (in cciRegion mask, in dm_int32 label, in boolean include_holes); */
CCI_IMETHODIMP cciMeasurements::CreateRegion(cciRegion mask, dm_int32 label, dm_bool include_holes)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void clear (in dm_int32 label); */
CCI_IMETHODIMP cciMeasurements::Clear(dm_int32 label)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* [noscript,notxpcom] dm_uint32 getLabels (in cciRegion rgn, [array] out dm_int32 labels); */
CCI_IMETHODIMP_(dm_uint32) cciMeasurements::GetLabels(cciRegion rgn, dm_int32 **labels CCI_OUTPARAM)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* dm_int32 getLabelFromPoint (in dm_int32 x, in dm_int32 y); */
CCI_IMETHODIMP cciMeasurements::GetLabelFromPoint(dm_int32 x, dm_int32 y, dm_int32 *_retval CCI_OUTPARAM)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void removeLabels ([array, size_is (count)] in dm_int32 labels, in dm_uint32 count); */
CCI_IMETHODIMP cciMeasurements::RemoveLabels(dm_int32 *labels, dm_uint32 count)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void selectLabels (in cciRegion mask, [array, size_is (count)] in dm_int32 labels, in dm_uint32 count); */
CCI_IMETHODIMP cciMeasurements::SelectLabels(cciRegion mask, dm_int32 *labels, dm_uint32 count)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void setCalibration (in dm_real unitsPerPixel, in dm_real aSpectRatio); */
CCI_IMETHODIMP cciMeasurements::SetCalibration(dm_real unitsPerPixel, dm_real aSpectRatio)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* [noscript,notxpcom] dm_uint32 getChildRegionLabels (in dm_int32 label, [array] out dm_int32 labels); */
CCI_IMETHODIMP_(dm_uint32) cciMeasurements::GetChildRegionLabels(dm_int32 label, dm_int32 **labels CCI_OUTPARAM)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* [noscript,notxpcom] dm_uint32 getIndexTable ([array] out dm_int32 table); */
CCI_IMETHODIMP_(dm_uint32) cciMeasurements::GetIndexTable(dm_int32 **table CCI_OUTPARAM)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void fillHoles (); */
CCI_IMETHODIMP cciMeasurements::FillHoles()
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void cleanBorders (); */
CCI_IMETHODIMP cciMeasurements::CleanBorders()
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] void getProperties (in dm_int32 label, in dm_uint32 flags, in cciRgnPropertiesRef props); */
CCI_IMETHODIMP cciMeasurements::GetProperties(dm_int32 label, dm_uint32 flags, cciRgnProperties & props)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void reconstruct (in cciRegion mask); */
CCI_IMETHODIMP cciMeasurements::Reconstruct(cciRegion mask)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

/* void cleanRegionBorders (in cciRegion mask); */
CCI_IMETHODIMP cciMeasurements::CleanRegionBorders(cciRegion mask)
{
    return CCI_ERROR_NOT_IMPLEMENTED;
}

///////////////////////////////////////////////////////////////////


//=====================================
// Module
//=====================================

#include "cciIGenericFactory.h"

CCI_GENERIC_FACTORY_CONSTRUCTOR(cciMeasurements)

static const cciModuleComponentInfo components[] = {
    {  CCI_MEASURES_CLASSNAME,
       CCI_MEASURES_CID,
       CCI_MEASURES_CONTRACTID,
       cciMeasurementsConstructor
    },
};

CCI_IMPL_GETMODULE(MeasuresModule, components)


