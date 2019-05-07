#ifndef daim_layers_h
#define daim_layers_h

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
//---------------------------------------------------------------------
class daim_layer_rgn
{
  public:
    EColorType Color;
    dmRegion   Region;
    dmString   Name;

  daim_layer_rgn( const char_t* _Name, EColorType _Color, const dmRegion& _Region )
  :Color (_Color)
  ,Region(_Region)
  ,Name  (_Name)
  {}

  daim_layer_rgn( const daim_layer_rgn& _aCopy )  
  :Color (_aCopy.Color)
  ,Region(_aCopy.Region)
  ,Name  (_aCopy.Name)
  {}
  
  
  void Draw( dmIImage<dmPixelFormat24bppRGB>& _Image ) {
    if(!Region.IsEmptyRoi() && Color!=dmTk::eClr_NONE) {
      Region.ClipToRect(_Image.Rect());
      daim::fill(Region,_Image.Gen(),COLORREF_TO_RGB(Color));
    }
  }
};
//---------------------------------------------------------------------
class daim_layers
{
  public:
  dmTList<daim_layer_rgn> _Layers;
 
  daim_layers() {}
 ~daim_layers() {}
 
  void CreateLayer(const char_t* _Name, EColorType _Color, const dmRegion& _Region );
  void DeleteLayer( const char_t* _Name );
  void ClearAll();
  
  void MoveLayerToBack  ( const char_t* _Name );
  void MoveLayerToFront ( const char_t* _Name );
  void MoveLayerBackward( const char_t* _Name );
  void MoveLayerForward ( const char_t* _Name );
   
  void SetLayerColor(  const char_t* _Name, EColorType _Color ); 
   
  dmTList<daim_layer_rgn>::iterator Find( const char_t* _Name );

  daim_layer_rgn*  GetLayer( const char_t* _Name ) {
    dmTList<daim_layer_rgn>::iterator it = Find(_Name);
    return (it!=_Layers.End() ? &(*it) : NULL);
  }
  
  void Draw( dmIImage<dmPixelFormat24bppRGB>& _Image )
  {
    dmTList<daim_layer_rgn>::iterator it   = _Layers.Begin();  
    dmTList<daim_layer_rgn>::iterator last = _Layers.End();  
    for(;it!=last;++it)
        (*it).Draw(_Image);
  }
};
//---------------------------------------------------------------------
#endif // daim_layers_h
