#ifndef dmLinearMorphology_h
#define dmLinearMorphology_h

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
// File         : dmLinearMorphology.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

#include "templates/processing/dmKernelFamily.h"
#include "templates/processing/_dmMorphology.h"

class dmLinearStructuringElement : public dmMutableKernelDescription
{
  protected:
    int* begin() { return __data; }
    int* end()   { return __data+__width; }

  public:
    typedef const int* iterator;

    dmLinearStructuringElement();
    dmLinearStructuringElement(const dmLinearStructuringElement& _kernel);
    dmLinearStructuringElement(size_t _or,size_t _width,const int* _data);

    void Complement(); // calcule le compl�mentaire de l'�l�ment structurant
    void Transpose();  // calcule le transpos�      de l'�l�ment structurant

    dmLinearStructuringElement& operator=( const dmLinearStructuringElement& );

    iterator Begin() const { return __data; }
    iterator End()   const { return __data+__width; }

    size_t GetLinearMask( const size_t _w ) const; // Taille utile calcul�e � partir de _w
    size_t GetLinearRect( const size_t _w ) const; // cf kernel_description
};

namespace daim {

//--------------------------------------------------------
// Apply morphologic filters on 1D functions :
//--------------------------------------------------------

template<class IT,class Op>
Op _LinearMorphologicTransform(IT _F,IT _L,IT _O,const dmLinearStructuringElement& _element,Op& op)
{
  typedef Op::value_type value_type;
  value_type A1,A2,m1,m2,value;

  if(_F==_L) return op;

  m1 = std::numeric_limits<value_type>::min();
  m2 = std::numeric_limits<value_type>::max();

  dmLinearStructuringElement::iterator it;
  dmLinearStructuringElement::iterator first =_element.Begin();
  dmLinearStructuringElement::iterator last  =_element.End();

  IT X;
  while(_F != _L) {
    A1 = m1; A2 = m2;
    X = _F-_element.Ox();
    for(it=first;it!=last;++it) {
      value = *X++;
      switch( *it ) {
        case  1: if(A2>=value) A2 = value; break; // erosion
        case -1: if(A1<=value) A1 = value; break; // dilatation
      }
    }
    *_O++ = op( *_F++, A1, A2 );
  }
  return op;
};
//-----------------------------------------------------------------------
#define BINDEF( Opname ) template<class T> Opname<T> __BindOp ## Opname( const T& x ) { return Opname<T>(); }
#define BINDOP( x, Opname ) __BindOp ## Opname( x )

BINDEF( _erosion    )
BINDEF( _dilation   )
BINDEF( _hitormiss  )
BINDEF( _thinning   )
BINDEF( _thickening )
//-----------------------------------------------------------------------
template<class IT>
void linear_erosion(IT F, IT L, IT O, const dmLinearStructuringElement& _element )
{
  _LinearMorphologicTransform( F,L,O,_element,BINDOP( *F, _erosion ));
}
//-----------------------------------------------------------------------
template<class IT>
void linear_dilation(IT F, IT L, IT O, const dmLinearStructuringElement& _element )
{
  _LinearMorphologicTransform( F,L,O,_element,BINDOP( *F, _dilation ));
}
//-----------------------------------------------------------------------
template<class IT>
void linear_hitormiss(IT F, IT L, IT O, const dmLinearStructuringElement& _element )
{
  _LinearMorphologicTransform( F,L,O,_element,BINDOP( *F, _hitormiss ));
}
//-----------------------------------------------------------------------
template<class IT>
void linear_thinning(IT F, IT L, IT O, const dmLinearStructuringElement& _element )
{
  _LinearMorphologicTransform( F,L,O,_element,BINDOP( *F, _thinning ));
}
//-----------------------------------------------------------------------
template<class IT>
void linear_thickening(IT F, IT L, IT O, const dmLinearStructuringElement& _element )
{
  _LinearMorphologicTransform( F,L,O,_element,BINDOP( *F, _thickening ));
}
//-----------------------------------------------------------------------
#undef BINDEF
#undef BINDOP

}; // namespace daim

#endif // dmLinearMorphology_h

