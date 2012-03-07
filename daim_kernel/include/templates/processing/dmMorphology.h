#ifndef dmMorphology_h
#define dmMorphology_h

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
// File         : dmMorphology.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

// Implementation of morphological operators

#include "_dmMorphology.h"

namespace daim {

//---------------------------------------------------------------------------
// in      : image to be processed (will be modified)
// tmp     : temporary buffer      (must be same size as in)
// _rgn    : input region
// _src    : output location
// _family : kernel family
// iter    : number of iterations
//
// Note : the input and output images should be large enough to handle
// extra storage due to the kernel size, to find out how much
// of extra space is needed, use the GetRect() member function
// from dmKernelFamily
//
// __DECLARE_RGN_BNDRY suggests what kind of boundary conditions
// should be applied
//---------------------------------------------------------------------------


#define __DECLARE_RGN_BNDRY( t )
//---------------------------------------------------------------------------
// Erosion
//---------------------------------------------------------------------------
template<class T>
void erosion( image<T>& in, image<T>& tmp,
              const dmRegion& rgn,
              const dmKernelFamily& _family,
              int iter )
{
   __DECLARE_RGN_BNDRY(dmOBJECT_BOUNDARY)

   dmDEBUG_ASSERT( equal_size(in,tmp) );

   typename image<T>::line_type lin   = in.begin ( rgn.Rectangle() );
   typename image<T>::line_type lout  = tmp.begin( rgn.Rectangle() );
   typename image<T>::line_type lsave = lin;

   filter::kernel<dmKernelDescription::value_type>  _kernel;

   for(int i=0;i<iter;++i) {
     for(size_t k=0;k<_family.Size();++k) {
       _kernel.fromDescription(_family[k]);
       morphology::apply(rgn,lin,lout,_kernel,morphology::erosion<T,T>());
       std::swap(lin,lout);
     }
   }

   // Because of the swap we need
   // to restore result in <in>
   if(lin!=lsave)
     core::copy(rgn,lin,lsave);
}
//---------------------------------------------------------------------------
// Dilation
// il faut prendre le complement du noyau pour une dilatation car
// l'operateur atomique renvoie A1 qui est d�finie pour les valeurs -1
// du noyau
//---------------------------------------------------------------------------
template<class T>
void dilation( image<T>& in, image<T>& tmp,
         const dmRegion& rgn,
         const dmKernelFamily& family,
         int iter )
{
   __DECLARE_RGN_BNDRY(eZERO_BOUNDARY)

   dmDEBUG_ASSERT( equal_size(in,tmp) );

   typename image<T>::line_type lin   = in.begin ( rgn.Rectangle() );
   typename image<T>::line_type lout  = tmp.begin( rgn.Rectangle() );
   typename image<T>::line_type lsave = lin;

   dmKernelFamily& _family = const_cast<dmKernelFamily&>(family);
   filter::kernel<dmKernelDescription::value_type>  _kernel;

   _family.Complement();

   for(int i=0;i<iter;++i) {
     for(size_t k=0;k<_family.Size();++k) {
       _kernel.fromDescription(_family[k]);
       morphology::apply(rgn,lin,lout,_kernel,morphology::dilation<T,T>());
       std::swap(lin,lout);
     }
   }
   _family.Complement();

   if(lin!=lsave)
     core::copy(rgn,lin,lsave);
}
//---------------------------------------------------------------------------
// Thinning
//---------------------------------------------------------------------------
template<class T>
void thinning( image<T>& in, image<T>& tmp,
         const dmRegion& rgn,
         const dmKernelFamily& _family,
         int iter )
{
   __DECLARE_RGN_BNDRY(eOBJECT_BOUNDARY)

   dmDEBUG_ASSERT( equal_size(in,tmp) );

   typename image<T>::line_type lin   = in.begin ( rgn.Rectangle() );
   typename image<T>::line_type lout  = tmp.begin( rgn.Rectangle() );
   typename image<T>::line_type lsave = lin;

   filter::kernel<dmKernelDescription::value_type>  _kernel;

   for(int i=0;i<iter;++i) {
     for(size_t k=0;k<_family.Size();++k) {
       _kernel.fromDescription(_family[k]);
       morphology::apply(rgn,lin,lout,_kernel,morphology::thinning<T,T>());
       std::swap(lin,lout);
     }
   }

   if(lin!=lsave)
     core::copy(rgn,lin,lsave);
}
//---------------------------------------------------------------------------
// Thickening
//---------------------------------------------------------------------------
template<class T>
void thickening( image<T>& in, image<T>& tmp,
         const dmRegion& rgn,
         const dmKernelFamily& family,
         int iter )
{
  __DECLARE_RGN_BNDRY(eZERO_BOUNDARY)

  dmDEBUG_ASSERT( equal_size(in,tmp) );

  typename image<T>::line_type lin   = in.begin ( rgn.Rectangle() );
  typename image<T>::line_type lout  = tmp.begin( rgn.Rectangle() );
  typename image<T>::line_type lsave = lin;

  dmKernelFamily& _family = const_cast<dmKernelFamily&>(family);
  filter::kernel<dmKernelDescription::value_type>  _kernel;

  _family.Complement();

  for(int i=0;i<iter;++i) {
    for(size_t k=0;k<_family.Size();++k) {
      _kernel.fromDescription(_family[k]);
      morphology::apply(rgn,lin,lout,_kernel,morphology::thickening<T,T>());
      std::swap(lin,lout);
    }
  }
  _family.Complement();

  if(lin!=lsave)
    core::copy(rgn,lin,lsave);
}
//---------------------------------------------------------------------------
// Opening )
//---------------------------------------------------------------------------
template<class T>
void opening( image<T>& in, image<T>& tmp,
              const dmRegion& rgn,
              const dmKernelFamily& family,
              int iter )
{
   __DECLARE_RGN_BNDRY(eZERO_BOUNDARY)

   dmDEBUG_ASSERT( equal_size(in,tmp) );

   typename image<T>::line_type lin   = in.begin ( rgn.Rectangle() );
   typename image<T>::line_type lout  = tmp.begin( rgn.Rectangle() );
   typename image<T>::line_type lsave = lin;

   dmKernelFamily& _family = const_cast<dmKernelFamily&>(family);
   filter::kernel<dmKernelDescription::value_type> _kernel;

   int i;
   for(i=0;i<iter;++i) {
     for(size_t k=0;k<_family.Size();++k) {
       _kernel.fromDescription(_family[k]);
       morphology::apply(rgn,lin,lout,_kernel,morphology::erosion<T,T>());
       std::swap(lin,lout);
     }
   }

   _family.Transpose();
   _family.Complement();

   for(i=0;i<iter;++i) {
     for(size_t k=0;k<_family.Size();++k) {
       _kernel.fromDescription(_family[k]);
       morphology::apply(rgn,lin,lout,_kernel,morphology::dilation<T,T>());
       std::swap(lin,lout);
     }
   }
   _family.Complement();
   _family.Transpose();

   if(lin!=lsave)
     core::copy(rgn,lin,lsave);
}
//---------------------------------------------------------------------------
// Closing
//---------------------------------------------------------------------------
template<class T>
void closing( image<T>& in, image<T>& tmp,
              const dmRegion& rgn,
              const dmKernelFamily& family,
              int iter )
{
   __DECLARE_RGN_BNDRY(eOBJECT_BOUNDARY)

   dmDEBUG_ASSERT( equal_size(in,tmp) );

   typename image<T>::line_type lin   = in.begin ( rgn.Rectangle() );
   typename image<T>::line_type lout  = tmp.begin( rgn.Rectangle() );
   typename image<T>::line_type lsave = lin;

   dmKernelFamily& _family = const_cast<dmKernelFamily&>(family);
   filter::kernel<dmKernelDescription::value_type> _kernel;

   int i;

   _family.Complement();
   for(i=0;i<iter;++i) {
     for(size_t k=0;k<_family.Size();++k) {
       _kernel.fromDescription(_family[k]);
       morphology::apply(rgn,lin,lout,_kernel,morphology::dilation<T,T>());
       std::swap(lin,lout);
     }
   }
   _family.Complement();
   _family.Transpose();
   for(i=0;i<iter;++i) {
     for(size_t k=0;k<_family.Size();++k) {
       _kernel.fromDescription(_family[k]);
       morphology::apply(rgn,lin,lout,_kernel,morphology::erosion<T,T>());
       std::swap(lin,lout);
     }
   }
   _family.Transpose();

   if(lin!=lsave)
     core::copy(rgn,lin,lsave);
}

//---------------------------------------------------------------------------
// Top Hat
// out must hold the original image
//---------------------------------------------------------------------------
template<class T>
void top_hat( image<T>& in, image<T>& out, image<T>& tmp,
              const dmRegion& rgn,
              const dmPoint& _src,
              const dmKernelFamily& _family,
              int iter )
{
   __DECLARE_RGN_BNDRY(eZERO_BOUNDARY)
   opening(in,tmp,rgn,_family,iter);
   isub_images(rgn,_src,in,out);
}
//---------------------------------------------------------------------------
// Bottom Hat
//---------------------------------------------------------------------------
template<class T>
void bottom_hat( image<T>& in, image<T>& out, image<T>& tmp,
                 const dmRegion& rgn,
                 const dmKernelFamily& _family,
                 const dmPoint& _src,
                 int iter )
{
   __DECLARE_RGN_BNDRY(eOBJECT_BOUNDARY)
   closing(in,tmp,rgn,_family,iter);
   sub_images(rgn,_src,in,out);
}

//---------------------------------------------------------------------------
// Hitormiss
//---------------------------------------------------------------------------
template<class T>
void hitormiss( image<T>& in, image<T>& out, image<T>& tmp,
                const dmRegion& rgn,
                const dmPoint& _src,
                dmKernelFamily& _family )
{
  __DECLARE_RGN_BNDRY(eZERO_BOUNDARY)

  typename image<T>::line_type lin   = in.begin ( rgn.Rectangle() );
  typename image<T>::line_type lout  = tmp.begin( rgn.Rectangle() );

  filter::kernel<dmKernelDescription::value_type>  _kernel;

  out.fill(pixel_traits<T>::zero());

  typedef typename pixel_traits<T>::value_type value_type;

  for(size_t k=0;k<_family.Size();++k) {
    _kernel.fromDescription(_family[k]);
    morphology::apply(rgn,lin,lout,_kernel,morphology::hitormiss<T,T>());
    RoiOperation( core::functional::Combine< max_of<value_type> >( max_of<value_type>() ),
                  tmp,out,rgn,_src);
  }
}
//---------------------------------------------------------------------------
#undef __DECLARE_RGN_BNDRY

}; // namespace daim


#endif // dmMorphology_h
