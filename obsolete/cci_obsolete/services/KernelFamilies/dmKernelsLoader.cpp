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

#define dmUserIncludes
#include "daim_kernel.h"
#include "dmKernelsLoader.h"

#include "daim_kernel/operators/dmOperators.h"

#include <algorithm>

//-----------------------------------------------
#define _TESTCODE( s, t, n )  if(s == t) return n
//-----------------------------------------------
int static __GetOperator( const dmString& s )
{
  if(!s.Empty()) {
   _TESTCODE(s,_TXT("ADD") ,dmTk::Math::AddPixels);
   _TESTCODE(s,_TXT("SUB") ,dmTk::Math::SubPixels);
   _TESTCODE(s,_TXT("OR")  ,dmTk::Math::OrPixels);
   _TESTCODE(s,_TXT("XOR") ,dmTk::Math::XorPixels);
   _TESTCODE(s,_TXT("AND") ,dmTk::Math::AndPixels);
   _TESTCODE(s,_TXT("MIN") ,dmTk::Math::MinPixels);
   _TESTCODE(s,_TXT("MAX") ,dmTk::Math::MaxPixels);
   _TESTCODE(s,_TXT("NSUB"),dmTk::Math::NSubPixels);
   _TESTCODE(s,_TXT("DIFF"),dmTk::Math::DiffPixels);
   _TESTCODE(s,_TXT("ABS") ,dmTk::Math::AbsPixels);
  }
  return dmTk::None;
};
//---------------------------------------------------
enum {
  eKFIRST,
  eKNONE,
  eKFAMILY,
  eKKERNEL,
  eKSKIP
};
//---------------------------------------------------
dmKernelsLoader::dmKernelsLoader(dmIXMLParser& _parser) 
: dmIXMLHelper(_parser), nState(eKSKIP) 
{}
//---------------------------------------------------
void dmKernelsLoader::OnBeginDocument()
{
  if(pSet) {
    pSet->Clear();
    nState =  eKFIRST;
  }
}
//---------------------------------------------------
void dmKernelsLoader::OnEndDocument()
{
  nState =  eKSKIP;
}
//---------------------------------------------------
void dmKernelsLoader::OnStartItem()
{
  const dmString::E* _tag = _parser.GetTag();
  if(_tag) {
    switch(nState) {
      case eKFIRST :
        pSet->SetName ( _tag );
        pSet->SetTitle( _parser.GetProperty(_TXT("title"),_tag) );
        nState = eKNONE;
        break; // first _item
      case eKNONE  :
        kX = kY = kWidth = kHeight = 0;
        if(_tcscmp(_tag,_TXT("family"))==0) { // nouvelle famille
           pCurrent = new dmKernelsSet::Element;
           pCurrent->parent = -1;
           pCurrent->name   = _parser.GetProperty(_TXT("name") ,_TXT(""));
           pCurrent->title  = _parser.GetProperty(_TXT("title"),_TXT(""));
           pCurrent->mode   = _parser.GetProperty(_TXT("mode") ,_TXT(""));
           kX               = _parser.GetProperty(_TXT("x"),0L );
           kY               = _parser.GetProperty(_TXT("y"),0L );
           kWidth           = _parser.GetProperty(_TXT("width") ,0L );
           kHeight          = _parser.GetProperty(_TXT("height"),0L );

           pCurrent->family.SetMode(__GetOperator(pCurrent->mode));

           pSet->Add(pCurrent);
           nState = eKFAMILY;
        } break;
      case eKFAMILY  : 
        if(_tcscmp(_tag,_TXT("kernel"))==0) { // nouveau noyau
           rElement.SetDescription(   // valeurs ou valeurs par défaut
             _parser.GetProperty(_TXT("x"),kX),
             _parser.GetProperty(_TXT("y"),kY),
             _parser.GetProperty(_TXT("width") ,kWidth),
             _parser.GetProperty(_TXT("height"),kHeight)
           );
           nState = eKKERNEL;
        } break;
    }
  }
}
//---------------------------------------------------
void dmKernelsLoader::OnEndItem()
{
  switch(nState) { 
    case eKFAMILY  : {
        const dmString::E* _content = _parser.GetContent();
        if(_content) pCurrent->description =  _content;
        nState = eKNONE  ; 
      } break;
    case eKKERNEL  : {
       // récuperer les valeurs du noyau
       const dmString::E* _content = _parser.GetContent();
       if(_content) {
         dmString val,sc = _content;
         sc.ReplaceTokens(_TXT("\t\n,;"),_TXT(' ')); // remove all white spaces
         int* pData = (int*)rElement.Data();
         size_t i,pos;
         for(i=0,pos = 0;pos!=dmString::npos && i<rElement.Size();++i) {
           pos = sc.Split(val,pos,NULL); 
           pData[i] = val.AsInt();
         }
         if(i!=rElement.Size()) {
           dmTRACE(_TXT("Incomplete kernel élement...skipping"));
         } else { 
           #ifdef _DEBUG
             dmLOG(_TXT("Adding Kernel element to family %s\n"),pCurrent->name.CStr());
             dmLOG(_TXT("x=%ld y=%ld w=%ld h=%ld\n"),
                 rElement.Ox(),rElement.Oy(),
                 rElement.Width(),rElement.Height());

             size_t i;
             for(i=0;i<rElement.Size();++i) {
                 dmLOG(_TXT("\t%d"),rElement[i]);
                 if(((i+1)%rElement.Height())==0) dmException::Trace(_TXT("\n"));
             }
             if(i<rElement.Size()) 
               dmTRACE(_TXT("WARNING INCOMPLETE ELEMENT !!!!!!!!!!"));
           #endif // _DEBUG      
           pCurrent->family.Add(rElement);
         }
       }
     } nState = eKFAMILY; break;
  }
}
//---------------------------------------------------
bool dmKernelsLoader::Create( dmKernelsSet& _set )
{
  pSet = &_set;
  _parser.Play();
  return !pSet->Empty();
}
//---------------------------------------------------
