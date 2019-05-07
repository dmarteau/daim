#ifndef dmRegionTree_h
#define dmRegionTree_h

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
// File         : dmRgnTree.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

//   - Analyse a ROI and create a tree describing the sub-region inclusion 
//     relationships et store the polygone associated to the boundary

struct dmRgnNode 
{  
  typedef dmTList<dmRgnNode> node_list;

  dmRegion  rgn;
  dmPoly    boundary; // boundary of the sub region
  node_list subrois;

  typedef node_list::iterator iterator;
  typedef node_list::const_iterator const_iterator;

  iterator Begin() { return subrois.Begin(); }
  iterator End()   { return subrois.End(); }

  const_iterator Begin() const { return subrois.Begin(); }
  const_iterator End()   const { return subrois.End(); }

  bool   Empty() const { return subrois.Empty(); }
  size_t Count() const { return subrois.Size();  }
  
  bool Compute( const dmRegion& );

  dmRegion&      Region()       { return rgn; }
  const dmRegion Region() const { return rgn; }

  dmPoly&        Coordinates()       { return boundary; }
  const dmPoly&  Coordinates() const { return boundary; }

  void Clear();
  size_t ComputeBoundary();   // compute boundary from stored "rgn" 
  size_t ComputeSubRegions(); // need to compute boundary first
 
  bool BuildConnexRoi( dmRegion& rgn );
  bool IsConnexRegion() const {
     return subrois.First()==subrois.Last(); 
  }

};
//---------------------------------------------------------------------------
// Notes : Rootnode contains the bounding box of the computed region
// Gives Access to sub regions computed from the list elements associated to 
// <rootnode>
//---------------------------------------------------------------------------
class dmRegionTree
{
  private:
    dmRgnNode _rootNode;

  public:
    dmRegionTree( const dmRegion& );
    dmRegionTree();
   ~dmRegionTree();

    dmRgnNode&       GetRoot()       { return  _rootNode; }
    const dmRgnNode& GetRoot() const { return  _rootNode; }

    bool Build(  const dmRegion& );
    bool Build(  const dmRegion&, const dmRect& );
    bool IsConnexRegion()  const { return _rootNode.IsConnexRegion(); }
    bool IsEmpty()         const { return _rootNode.Empty(); }
};
//---------------------------------------------------------------------------



#endif // dmRegionTree_h
