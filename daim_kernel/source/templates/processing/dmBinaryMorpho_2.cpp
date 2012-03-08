
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

#include "templates/processing/dmKernelFamily.h"
#include "templates/processing/dmBinaryMorphology.h"
#include "templates/processing/dmPartition.h"

#include <algorithm>
#include <vector>

namespace daim {

//--------------------------------------------------------------------
// Principe
// 1 - effectuer des erosion successives sur la region
// 2 - soustraire la region obtenue de la region pr�cedente
// 2 - ajouter 1 dans la map pour cette la region
//--------------------------------------------------------------------
int distance_map( const dmRegion& _rgn, const dmKernelFamily& _family,
                  image<dm_uint16>& _map )
{
  _map.fill(0);
  dmRegion R,E,*pR,*pE,R2;
  int n = 0;

  dmRect r = _map.rect();

  R.SetRectRoi(r);
  R.GetOutLine(R2);   // bords de l'image;
  R2.AndRoi(_rgn);    // regions de l'objet adjacent aux bords;

  if(!R2.IsEmptyRoi()) {                    // On a des objets qui touche le bord
    dmMaskDescription _mask;                // Compute Borders
    _family.GetMaskDescription(_mask);
    R2.SetRectRoi(_mask.GetRect(r));
    R2.SubCoordinates(r);
  }

  R = _rgn;

  pR = &R; pE = &E;
  while(!pR->IsEmptyRoi())
  {
    pR->ClipToRect(r);
    fill(*pR,_map,++n);

    if(!R2.IsEmptyRoi())
        pR->AddRoi(R2);

    for(size_t k=0;k<_family.Size();++k) {
      erode_region(*pR,*pE,_family[k]);
      std::swap(pR,pE);
    }
  };
  return n;
}
//--------------------------------------------------------------------
// on travail en 4-connectivite
//--------------------------------------------------------------------
struct _assign_label
{
  void operator()( image<label_type>::line_type in, long x1, long x2 )
  {
    // On teste si le voisinage du pixel x contient un label unique
    // si oui on lui affecte la valeurs de ce label
    // sinon on onvalide le pixel en lui affectant la valeur -1

    label_type c,t,lbl;
    for(dm_int x=x1;x<=x2;++x)
    {
      if( (*in)[x] == 0 ) {  // pixel valide
         lbl=c=0;
         while(lbl<=0 && ++c<=4) { // recherche d'un label valide
           switch(c) {
             case 1 : lbl=in[-1][x]  ; break;
             case 2 : lbl=in[0][x+1] ; break;
             case 3 : lbl=in[1][x]   ; break;
             case 4 : lbl=in[0][x-1] ; break;
           }
         }
         while(lbl>0 && ++c<=4) {
           switch(c) {
             case 2 : if( (t=in[0][x+1])>0 && t!=lbl) lbl=-1; break;
             case 3 : if( (t=in[1][x]  )>0 && t!=lbl) lbl=-1; break;
             case 4 : if( (t=in[0][x-1])>0 && t!=lbl) lbl=-1; break;
           }
         }
         (*in)[x] = lbl; // assigne le label
      }
    }
  }
};
//--------------------------------------------------------------------
#define _KSIZE 3
#define _KLENGTH 9
static dmStructuringElement::value_type _H8[_KLENGTH] =
{
   0,-1, 0,
  -1,-1,-1,
   0,-1, 0,
};

//--------------------------------------------------------------------
int separate_particules( const dmRegion& _rgn, dmRegion& _dest,
                         const dmKernelFamily& _family,
                         const dmRegion* Seeds,
                         const dmKernelFamily* kC )
{
  if(_rgn.IsEmptyRoi()) return 0;

  dmRegion R,S,E,Q;

  dmRect r= _rgn.Rectangle();

  std::vector<dmRegion> X;

  dmKernelFamily& kD = const_cast<dmKernelFamily&>(_family);

  R = _rgn;
  R.Translate(1-r.Left(),1-r.Top());

  if(Seeds) {
    Q = *Seeds;
    Q.Translate(1-r.Left(),1-r.Top());
  }

  r.Resize(1);

  cont_image<label_type> _dist( r );

  S = R;
  _dist.fill(0);

  // Calcul de la fonction distance
  dmRegion *pR=&R,*pE=&E;
  for(int n=0;;)
  {
    fill(*pR,_dist,++n);
    X.push_back(*pR);
    for(size_t k=0;k<kD.Size();++k) {
      erode_region(*pR,*pE,kD[k]);
    }
    std::swap(pR,pE);
    if(pR->IsEmptyRoi()) break;
  };

  cont_image<dm_int> _map( r );
  _map.fill(0);

  // On recupere les maxima locaux
  // Et on construit la region complementaire dans S
  local_maxima8(S,dmPoint(1,1),_dist,_map);

  create_rgnroi(_map,std::bind2nd(std::not_equal_to<dm_int>(),
          pixel_traits<dm_int>::object()),S,_map.rect());

  if(Seeds) {
    _map.fill(0);
    fill(Q,_map,pixel_traits<dm_int>::object());
  } else if(kC) {
    //----------------------------------------
    // Les erosions ont tendance a morceler trop
    // les regions, on corrige  en effectuant une
    // dilatation par le noyau utilise et une fermeture
    // par la famille _C, le resultat est alors utilise
    // comme germes
    //----------------------------------------
    Q = S;
    Q.XorCoordinates(_dist.rect());

    kD.Complement();

    for(size_t k=0;k<kD.Size();++k)
      dilate_region(Q,E,kD[k]);

    kD.Complement();

    binary_closing(E,Q,*kC,1);
    _map.fill(0);
    Q.AndCoordinates(_map.rect());
    fill(Q,_map,pixel_traits<dm_int>::object());
  }

  basic_partition __partition;
  create_map(_dist,__partition,_map,std::equal_to<dm_int>(),connect8);
  relabel_image_map(__partition,_dist);

  fill(S,_dist,0);
  S.XorCoordinates(_dist.rect());

  dmStructuringElement _di(1,1,_KSIZE,_KSIZE,_H8);

  E.KillRoi();
  Q = S;
  for(int k=X.size();--k>=0;) {
     for(;;) {
       dilate_region(S,R,_di);
       R.SubRoi(S);
       R.AndRoi(X[k]);
       if(R.IsEmptyRoi()) { S.AddRoi(Q); break; }
       core::RoiOperation(R,_dist.begin(R.Rectangle()),_assign_label());
       create_rgnroi(_dist,std::bind2nd(std::equal_to<int>(),-1),E,R);
       R.SubRoi(E);
       if(R.IsEmptyRoi()) break;
       S.AddRoi(R);
     };
  }

  _dest = S;
  _dest.Translate(r.Left(),r.Top());
  return X.size();
}
//--------------------------------------------------------------------
int ultimate_dilation( const dmRegion& _rgn, dmRegion& _dest,
                       const dmKernelFamily& _family,
                       const dmRegion* _cond )
{
  if(_rgn.IsEmptyRoi()) return 0;

  dmKernelFamily& kD = const_cast<dmKernelFamily&>(_family);

  dmRegion R,E,C,S,Q,X,Z;

  if(_cond)
    C = *_cond;
  else
    C.SetRectRoi(_rgn.Rectangle());

  dmRect r = C.Rectangle();

  R = _rgn;
  R.ClipToRect(r);

  C.Translate(1-r.Left(),1-r.Top());
  R.Translate(1-r.Left(),1-r.Top());
  r.Resize(1);

  cont_image<label_type>    _map ( r, 0 );
  cont_image<unsigned char> _dist( r, 0 );


  fill(R,_dist,1);

  // On effectue une partition des regions
  // qui correspondent aux germes
  basic_partition __partition;
  create_map(_map,__partition,_dist,std::equal_to<label_type>(),connect4);
  relabel_image_map(__partition,_map);

  S = R;
  S.XorCoordinates(_map.rect());
  fill(S,_map,0);

  dmStructuringElement _di(1,1,_KSIZE,_KSIZE,_H8);
  dmRegion *pR=&R,*pE=&E;


  S = R;
  kD.Complement();

  int n;

  for(n=1;;++n)
  {
    for(size_t k=0;k<kD.Size();++k) {
      dilate_region(*pR,*pE,kD[k]);
    }
    X = *pE;
    X.SubRoi(*pR);  // Region to explore
    X.AndRoi(C);
    if(X.IsEmptyRoi()) break;

    for(;;) {
      dilate_region(S,Q,_di);
      Q.SubRoi(S);
      Q.AndRoi(X);
      if(Q.IsEmptyRoi()) break;
      core::RoiOperation(Q,_map.begin(Q.Rectangle()),_assign_label());
      create_rgnroi(_map,std::bind2nd(std::equal_to<int>(),-1),Z,Q);
      Q.SubRoi(Z);
      if(Q.IsEmptyRoi()) break;
      S.AddRoi(Q);
    }
    std::swap(pR,pE);
  };

  kD.Complement();
  _dest = S;
  _dest.Translate(r.Left(),r.Top());
  return n;
}
//--------------------------------------------------------------------
int ultimate_erosion( const dmRegion& _rgn, dmRegion& _dest, const dmKernelFamily& _family )
{
  dmRect r = _rgn.Rectangle();
  r.Resize(1);

  cont_image<dm_uint16> _map(r);
  cont_image<dm_uint8>  _buf(r);

  typedef cont_image<dm_uint8>::traits_type traits_type;

  dmRegion _mask = _rgn;
  dmPoint  _offs = dmPoint(1,1) - _rgn.Rectangle().TopLeft();
  _mask.Translate( _offs );

  // Create a distance map
  int n = distance_map(_mask,_family,_map);

  // Take local maxima
  local_maxima8(_mask,dmPoint(1,1),_map,_buf);

  // Create the mask
  create_rgnroi(_buf,std::bind2nd(
       std::equal_to<traits_type::value_type>(),traits_type::max()),
       _dest,_mask);

  _dest.Translate( -_offs );
  return n;
}
//--------------------------------------------------------------------
// Effectue une reconstruction de la region <_rgn> a partir de la
// region <_dest>, cette operation differe de la dilatation ultime
// conditionnelle en ce sens ou deux elements de <dest> sont fusionnes
// si il correspondent a la meme zone connexe dans <_rgn>.
//--------------------------------------------------------------------
int reconstruct( const dmRegion& _rgn, dmRegion& _dst,
                 connectivity _connect  )
{
  if(_rgn.IsEmptyRoi()) return 0;
  if(_dst.IsEmptyRoi()) return 0;

  dmRegion R;

  dmRect r = _rgn.Rectangle();
  R        = _rgn;
  // offset regions;
  R.Translate   (-r.Left(),-r.Top());
  _dst.Translate(-r.Left(),-r.Top()) ;

  // Build map
  cont_image<label_type> __map ( R.Rectangle(), 0 );
  cont_image<dm_uint8>   __buff( R.Rectangle(), 0 );
  fill(R,__buff,1);
  basic_partition __partition;
  create_map(__map,__partition,__buff,std::equal_to<label_type>(),_connect);

  R.XorCoordinates(__map.rect());
  fill(R,__map,0);

  label_type index;
  // collect labels
  _dst.ClipToRect(R.Rectangle());
  labels_array_type labels;
  index = get_overlapping_regions_labels(_dst,__partition,__map,labels);
  index = merge_labels( __partition , labels, index );

  // create region
  _dst.KillRoi();
  create_roi(__map,__partition.bind(std::bind2nd(std::equal_to<dm_int>(),index)),_dst);

  _dst.Translate(r.Left(),r.Top());

  return labels.size();
}
//--------------------------------------------------------------------
// Simple watershed implementation based on the same algorithm
// as particle separation. Regions where pixels are == 0 are
// discarded.
//--------------------------------------------------------------------
int watershed_simple( const image<dm_uint8>& _image, dmRegion& _dest,
                      const dmRegion* Seeds )
{
  dmRegion R,S,E,Q,X;

  dmRect r = _image.rect();

  R = r;
  R.Translate(1-r.Left(),1-r.Top());

  if(Seeds) {
    Q = *Seeds;
    Q.Translate(1-r.Left(),1-r.Top());
  }

  r.Resize(1);

  cont_image<label_type> _dist( r );
  cont_image<label_type> _map ( r );

  S = R;
  daim::copy(_image.rect(),dmPoint(1,1),_image,_dist);
  _map.fill(0);

  // On recupere les maxima locaux
  // Et on construit la region complementaire dans S
  local_maxima8(S,dmPoint(1,1),_dist,_map);

  create_rgnroi(_map,std::bind2nd(std::not_equal_to<dm_int>(),
          pixel_traits<dm_int>::object()),S,_map.rect());

  if(Seeds) {
    _map.fill(0);
    fill(Q,_map,pixel_traits<dm_int>::object());
  }

  //----------------------------------------
  // label maxima by creating a partition
  // of the image of the local max.
  //----------------------------------------
  basic_partition __partition;
  create_map(_dist,__partition,_map,std::equal_to<dm_int>(),connect8);
  relabel_image_map(__partition,_dist);

  fill(S,_dist,0);
  S.XorCoordinates(_dist.rect());

  dmStructuringElement _di(1,1,_KSIZE,_KSIZE,_H8);

  // Copy back image into map
  daim::copy(_image.rect(),dmPoint(1,1),_image,_map);

  E.KillRoi();
  Q = S;
  for(dm_int k=255;--k>0;)
  {
     create_roi(_map,std::bind2nd(std::greater_equal<dm_int>(),k),X);
     if(X.IsEmptyRoi())
        continue;

     for(;;) {
       dilate_region(S,R,_di);
       R.SubRoi(S);
       R.AndRoi(X);
       if(R.IsEmptyRoi()) { S.AddRoi(Q); break; }
       core::RoiOperation(R,_dist.begin(R.Rectangle()),_assign_label());
       create_rgnroi(_dist,std::bind2nd(std::equal_to<int>(),-1),E,R);
       R.SubRoi(E);
       if(R.IsEmptyRoi()) break;
       S.AddRoi(R);
     };
  }

  _dest = S;
  _dest.Translate(r.Left(),r.Top());
  return 1;
}
//--------------------------------------------------------------------
} // namespace daim
