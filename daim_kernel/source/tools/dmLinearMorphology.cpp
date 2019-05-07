#define dmUseKernelTemplates
#include "daim_kernel.h"
#include "tools/dmLinearMorphology.h"

#include <algorithm>

//-------------------------------------------------------
dmLinearStructuringElement::dmLinearStructuringElement() 
: dmKernelDescription() {}
//-------------------------------------------------------
dmLinearStructuringElement::dmLinearStructuringElement(const dmLinearStructuringElement& _kernel)
: dmKernelDescription(_kernel)
{}
//-------------------------------------------------------
dmLinearStructuringElement::dmLinearStructuringElement(size_t _or,size_t _width,const int* _pdata) 
: dmKernelDescription(_or,0,_width,1,_pdata)
{}
//-----------------------------------------------------------------	     
dmLinearStructuringElement& dmLinearStructuringElement::operator=(const dmLinearStructuringElement& _ker)
{
  dmKernelDescription::operator=(_ker);
  return *this;
}
//-----------------------------------------------------------------	     
void dmLinearStructuringElement::Complement()
{
  for(size_t i=0;i<Size();++i)
    __data[i] = -__data[i]; 
}
//-----------------------------------------------------------------	     
void dmLinearStructuringElement::Transpose()
{
  SetOrigin(Width()-Ox()-1,0);
  std::reverse(begin(),end()) ;
}
//-----------------------------------------------------------------	     
size_t dmLinearStructuringElement::GetLinearMask( const size_t _w ) const
{
  dmRect r(0,0,_w,1);
  return GetMask(r).Width();
}
//-----------------------------------------------------------------	     
size_t dmLinearStructuringElement::GetLinearRect( const size_t _w ) const
{
  dmRect r(0,0,_w,1);
  return GetRect(r).Width();
}
//-----------------------------------------------------------------	     
