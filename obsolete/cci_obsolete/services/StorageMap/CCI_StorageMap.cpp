
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

/* $ChangeLog  v1.2, 26 oct. 2006 02:06:56 , David
 *
 * Bug fixed, pushValue was not setting the correct size 
 * on column creation 
 */ 
 
 /* $ChangeLog  v1.3, 12 nov. 2006 16:10:18 , David
 *
 * Bug fixed, bad row indices in method Copy()
 */ 

/* $ChangeLog  v1.4, 7 déc. 06 01:32:41 , David
 *
 * Bug fixed, in method Export, no tabulation was output
 * when a row was not valid for a column.
 */

#define dmUseCCI
#define dmUseUtilitiesExtra
#include "daim_utilities.h"

#include "daim_modules/CCI_Notify.h"
#include "daim_modules/CCI_StringList.h"
#include "daim_modules/CCI_PropertyList.h"
#include "daim_modules/CCI_StorageMap.h"

//------------------------------------------------------------------------
#define CCIC_NAME  CCIC_StorageMap
#define CCIC_SUPER CCIC_Notify
#define CCIC_SUPER_VERSION CCI_Notify_Version
#define AUTHOR "David Marteau"
#define CCI_VERSION   CCI_StorageMap_Version
#define CCI_REVISION  4
#define CCI_IDSTRING  _MAKE_CCI_IDSTRING(CCIC_NAME,AUTHOR,_TXT("(C) 2005"))
//------------------------------------------------------------------------
#define SET_ERROR( ob,me,txt ) CCI_SetError(ECCI_ERROR,me,CCI_ObjectClass(ob),ob,txt)
//--------------------------------------------------------------------
#include "daim_modules/CCI_Base.h"
#include "daim_modules/CCI_Interface.h"

#include "daim/daim_base.h"
#include "daim/daim_interfaces.h"

#include "dmMatrixUtils.h"

#include <stdio.h>
#include <errno.h>
#include <numeric>
#include <map>

using namespace std;
//--------------------------------------------------------------------
struct __Storage
{
  dmString            name;
  dmVector<dm_real>   data;  
  __Storage(const char* _name ) : name(_name) {}
};
//--------------------------------------------------------------------
typedef map<dmString,__Storage*> st_map_type;

CCI_DECL_DATA( StorageMap )
{
  st_map_type       m_StoreMap;
  dmString          m_HeaderCache;
  dm_bool           m_Modified;

} mData;
//--------------------------------------------------------------------
// Helpers
//--------------------------------------------------------------------
static bool StorageMap_ResolveURI( CCI_INSTANCE_PTR(StorageMap) _This,
                                   dmString& _uri, 
                                   dmString& _protocol, 
                                   dmString& _path 
                                 )
{
  // Find protocol
  size_t pos = _uri.Find(_TXT("://"));
  if(pos!=dmString::npos) {
    _protocol = _uri.SubStr(0,pos);
    if(!_protocol.Empty()) 
    {      
      _path.Assign(_uri,pos+3,dmString::npos);
      return true;
    }
  }
  return false;
}
//--------------------------------------------------------------------
static __Storage* StorageMap_CreateCol( CCI_INSTANCE_PTR(StorageMap) _This,
                                        const char_t* column )
{
  st_map_type::iterator it = _This->m_StoreMap.find(column);
  if(it==_This->m_StoreMap.end()) {
     __Storage* pStore = new __Storage(column);
    _This->m_StoreMap[column] = pStore; 
    return pStore;
  } else
    return (*it).second;
}
//--------------------------------------------------------------------
static __Storage* StorageMap_FindCol( CCI_INSTANCE_PTR(StorageMap) _This,
                                      const char_t* column )
{
  st_map_type::iterator it = _This->m_StoreMap.find(column);
  if(it!=_This->m_StoreMap.end())
     return (*it).second;
  
  return NULL;
}
//--------------------------------------------------------------------
// NEW
//--------------------------------------------------------------------
static CCI_CONSTRUCTOR( StorageMap )
{
  CCI_GENERIC_CONSTRUCT(StorageMap)

  _This->m_Modified = dm_false;
 
  CCI_RETURN_THIS()
}
//--------------------------------------------------------------------
// DISPOSE
//---------------------------------------------------------------------
static CCI_DESTRUCTOR( StorageMap )
{
  CCI_INSTANCE(StorageMap)

  st_map_type::iterator it   = _This->m_StoreMap.begin();
  st_map_type::iterator last = _This->m_StoreMap.end();
  for(;it!=last;++it)  delete (*it).second;

  CCI_DESTROY_DATA()     
  CCI_DESTROY_SUPER()
}
//---------------------------------------------------------------------
// GET
//---------------------------------------------------------------------
CCI_IMPL_GETTER( StorageMap )
  CCI_ATTR_GET(StorageMap,Count   , CCIA_RETVAL = _This->m_StoreMap.size() )
  CCI_ATTR_GET(StorageMap,Modified, CCIA_RETVAL = _This->m_Modified        )
CCI_END_GETTER()
//---------------------------------------------------------------------
// SET
//---------------------------------------------------------------------
CCI_IMPL_SETTER( StorageMap )
  CCI_ATTR_SET(StorageMap,Modified, _This->m_Modified = CCIA_DATA  )
  CCIA_DISCARD(StorageMap,Count     )
CCI_END_SETTER()
//---------------------------------------------------------------------
// Method     : GetCol
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StorageMap , GetCol )
{
  CCI_INSTANCE(StorageMap)

  __Storage* pStore = NULL;

  if(!EMPTY_STRING(CCI_P(name))) 
  {
    st_map_type::iterator it = _This->m_StoreMap.find(CCI_P(name));
    if(it!=_This->m_StoreMap.end()) 
         pStore = (*it).second;
  } else
    CCI_RETURN_INVALID_PARAM()
    
  // Size is specified 
  // pStore does not exists but <name> and <size>  
  // are specified => create a new entry

  if(!pStore) 
  {
    if(CCI_P(size)>0) 
    {
      pStore = new __Storage(CCI_P(name));
      _This->m_StoreMap[CCI_P(name)] = pStore;

      // Create new name if name was not specified
      if(pStore->name.Empty())
         pStore->name.Format(dmString::npos,"C%d",_This->m_StoreMap.size());

    } else {
      CCI_RETVAL_P(size)  = 0;
      CCI_RETVAL_P(store) = NULL;
      CCI_RETURN_OK()
    }
  }

  if(pStore) 
  {
    // Resize column
    if(CCI_P(size)>0) 
    {
      pStore->data.Resize(CCI_P(size)+1);
      pStore->data[0] = CCI_P(size);
    }

    CCI_RETVAL_P(size)  = static_cast<dm_uint>(pStore->data[0]);
    CCI_RETVAL_P(store) = pStore->data.Begin();
    CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method     : Clear
//---------------------------------------------------------------------
static CCI_IMPL_METHOD_NP( StorageMap , Clear )
{
  CCI_INSTANCE(StorageMap)
  st_map_type::iterator it   = _This->m_StoreMap.begin();
  st_map_type::iterator last = _This->m_StoreMap.end();

  for(;it!=last;++it)  
      delete (*it).second;

  _This->m_StoreMap.clear();
  
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method     : Remove 
// Description: Remove Column
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StorageMap , Remove )
{
  CCI_INSTANCE(StorageMap)

  if(CCI_P(name)) 
  {
    st_map_type::iterator it = _This->m_StoreMap.find(CCI_P(name));
    if(it!=_This->m_StoreMap.end()) 
    {
      delete (*it).second;
      _This->m_StoreMap.erase(it);

      CCI_RETURN_OK()
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method     : GetRow
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StorageMap , GetRow )
{
  CCI_INSTANCE(StorageMap)

  dmIPropertyList _IPropList;
  if(_IPropList.QueryInterface(CCI_P(propertylist)))
  {
    if(!EMPTY_STRING(CCI_P(fmt))) CCI_P(fmt) = _TXT("%g");

    dm_param  key = CCI_P(key);
    dm_uint   row = CCI_P(row);    
    dmString  value;

    st_map_type::iterator it   = _This->m_StoreMap.begin();
    st_map_type::iterator last = _This->m_StoreMap.end();

    __Storage* pStore = NULL;

    for(;it!=last;++it) {
      pStore = (*it).second;
      if(row < pStore->data.Size()) {
        value.Format(dmString::npos,CCI_P(fmt),pStore->data[row]);
        _IPropList.SetProperty(key,NULL,pStore->name.CStr(),value.CStr());
      }
    }

    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method     : SetRow
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StorageMap , SetRow )
{
  CCI_INSTANCE(StorageMap)

  dmIPropertyList _IPropList;
  if(_IPropList.QueryInterface(CCI_P(propertylist)))
  {
    dm_param  key = CCI_P(key);
    dm_uint   row = CCI_P(row);    

    CONST char_t* value;

    st_map_type::iterator it   = _This->m_StoreMap.begin();
    st_map_type::iterator last = _This->m_StoreMap.end();

    __Storage* pStore = NULL;

    for(;it!=last;++it) {
      pStore = (*it).second;      
      if(row < pStore->data.Size()) {
        value = _IPropList.GetPropertyStr(key,NULL,pStore->name.CStr());
        if(value) 
          pStore->data[row] = atof( value );
      }
    }

    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method     : GetCols
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StorageMap , GetCols )
{
  CCI_INSTANCE(StorageMap)
  if(!_This->m_StoreMap.empty()) 
  {
    dmIStringList _IStrings;
    if(_IStrings.QueryInterface(CCI_P(stringlist)))
    {  
      st_map_type::iterator it   = _This->m_StoreMap.begin();
      st_map_type::iterator last = _This->m_StoreMap.end();
      for(;it!=last;++it) 
        _IStrings.AddTailA( (*it).first );

      CCI_RETURN_OK()
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method     : AddRow
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StorageMap , AddRow )
{
  CCI_INSTANCE(StorageMap)

  if(_This->m_StoreMap.empty()) { // Create "0" entry
    __Storage* pStore = NULL;
    pStore = new __Storage("C0");
    pStore->data.Resize(1,0.0);
    pStore->data[0] = 0;
    _This->m_StoreMap[pStore->name] = pStore;
  }
  
  dm_uint sz = CCI_P(count);

  if(sz==0) sz=1; // Set size to 1;

  st_map_type& store  = _This->m_StoreMap;
  st_map_type::iterator it   = store.begin();
  st_map_type::iterator last = store.end();
  for(__Storage* pStore = NULL;it!=last;++it)
  {
    pStore = (*it).second;
    pStore->data.Resize(static_cast<size_t>(pStore->data[0])+sz+1);
    pStore->data[0] += sz;
  }

  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method     : DeleteRow 
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StorageMap , DeleteRow )
{
  CCI_INSTANCE(StorageMap)
  if(!_This->m_StoreMap.empty()) 
  {
    dm_uint row = CCI_P(row);
    if(row <= 0) 
       CCI_RETURN_FAIL(); 
 
    st_map_type::iterator it   = _This->m_StoreMap.begin();
    st_map_type::iterator last = _This->m_StoreMap.end();

    for(__Storage* pStore = NULL;it!=last;++it)
    {
      pStore = (*it).second;
      if(row < pStore->data.Size()) {
         pStore->data.Erase(pStore->data.Begin()+row);
         --pStore->data[0];
      }
    }

    CCI_RETURN_OK()
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method     : GetValue
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StorageMap , GetValue )
{
  CCI_INSTANCE(StorageMap)   
  
  st_map_type::iterator it = _This->m_StoreMap.find(CCI_P(column));
  if(it!=_This->m_StoreMap.end()) 
  {
    if(CCI_P(row) < (*it).second->data.Size()) {
       CCI_RETVAL_P(value) = (*it).second->data[CCI_P(row)];
       CCI_RETURN_OK()
    }
  }
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method     : SetValue
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StorageMap , SetValue )
{
  CCI_INSTANCE(StorageMap)   
  
  st_map_type::iterator it = _This->m_StoreMap.find(CCI_P(column));
  if(it!=_This->m_StoreMap.end()) 
  {
    if(CCI_P(row) < (*it).second->data.Size()) {
      (*it).second->data[CCI_P(row)] = CCI_P(value);
      CCI_RETURN_OK()
    }
  }
  CCI_RETURN_OK()
}
//---------------------------------------------------------------------
// Method     : Reserve
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StorageMap , Reserve )
{
  CCI_INSTANCE(StorageMap)   
  
  if(CCI_P(capacity) > 0)
  {
    __Storage* pStore = StorageMap_CreateCol(_This,CCI_P(column));
    if(pStore) 
    {
       pStore->data.Reserve(CCI_P(capacity));
       if(pStore->data.Size()==0) {
          pStore->data.Resize(1);
          pStore->data[0] = 0;
       }
      CCI_RETURN_OK()
    }
  }
  CCI_RETURN_FAIL();
}
//---------------------------------------------------------------------
// Method     : Export 
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StorageMap , Export )
{
  CCI_INSTANCE(StorageMap)
    
  if(!EMPTY_STRING(CCI_P(url)) && !_This->m_StoreMap.empty()) 
  {    
    dmString uri(CCI_P(url));
    dmString prot,path;
  
    if(!StorageMap_ResolveURI(_This,uri,prot,path)) {
       CCI_SET_ERROR_MSG(_TXT("Malformed uri"));
       CCI_RETURN_FAIL();
    }
  
    // Get the wanted columns
    
    dmIStringList _Strings;
    if(_Strings.QueryInterface(CCI_P(stringlist)))
    {
      std::vector<__Storage*> _storeList;
      _storeList.reserve(_Strings.Count());

      dmString _Header;
      dm_param pos = dm_null;
      
      // Collect all specified rows;
      while( (pos=_Strings.GetNext(pos)) )
      { 
        if(_Strings.GetStringA(pos,_Header) && !_Header.Empty()) 
        {
          st_map_type::iterator it = _This->m_StoreMap.find(_Header.CStr());
          if(it!=_This->m_StoreMap.end()) 
             _storeList.push_back((*it).second);
        }
      }

      if(!_storeList.empty()) 
      {
        FILE* _file = _tfopen(path.CStr(),_TXT("w"));
        if(_file!=NULL) 
        {
          dmStrStream _Str;

          dm_uint i,row,maxrows = 0;
          __Storage* pStore;
         
          _Str << _TXT("#index\t");

          // Output header
          for(i=0;i<_storeList.size();++i) 
          {
            pStore = _storeList[i];
            if(maxrows < pStore->data[0] )
               maxrows = static_cast<dm_uint>(pStore->data[0]);

            _Str << pStore->name  << _TXT("\t");
          }

          _Str << _TXT("\n"); 
          _fputts(_Str.CStr(),_file);

          // Output data
          for(row=1;row<=maxrows;++row)
          {   
            _Str.Clear();
            _Str << row <<  _TXT("\t");
            for(i=0;i<_storeList.size();++i) 
            {  
               pStore = _storeList[i];
               if(row <= pStore->data[0]) {
                  _Str << pStore->data[row]; // << _TXT("\t");
               } 

               _Str << _TXT("\t");
               
            }
            _Str << _TXT("\n");
            _fputts(_Str.CStr(),_file);
          }
          
          fclose(_file);      
          CCI_RETURN_OK()
        } 
        else
        {
          int err = errno;
          if(err!=0) { 
            dmString _ErrMsg = dmString::FormatString(dmString::npos,_TXT("Cannot open %s : %s "),
                                                      path.CStr(),strerror( err ));
            CCI_SET_ERROR_MSG( _ErrMsg.CStr() );     
            CCI_RETURN_IOERROR()
          }
        }
      }
    }
  }

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method     : ResizeCol
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StorageMap , ResizeCol )
{
  CCI_INSTANCE(StorageMap)   
  
  if(!EMPTY_STRING(CCI_P(column)))
  {
    __Storage* pStore = StorageMap_CreateCol(_This,CCI_P(column));
    if(CCI_P(newsize)>=0) 
    {
       pStore->data.Resize(CCI_P(newsize)+1);
       pStore->data[0] = CCI_P(newsize);
    }
    CCI_RETURN_OK()
  }
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method     : SetCols
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StorageMap , SetCols )
{
  CCI_INSTANCE(StorageMap)   

  dmIStringList _Strings;
  if(_Strings.QueryInterface(CCI_P(stringlist)))
  {
    dmString _Column;
    dm_param pos     = dm_null;
    dm_uint  minsize = CCI_P(minsize);
      
    while( (pos=_Strings.GetNext(pos)) )
    { 
      if(_Strings.GetStringA(pos,_Column) && !_Column.Empty()) 
      {
        __Storage* pStore = StorageMap_CreateCol(_This,_Column.CStr());
        if(minsize >= pStore->data.Size()) { // This will work for size==0
           pStore->data.Resize(minsize+1);
           pStore->data[0] = minsize;
        }
      }
    }
    CCI_RETURN_OK()
  }
  
  CCI_RETURN_INVALID_PARAM()
}
//---------------------------------------------------------------------
// Method     : PushValue
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StorageMap , PushValue )
{
  CCI_INSTANCE(StorageMap)   
  
  __Storage* pStore = StorageMap_CreateCol(_This,CCI_P(column));
  if(pStore) {

    if(pStore->data.Size()==0) {
       pStore->data.Resize(1);
       pStore->data[0] = 0;
     }
    
     pStore->data.Push_Back(CCI_P(value));
     pStore->data[0] = pStore->data.Size()-1;
     CCI_RETURN_OK()
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method     : Copy
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StorageMap , Copy )
{
  CCI_INSTANCE(StorageMap)   
    
  dmIStorageMap _IStorage;
  if(_IStorage.QueryInterface(CCI_P(from))) 
  {
    dm_uint istart = CCI_P(start);
    dm_uint iend   = CCI_P(end);

    if(istart == ~0U) 
       istart = 1;

    if(istart <= 0 || istart > iend)
       CCI_RETURN_INVALID_PARAM()

    dm_real* src = _IStorage.GetCol(CCI_P(srccol),0);

    if(src) 
    {
      __Storage* _Dst = StorageMap_CreateCol(_This,CCI_P(dstcol));
    
      if(_Dst) 
      { 
        dm_uint nrows = static_cast<dm_uint>(src[0]);
        if(istart <= nrows) 
        {
          if(iend > nrows) 
             iend = nrows;
          
          ++iend;
          
          dm_uint length = iend - istart;
          _Dst->data.Resize(length +1);
          _Dst->data[0] = length;
        
          std::copy(src+istart,src+iend,&_Dst->data[1]);
        
          CCI_RETURN_OK()      
        }
      }
    }
  } else
    CCI_RETURN_INVALID_PARAM()

  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Method     : GetStat
//---------------------------------------------------------------------
static CCI_IMPL_METHOD( StorageMap , GetStat )
{
  CCI_INSTANCE(StorageMap)   

  const char_t* type = CCI_P(type);

  st_map_type::iterator store = _This->m_StoreMap.find(CCI_P(column));
  
  if(store!=_This->m_StoreMap.end()) 
  {
     __Storage* _Src = (*store).second;
     
    dmVector<dm_real>& _Data = _Src->data;
    
    if(EQUAL_STRING(type,"len")) {
      CCI_RETVAL_P(value) = _Data[0];
      CCI_RETURN_OK()
    }
    
    // non-zero length required;
    
    if(_Data[0] <= 0)
       CCI_RETURN_FAIL();

    if(EQUAL_STRING(type,"mean")) {
      dm_real sum   = std::accumulate(_Data.Begin()+1,_Data.End(),0.0);
      dm_real count = _Data[0];
      CCI_RETVAL_P(value) = sum / count;
      CCI_RETURN_OK()
    }

    if(EQUAL_STRING(type,"min")) {
      CCI_RETVAL_P(value) = *std::min_element(_Data.Begin()+1,_Data.End());
      CCI_RETURN_OK()    
    }

    if(EQUAL_STRING(type,"max")) {
      CCI_RETVAL_P(value) = *std::max_element(_Data.Begin()+1,_Data.End());
      CCI_RETURN_OK()
    }

    if(EQUAL_STRING(type,"sum")) {
      CCI_RETVAL_P(value) = std::accumulate(_Data.Begin()+1,_Data.End(),0.0); 
      CCI_RETURN_OK()
    }
  
    if(EQUAL_STRING(type,"var"))
    {
      dmVector<dm_real>::iterator it   = _Data.Begin()+1;
      dmVector<dm_real>::iterator last = _Data.End();
      
      dm_real m     = std::accumulate(_Data.Begin()+1,_Data.End(),0.0);
      dm_real count = _Data[0],sum2 = 0.0,val;
      
      m /= count;
      
      for(;it!=last;++it) {
        val = (*it - m);
        sum2 += val*val;
      }        
      CCI_RETVAL_P(value) = sum2/count;
      CCI_RETURN_OK()
    } 
    
    if(EQUAL_STRING(type,"rms")) {
      dmVector<dm_real>::iterator it   = _Data.Begin()+1;
      dmVector<dm_real>::iterator last = _Data.End();
      
      dm_real count = _Data[0],sum2 = 0.0,val;
      
      for(;it!=last;++it) {
        val = *it;
        sum2 += val*val;
      }        
      CCI_RETVAL_P(value) = sum2/count;
      CCI_RETURN_OK()
    } 
    
    if(EQUAL_STRING(type,"min element")) {
      dmVector<dm_real>::iterator it = std::min_element(_Data.Begin()+1,_Data.End());
      CCI_RETVAL_P(value) = it - _Data.Begin();
      CCI_RETURN_OK()    
    }

    if(EQUAL_STRING(type,"max element")) {
      dmVector<dm_real>::iterator it = std::max_element(_Data.Begin()+1,_Data.End());
      CCI_RETVAL_P(value) = it - _Data.Begin();
      CCI_RETURN_OK()
    }
  
  }
  CCI_RETURN_FAIL()
} 
//---------------------------------------------------------------------
// Method: GetInterval
//---------------------------------------------------------------------
 static CCI_IMPL_METHOD( StorageMap , GetInterval )
{
  CCI_INSTANCE(StorageMap)   

  st_map_type::iterator store = _This->m_StoreMap.find(CCI_P(column));
  if(store!=_This->m_StoreMap.end()) 
  {
    __Storage* _Src = (*store).second;
        
    if(CCI_P(start) <=0 || CCI_P(start) > _Src->data[0])
       CCI_RETURN_INVALID_PARAM()

    dm_real upper = CCI_P(upper);
    dm_real lower = CCI_P(lower);
    
    dmVector<dm_real>::iterator it = _Src->data.Begin()+CCI_P(start);
    
    if(lower > upper)
    {
      std::swap(lower,upper);
      dmVector<dm_real>::iterator last  = _Src->data.Begin();
            
      if( dm_numerical::in_range(*it,lower,upper) ) {
         for(--it;it!=last;--it) {
           if(dm_numerical::out_range(*it,lower,upper)){
              ++it;
              break;
           }
         }
      } else 
      if( dm_numerical::out_range(*it,lower,upper) ) {
         for(--it;it!=last;--it) {
           if(dm_numerical::in_range(*it,lower,upper))
              break;
         }
      } 
      
      if(it==last)
         it = _Src->data.Begin()+1;    
    }
    else
    {
      dmVector<dm_real>::iterator last = _Src->data.End();

      if( dm_numerical::in_range(*it,lower,upper) ) {
         for(++it;it!=last;++it) {
           if(dm_numerical::out_range(*it,lower,upper)) {
              --it;
              break;
           }
         }
      } else 
      if( dm_numerical::out_range(*it,lower,upper) ) {
         for(++it;it!=last;++it) {
           if(dm_numerical::in_range(*it,lower,upper))
              break;
         }
      }
    }    

    CCI_RETVAL_P(next) = it - _Src->data.Begin();
    CCI_RETURN_OK() 
  }
  
  CCI_RETURN_FAIL()
} 
//---------------------------------------------------------------------
// Method: DoArithmetics
//---------------------------------------------------------------------
 static CCI_IMPL_METHOD( StorageMap , DoArithmetics )
{
  CCI_INSTANCE(StorageMap)   

  __Storage* leftstore  = StorageMap_FindCol(_This,CCI_P(leftcol) );
  __Storage* rightstore = StorageMap_FindCol(_This,CCI_P(rightcol));

  if(leftstore && rightstore)
  {
    __Storage* deststore = StorageMap_CreateCol(_This,CCI_P(destcol));
    
    dm_uint count = static_cast<dm_uint>(std::min(leftstore->data[0],rightstore->data[0]));
    if(count > 0)
    {
      deststore->data.Resize(count+1);
      deststore->data[0] = count;
      
      dmVector<dm_real>::iterator lit = leftstore->data.Begin()+1;   
      dmVector<dm_real>::iterator rit = rightstore->data.Begin()+1;     
      dmVector<dm_real>::iterator dit = deststore->data.Begin()+1;      
      
      const char_t* op = CCI_P(op);
      
      if(EQUAL_STRING(op,"*")) { 
        for(dm_uint i=0;i<count;++i) 
            (*dit++) = (*lit++) * (*rit++);
        CCI_RETURN_OK()
      }
    
      if(EQUAL_STRING(op,"+")) { 
        for(dm_uint i=0;i<count;++i) 
            (*dit++) = (*lit++) * (*rit++);
        CCI_RETURN_OK()
      }
      
      if(EQUAL_STRING(op,"/")) { 
        for(dm_uint i=0;i<count;++i) 
            (*dit++) = (*lit++) / (*rit++);
        CCI_RETURN_OK()
      }

      if(EQUAL_STRING(op,"-")) { 
        for(dm_uint i=0;i<count;++i) 
            (*dit++) = (*lit++) - (*rit++);
        CCI_RETURN_OK()
      }
      
    } else
        CCI_RETURN_OK( )// Nothing to do
  }
  CCI_RETURN_FAIL()
}
//---------------------------------------------------------------------
// Class Dispatcher
//---------------------------------------------------------------------
CCI_BEGIN_DISPATCH_MAP( StorageMap )
  CCI_REGISTER_METHOD_PP(StorageMap,GetCol       )
  CCI_REGISTER_METHOD_NP(StorageMap,Clear        )
  CCI_REGISTER_METHOD_PP(StorageMap,Remove       )
  CCI_REGISTER_METHOD_PP(StorageMap,SetRow       )
  CCI_REGISTER_METHOD_PP(StorageMap,GetRow       )
  CCI_REGISTER_METHOD_PP(StorageMap,GetCols      )
  CCI_REGISTER_METHOD_PP(StorageMap,AddRow       )
  CCI_REGISTER_METHOD_PP(StorageMap,DeleteRow    )
  CCI_REGISTER_METHOD_PP(StorageMap,GetValue     )
  CCI_REGISTER_METHOD_PP(StorageMap,SetValue     )
  CCI_REGISTER_METHOD_PP(StorageMap,Export       )
  CCI_REGISTER_METHOD_PP(StorageMap,ResizeCol    )
  CCI_REGISTER_METHOD_PP(StorageMap,SetCols      )
  CCI_REGISTER_METHOD_PP(StorageMap,PushValue    )
  CCI_REGISTER_METHOD_PP(StorageMap,Copy         )
  CCI_REGISTER_METHOD_PP(StorageMap,GetStat      )
  CCI_REGISTER_METHOD_PP(StorageMap,GetInterval  )
  CCI_REGISTER_METHOD_PP(StorageMap,Reserve      )
  CCI_REGISTER_METHOD_PP(StorageMap,DoArithmetics)
CCI_NEXT_DISPATCH_MAP()
//---------------------------------------------------------------------
// call to MakeClass(...)
//---------------------------------------------------------------------
CCI_ENTRY_POINT( StorageMap )
{
  //DM_INIT_API()
  CCI_INVOKE_FACTORY(StorageMap)
}
//---------------------------------------------------------------------
