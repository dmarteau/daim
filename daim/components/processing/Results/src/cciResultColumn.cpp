/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code 3liz code.
 *
 * The Initial Developer of the Original Code is 3Liz SARL.
 * Portions created by the Initial Developer are Copyright (C) 2010
 * 3Liz SARL. All Rights Reserved.
 *
 * Contributor(s):
 *   David Marteau  (original author)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

/* $Id: cciResultColumn.cpp 2 févr. 2010 22:36:09 */

#include "cciResultColumn.h"
#include "dmMatrixUtils.h"


/* Implementation file */
CCI_IMPL_ISUPPORTS2(cciResultColumn, cciResultColumn, cciIResultColumn)

cciResultColumn::cciResultColumn()
{
  /* member initializers and constructor code */
}

/* [noscript,notxpcom] unsigned long getData (out doublePtr data); */
CCI_IMETHODIMP_(dm_uint32) cciResultColumn::GetData( double **data )
{
  if(data)
    *data = Data();
  
  return Size();
}

/* [noscript,notxpcom] doublePtr getNewData (in unsigned long desiredSize); */
CCI_IMETHODIMP_(double *) cciResultColumn::GetNewData(dm_uint32 desiredSize)
{
  mData.Resize(desiredSize);
  return Data();
}

/* void setData ([array, size_is (count)] in double data, in unsigned long count); */
CCI_IMETHODIMP cciResultColumn::SetData(double *data, dm_uint32 count)
{
  CCI_ENSURE_ARG_POINTER(data);
  
  if(count) {
    mData.Resize(count);  
    std::copy(data,data+count,Begin());
  }
  
  return CCI_OK;
}

/* void fill (in double value); */
CCI_IMETHODIMP cciResultColumn::Fill(double value)
{
  std::fill(Begin(),End(),value);
  return CCI_OK;
}

/* void clear (); */
CCI_IMETHODIMP cciResultColumn::Clear()
{
  mData.Clear();
  return CCI_OK;
}

/* void reserve (in unsigned long capacity); */
CCI_IMETHODIMP cciResultColumn::Reserve(dm_uint32 capacity)
{
  mData.Reserve(capacity);
  return CCI_OK;
}

/* void resize (in unsigned long newsize, [optional] in double padding); */
CCI_IMETHODIMP cciResultColumn::Resize(dm_uint32 newsize, double padding)
{
  mData.Resize(newsize,padding);
  return CCI_OK;
}

/* readonly attribute unsigned long length; */
CCI_IMETHODIMP cciResultColumn::GetLength(dm_uint32 *aLength)
{
  *aLength = Size();
  return CCI_OK;
}

/* double getValue (in unsigned long row); */
CCI_IMETHODIMP cciResultColumn::GetValue(dm_uint32 row, double *_retval)
{
  if(row < Size()) 
  {
    *_retval = mData[row];
    return CCI_OK;
  }
  return CCI_ERROR_FAILURE;
}

/* void setValue (in unsigned long row, in double value); */
CCI_IMETHODIMP cciResultColumn::SetValue(dm_uint32 row, double value)
{
  if(row < Size()) 
  {
    mData[row] = value;
    return CCI_OK;
  }
  return CCI_ERROR_FAILURE;
}

/* void push (in double value); */
CCI_IMETHODIMP cciResultColumn::Push(double value)
{
  mData.Push_Back(value);
  return CCI_OK;
}

/* void deleteRow (in unsigned long row); */
CCI_IMETHODIMP cciResultColumn::DeleteRow(dm_uint32 row)
{
  if(row < mData.Size())
     mData.Erase(Begin()+row);
  
  return CCI_OK;
}

/* void copy (in cciIResultColumn from, in unsigned long srcStart, in unsigned long srcEnd, in unsigned long dstRow); */
CCI_IMETHODIMP cciResultColumn::Copy(cciIResultColumn *from, dm_uint32 srcStart, 
                                                             dm_uint32 srcEnd, 
                                                             dm_uint32 dstRow)
{
  CCI_ENSURE_ARG_POINTER(from);
  
  // Ensure that we are not copying onto ourself;
  cci_Ptr<cciResultColumn> rcol(do_QueryInterface(from));
  if(!rcol || rcol == this)
     return CCI_ERROR_INVALID_ARG;

  dm_real* src;
  dm_uint32 srcLen = from->GetData(&src);
  
  if(srcEnd > srcLen)
     srcEnd = srcLen;

  if(srcEnd <= 0 || srcStart > srcEnd)
     return CCI_ERROR_OUT_OF_RANGE; 
    
  mData.Resize(srcEnd - srcStart + dstRow);
  
  std::copy(src+srcStart,src+srcEnd,mData.Begin()+dstRow);
  return CCI_OK;
}

/* unsigned long getInterval (in unsigned long start, in double lower, in double upper); */
CCI_IMETHODIMP cciResultColumn::GetInterval(dm_uint32 start, double lower, double upper, 
                                            dm_uint32 *_retval )
{
  /** 
   * @brief Return the range of data lying in a specified interval
   * 
   *  Given the  /a start index, the method return the /a last index
   *  that define the range of consecutive data lying in the specified 
   *  interval.
   */   
  
  if(start >= Size())
     return CCI_ERROR_INVALID_ARG;
  
  iterator it = mData.Begin()+start;
  
  // Search backward
  if(lower > upper)
  {
    std::swap(lower,upper);
    iterator last = mData.Begin();
    
    // If we are 'in range' look for the last 'in range' position
    if( dm_numerical::in_range(*it,lower,upper) ) {
       for(--it;it!=last;--it) {
         if(dm_numerical::out_range(*it,lower,upper)){
            ++it;
            break;
         }
       }
    } else 
    // If we are 'out range' look for the next 'in range' position
    if( dm_numerical::out_range(*it,lower,upper) ) {
       for(--it;it!=last;--it) {
         if(dm_numerical::in_range(*it,lower,upper))
            break;
       }
    } 
    
    if(it==last)
       it = mData.Begin()+1;    
  }
  else
  {
    iterator last = mData.End();

    // If we are 'in range' look for the last 'in range' position
    if( dm_numerical::in_range(*it,lower,upper) ) {
       for(++it;it!=last;++it) {
         if(dm_numerical::out_range(*it,lower,upper)) {
            --it;
            break;
         }
       }
    } else 
    // If we are 'out range' look for the next 'in range' position
    if( dm_numerical::out_range(*it,lower,upper) ) {
       for(++it;it!=last;++it) {
         if(dm_numerical::in_range(*it,lower,upper))
            break;
       }
    }
  }    

  // Return the next search index
  *_retval = it - mData.Begin();
  return CCI_OK; 
}
