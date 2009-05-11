
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

#define dmUseKernelTemplates
#include "daim_kernel.h"

#include "tools/dmRegionTree.h"
#include "templates/processing/dmObjectDetection.h"

//------------------------------------------------------------------
void dmRgnNode::Clear()
{
  rgn.KillRoi();
  boundary.Clear();
  subrois.Clear();
}
//------------------------------------------------------------------
bool dmRgnNode::Compute( const dmRegion& _roi )
{
  if(!_roi.IsEmptyRoi()) 
  {
    rgn = _roi;
    ComputeBoundary(); 
    ComputeSubRegions(); 
  }
  return boundary.Size()!=0;
}
//------------------------------------------------------------------
size_t dmRgnNode::ComputeBoundary()
{
  return daim::extract_rgn_boundary(rgn,boundary);
}
//------------------------------------------------------------------
size_t dmRgnNode::ComputeSubRegions()
{
  dmRegion holes;          
  BuildConnexRoi(holes); // Build connexe region;
  rgn.AndRoi(holes);     // Isolate Region;
  holes.XorRoi(rgn);     // XOR to get holes;

  while(!holes.IsEmptyRoi()) {     
    iterator it = subrois.Insert(End());
    (*it).Compute(holes);
    holes.SubRoi( (*it).Region() ); // remove computed region
  }
  return Count();
}
//------------------------------------------------------------------
bool dmRgnNode::BuildConnexRoi( dmRegion& rgn )
{
  rgn.KillRoi();
  if(boundary.Size()!=0) rgn.SetRoi(boundary);
  return !rgn.IsEmptyRoi();
}
//------------------------------------------------------------------
dmRegionTree::dmRegionTree( const dmRegion& _roi )
{
  Build(_roi);
}
//------------------------------------------------------------------
dmRegionTree::dmRegionTree() 
{}
//------------------------------------------------------------------
dmRegionTree::~dmRegionTree() 
{}
//------------------------------------------------------------------
bool dmRegionTree::Build(  const dmRegion& _roi, const dmRect& r )
{
  dmASSERT( _roi.Rectangle() >= r );
  _rootNode.Clear();
  dmRect rect = r;
  rect.Resize(2);                 // Resize to get rid of sub-regions touching edges
  _rootNode.rgn = rect;           // Set rectangle
  _rootNode.ComputeBoundary();    // Compute boundary poly
  _rootNode.rgn.SubRoi(_roi);     // Complementary region
  _rootNode.ComputeSubRegions();
  _rootNode.rgn.ClipToRect(r);    // Restore region to originale boundary rectangle

  return !_rootNode.Empty();
}
//------------------------------------------------------------------
bool dmRegionTree::Build(  const dmRegion& _roi )
{
  return Build(_roi,_roi.Rectangle());
}
//------------------------------------------------------------------
