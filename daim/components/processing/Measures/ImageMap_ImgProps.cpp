
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
// Compute sum of luminance
//--------------------------------------------------------------
struct ImageProp_CumulateDensity
{
  CCI_INSTANCE_PTR(ImageMap) _This;

  ImageProp_CumulateDensity( CCI_INSTANCE_PTR(ImageMap) _Instance ) 
  :_This(_Instance)
  {}

  void operator()(int x, const dm_float& v ) { 
    _PARTITION_NODE(_This,x).ri_fvalue += v;
  }
};
//---------------------------------------------------------------------
// Compute sum of luminance of order 2
//--------------------------------------------------------------
struct ImageProp_CumulateDensityOrder2
{
  CCI_INSTANCE_PTR(ImageMap) _This;

  ImageProp_CumulateDensityOrder2( CCI_INSTANCE_PTR(ImageMap) _Instance ) 
  :_This(_Instance)
  {}

  void operator()( int x, const dm_float& v ) { 
    _PARTITION_NODE(_This,x).ri_fvalue += v * v;
  }
};
//---------------------------------------------------------------------
// Compute sum of luminance of order N
//--------------------------------------------------------------
struct ImageProp_CumulateDensityOrderN
{
  CCI_INSTANCE_PTR(ImageMap) _This;
  dm_real                    n;

  ImageProp_CumulateDensityOrderN( CCI_INSTANCE_PTR(ImageMap) _Instance, dm_real _Order ) 
  :_This(_Instance),n(_Order)
  {}

  void operator()(int x, const dm_float& v ) { 
    _PARTITION_NODE(_This,x).ri_fvalue += pow(static_cast<dm_real>(v),n);
  }
};
//--------------------------------------------------------------
bool ImageProp_ComputeDensity( CCI_INSTANCE_PTR(ImageMap) _This, 
                               const dmIImage<dmPixelFormat32bppFloat>& _Image,
                               dm_uint  _Order,
                               dm_real* _Results, const dmPoint& p  )
{
  ImageMap_UpdateRoi(_This);

  info_list_type::iterator it   = _This->NodeList.Begin();
  info_list_type::iterator last = _This->NodeList.End();

  for(;it != last; ++it)
     (*it).ri_fvalue = 0;

  dmIImage<dmPixelFormat32bppFloat>* pImg = 
    const_cast<dmIImage<dmPixelFormat32bppFloat>*>(&_Image);

  if(_Order == 1) {
    daim::evaluate(_This->Roi,p,_This->Map,pImg->Gen(),ImageProp_CumulateDensity(_This));
  } else
  if(_Order == 2) {
    daim::evaluate(_This->Roi,p,_This->Map,pImg->Gen(),ImageProp_CumulateDensityOrder2(_This));
  } else {    
    daim::evaluate(_This->Roi,p,_This->Map,pImg->Gen(),ImageProp_CumulateDensityOrderN(_This,_Order));
  } 

  index_table_type& _itable = _This->IndexTable;
  XNODE pNode;

  for(it = _This->NodeList.Begin();it!= last;++it) {
    pNode = &(*it);
    if(pNode->ri_Status==RI_REGION)
       _Results[_itable[pNode->ri_Part]] = pNode->ri_fvalue; 
  } 
  return true;
}
//--------------------------------------------------------------
bool ImageProp_ComputeMeanDensity( CCI_INSTANCE_PTR(ImageMap) _This, 
                               const dmIImage<dmPixelFormat32bppFloat>& _Image,
                               dm_real* _Results, const dmPoint& p  )
{
  ImageMap_UpdateRoi(_This);
  ImageMap_UpdateCount(_This);

  info_list_type::iterator it   = _This->NodeList.Begin();
  info_list_type::iterator last = _This->NodeList.End();

  for(;it != last; ++it)
     (*it).ri_fvalue = 0;

  dmIImage<dmPixelFormat32bppFloat>* pImg = 
    const_cast<dmIImage<dmPixelFormat32bppFloat>*>(&_Image);

  daim::evaluate(_This->Roi,p,_This->Map,pImg->Gen(),ImageProp_CumulateDensity(_This));

  index_table_type& _itable = _This->IndexTable;
  XNODE pNode;

  for(it = _This->NodeList.Begin();it!= last;++it) {
    pNode = &(*it);
    if(pNode->ri_Status==RI_REGION)
       _Results[_itable[pNode->ri_Part]] = (pNode->ri_fvalue/pNode->ri_Count); 
  } 
  return true;
}
//---------------------------------------------------------------------
// Compute min of luminance
//--------------------------------------------------------------
struct ImageProp_MinDensity
{
  CCI_INSTANCE_PTR(ImageMap) _This;
  XNODE                      _Node;

  ImageProp_MinDensity( CCI_INSTANCE_PTR(ImageMap) _Instance ) 
  :_This(_Instance)
  {}

  void operator()( int x, const dm_float& v ) { 
    _Node = &_PARTITION_NODE(_This,x);
    _Node->ri_fvalue = dm_min(_Node->ri_fvalue,static_cast<dm_real>(v));
  }
};
//--------------------------------------------------------------
bool ImageProp_ComputeMinDensity( CCI_INSTANCE_PTR(ImageMap) _This, 
                               const dmIImage<dmPixelFormat32bppFloat>& _Image,
                               dm_real* _Results, const dmPoint& p  ) 
{
  ImageMap_UpdateRoi(_This);

  info_list_type::iterator it   = _This->NodeList.Begin();
  info_list_type::iterator last = _This->NodeList.End();

  for(;it != last; ++it)
     (*it).ri_fvalue = dmIImage<dmPixelFormat32bppFloat>::traits_type::max();

  dmIImage<dmPixelFormat32bppFloat>* pImg = 
    const_cast<dmIImage<dmPixelFormat32bppFloat>*>(&_Image);

  daim::evaluate(_This->Roi,p,_This->Map,pImg->Gen(),ImageProp_MinDensity(_This));

  index_table_type& _itable = _This->IndexTable;
  XNODE pNode;

  for(it = _This->NodeList.Begin();it!= last;++it) {
    pNode = &(*it);
    if(pNode->ri_Status==RI_REGION)
       _Results[_itable[pNode->ri_Part]] = pNode->ri_fvalue; 
  } 
  return true;
}
//---------------------------------------------------------------------
// Compute max of luminance
//--------------------------------------------------------------
struct ImageProp_MaxDensity
{
  CCI_INSTANCE_PTR(ImageMap) _This;
  XNODE                      _Node;

  ImageProp_MaxDensity( CCI_INSTANCE_PTR(ImageMap) _Instance ) 
  :_This(_Instance)
  {}

  void operator()( int x, const dm_float& v ) { 
    _Node = &_PARTITION_NODE(_This,x);
    _Node->ri_fvalue = dm_max(_Node->ri_fvalue,static_cast<dm_real>(v));
  }
};
//--------------------------------------------------------------
bool ImageProp_ComputeMaxDensity( CCI_INSTANCE_PTR(ImageMap) _This, 
                               const dmIImage<dmPixelFormat32bppFloat>& _Image,
                               dm_real* _Results, const dmPoint& p  ) 
{
  ImageMap_UpdateRoi(_This);

  info_list_type::iterator it   = _This->NodeList.Begin();
  info_list_type::iterator last = _This->NodeList.End();

  for(;it != last; ++it)
     (*it).ri_fvalue = dmIImage<dmPixelFormat32bppFloat>::traits_type::min();

  dmIImage<dmPixelFormat32bppFloat>* pImg = 
    const_cast<dmIImage<dmPixelFormat32bppFloat>*>(&_Image);

  daim::evaluate(_This->Roi,p,_This->Map,pImg->Gen(),ImageProp_MaxDensity(_This));

  index_table_type& _itable = _This->IndexTable;
  XNODE pNode;

  for(it = _This->NodeList.Begin();it!= last;++it) {
    pNode = &(*it);
    if(pNode->ri_Status==RI_REGION)
       _Results[_itable[pNode->ri_Part]] = pNode->ri_fvalue; 
  } 
  return true;
}
//---------------------------------------------------------------------
// Extract the image in appropriate format
//---------------------------------------------------------------------
dmIImage<dmPixelFormat32bppFloat>* 
ImageProp_PrepareImage(
  CCI_INSTANCE_PTR(ImageMap) _This,
  cci_Object* params,
  dmLink<dmImage>& _InputImage,
  dmPoint& offs  )
{
  if(params) 
  {
    offs = dmPoint(0,0);    
    
    dmIParameters _IParams;
    if(_IParams.QueryInterface(params))
    {
      dmImage* img = _IParams.GetImage();
      if(img) 
      { 
        dmRegion rgn;
        rgn.SetRectRoi(_This->Roi.Rectangle());
        rgn.Translate(_This->Offset);

        dm_uint  channel = _IParams.GetChannel();

        // Handle multi-channel image
        if(!dmIsPixelFormatScalar(img->PixelFormat()) && 
            channel != dmIParameters::flAllChannels)
        {
          dmIColorSpace _ColorSpace;
          if(_ColorSpace.QueryInterface(_IParams.GetColorSpace()))
          {   
            _ColorSpace.SetChannelFmt(dmPixelFormat32bppFloat);

            dmIInterface<dmIParameters> _Params2;
            _Params2.SetImageList(_ColorSpace); 
            _Params2.SetImage(img);
            _Params2.SetRoi(&rgn);

            if(!_ColorSpace.Extract(_Params2,channel,0)) {
               dmLOG(_TXT("ERR:ImageMap: Invalid channel !\n"));
               return NULL;
            }
            
            // Return the internal link to that image
            _ColorSpace.GetBufferA(0,_InputImage);
          } 
          else 
          {
            dmLOG(_TXT("ERR:ImageMap: Cannot get colorspace !"));
            return NULL;
          }
        }
        else
        {
          if(img->PixelFormat()!=dmPixelFormat32bppFloat) {
            _InputImage = dmCreateCopy( img,rgn.Rectangle(),dmPixelFormat32bppFloat);
          } else {
            offs = rgn.Rectangle().TopLeft();
            _InputImage = dmNewLink<dmImage>(img);
          }
        }
        return dmIImage<dmPixelFormat32bppFloat>::Cast(_InputImage);
      }
    }
  }

  return NULL;    
}
//---------------------------------------------------------------------
