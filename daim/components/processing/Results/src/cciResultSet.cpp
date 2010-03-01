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

/* $Id: cciResultSet.cpp 31 janv. 2010 22:00:51 */

#define dmUseUtilitiesExtra
#include "cciCOMPtr.h"
#include "cciIResultSet.h"
#include "cciIStringIterator.h"
#include "cciIStringEnumerator.h"

#include "cciResultColumn.h"

#include <map>

typedef std::map<dmCString,cci_Ptr<cciResultColumn> > dmStoreDict;


/* Header file */
class cciResultSet : public cciIResultSet
{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IRESULTSET

  cciResultSet();

private:
  ~cciResultSet();

protected:
  dmStoreDict mCols;
  
  cciResultColumn* CreateColumn( const dmCString& );
};

/* Implementation file */
CCI_IMPL_ISUPPORTS1(cciResultSet, cciIResultSet)

cciResultSet::cciResultSet()
{
  /* member initializers and constructor code */
}

cciResultSet::~cciResultSet()
{
  /* destructor code */
}

cciResultColumn*
cciResultSet::CreateColumn( const dmCString& column )
{
  dmStoreDict::iterator it = mCols.find(column);
  if(it==mCols.end()) {
    cciResultColumn* col = new cciResultColumn();
    if(col) {
       mCols[column] = col;
       return col;
    }
  } else
    return (*it).second.get();
  
  return dm_null;
}

/* [noscript,notxpcom] unsigned long getCol (in string column, out doublePtr data); */
CCI_IMETHODIMP_(dm_uint32) cciResultSet::GetCol(const char * column, double **data CCI_OUTPARAM)
{
  if(!column)
      return dm_null;
  
  dmStoreDict::iterator it = mCols.find(dmCString(column));
  if(it != mCols.end()) {
    cciResultColumn* col = (*it).second.get();
    if(data)
      *data = col->Data();
    
    return col->Size();
  }
  return 0;
}


/* [noscript,notxpcom] doublePtr createCol (in string column, in unsigned long length); */
CCI_IMETHODIMP_(double *) cciResultSet::CreateCol(const char * column, dm_uint32 length)
{
  if(column)
  {
    cciResultColumn* col = CreateColumn(dmCString(column));
    if(col)
       return col->GetNewData(length); 
  }
  return dm_null;
}


/* void removeCol (in string column); */
CCI_IMETHODIMP cciResultSet::RemoveCol(const char * column)
{
  CCI_ENSURE_ARG_POINTER(column);
  
  dmStoreDict::iterator it = mCols.find(dmCString(column));
  if(it!=mCols.end()) 
  {
    cciResultColumn* col = dm_null;
    (*it).second.swap(col);
    
    mCols.erase(it);
    
    CCI_RELEASE(col);
    return CCI_OK;
  }
  return CCI_ERROR_FAILURE;
}

/* cciIResultColumn getColumn (in string column, [optional] in boolean create); */
CCI_IMETHODIMP cciResultSet::GetColumn(const char * column, dm_bool create, cciIResultColumn * *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(column);
  
  cci_result rv;
  cciResultColumn* col = dm_null;
  
  if(create) 
  {
    col = CreateColumn(dmCString(column));
    CCI_ENSURE_TRUE(col,CCI_ERROR_OUT_OF_MEMORY);
    rv = CCI_OK;
  }
  else
  {
    dmStoreDict::iterator it = mCols.find(dmCString(column));
    if(it!=mCols.end()) {
      col = (*it).second.get();  
      rv = CCI_OK;
    }
    rv = CCI_ERROR_NOT_AVAILABLE;
  }
  
  CCI_IF_ADDREF(*_retval = col);
  return rv;
}


/* void setColumn (in string name, in cciIResultColumn column); */
CCI_IMETHODIMP cciResultSet::SetColumn(const char * name, cciIResultColumn *column)
{
  CCI_ENSURE_ARG_POINTER(name);
  CCI_ENSURE_ARG_POINTER(column);
  
  if(name[0]==L'\0') 
     return CCI_ERROR_INVALID_ARG;
  
  cci_Ptr<cciResultColumn> col(do_QueryInterface(column));
  if(!col)
     return CCI_ERROR_INVALID_ARG;
  
  mCols[dmCString(name)] = col;
  return CCI_OK;
}

class cciColumnIterator : public cciIUTF8StringIterator,
                          public cciIUTF8StringEnumerator,
                          public cciIStringEnumerator

{
public:
  CCI_DECL_ISUPPORTS
  CCI_DECL_IUTF8STRINGITERATOR

  // have to declare cciIStringEnumerator manually, because of
  // overlapping method names
  CCI_IMETHOD GetNext(dmAString& );
  CCI_IMETHOD GetNext(dmACString&);
  
  cciColumnIterator(const dmStoreDict& dict, cciISupports* owner)
  : mFirst(dict.begin())
  , mLast(dict.end())
  , mOwner(owner)
  {
  }

  dmStoreDict::const_iterator mFirst;
  dmStoreDict::const_iterator mLast;
  cci_Ptr<cciISupports> mOwner;
  
  const dmCString& GetNext() { return (*mFirst++).first; }

private:
  ~cciColumnIterator() {}
};

CCI_IMPL_ISUPPORTS3(cciColumnIterator,
                    cciIUTF8StringIterator,
                    cciIUTF8StringEnumerator,
                    cciIStringEnumerator)

CCI_IMETHODIMP_(dm_bool)
cciColumnIterator::HasMore()
{
  return (mFirst != mLast);
}

/* void getNext ([shared, retval] out string value); */
CCI_IMETHODIMP 
cciColumnIterator::GetNext(const char * *value CCI_OUTPARAM)
{
  if(mFirst == mLast)
     return CCI_ERROR_FAILURE;

  *value = GetNext().get();
  return CCI_OK;
}


CCI_IMETHODIMP
cciColumnIterator::GetNext(dmACString& _retval)
{
  if(mFirst == mLast)
     return CCI_ERROR_FAILURE;

  _retval.Assign( GetNext() );
  return CCI_OK;
}

CCI_IMETHODIMP
cciColumnIterator::GetNext(dmAString& _retval)
{
  if(mFirst == mLast)
     return CCI_ERROR_FAILURE;

  _retval.Assign( GetNext() );
  return CCI_OK;
}


/* cciIUTF8StringIterator enumerateColumns (); */
CCI_IMETHODIMP cciResultSet::EnumerateColumns(cciIUTF8StringIterator * *_retval CCI_OUTPARAM)
{
  *_retval = new cciColumnIterator(mCols,CCI_ISUPPORTS_CAST(cciIResultSet*,this));
  CCI_ADDREF(*_retval);
  return CCI_OK;

}

/* readonly attribute unsigned long numcols; */
CCI_IMETHODIMP cciResultSet::GetNumcols(dm_uint32 *aNumcols)
{
  *aNumcols = mCols.size();
  return CCI_OK;
}

/* void resizeCol (in string column, in unsigned long newsize, [optional] in double padding); */
CCI_IMETHODIMP cciResultSet::ResizeCol(const char * column, dm_uint32 newsize, dm_double padding)
{
  CCI_ENSURE_ARG_POINTER(column);
  
  cciResultColumn* col = CreateColumn(dmCString(column));
  if(col)
     return col->Resize(newsize,padding); 
  
  return CCI_ERROR_OUT_OF_MEMORY;
}

/* void deleteRow (in unsigned long row); */
CCI_IMETHODIMP cciResultSet::DeleteRow(dm_uint32 row)
{
  dmStoreDict::iterator it   = mCols.begin();
  dmStoreDict::iterator last = mCols.end();

  for(;it!=last;++it)
    (*it).second->DeleteRow(row);
  
  return CCI_OK;
}

/* double getValue (in string column, in unsigned long row); */
CCI_IMETHODIMP cciResultSet::GetValue(const char * column, dm_uint32 row, dm_double *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(column);
  
  dmStoreDict::iterator it = mCols.find(dmCString(column));
  if(it!=mCols.end() && row < (*it).second->Size()) 
  {
    *_retval = (*it).second->Data()[row];
    return CCI_OK;
  }
  return CCI_ERROR_FAILURE;
}

/* void setValue (in string column, in unsigned long row, in double value); */
CCI_IMETHODIMP cciResultSet::SetValue(const char * column, dm_uint32 row, dm_double value)
{
  CCI_ENSURE_ARG_POINTER(column);
  
  dmStoreDict::iterator it = mCols.find(dmCString(column));
  if(it!=mCols.end() && row < (*it).second->Size()) 
  {
    (*it).second->Data()[row] = value;
    return CCI_OK;
  }
  return CCI_ERROR_FAILURE;
}


/* void clearCol (in string column); */
CCI_IMETHODIMP cciResultSet::ClearCol(const char * column)
{
  dmStoreDict::iterator it = mCols.find(dmCString(column));
  if(it!=mCols.end()) 
  {
    (*it).second->Clear();
    return CCI_OK;
  }
  return CCI_ERROR_FAILURE;
}

/* void reserve (in string column, in unsigned long capacity); */
CCI_IMETHODIMP cciResultSet::Reserve(const char * column, dm_uint32 capacity)
{
  CCI_ENSURE_ARG_POINTER(column);
  CCI_ENSURE_ARG_MIN(capacity,1);
  
  cciResultColumn* col = CreateColumn(dmCString(column));
  if(!col)
    return CCI_ERROR_OUT_OF_MEMORY;
  
  return col->Reserve(capacity);
}

/* unsigned long length (in string column); */
CCI_IMETHODIMP cciResultSet::Length(const char * column, dm_uint32 *_retval CCI_OUTPARAM)
{
  CCI_ENSURE_ARG_POINTER(column);
  
  dmStoreDict::iterator it = mCols.find(dmCString(column));
  if(it!=mCols.end()) {
     *_retval = (*it).second->Size();
     return CCI_OK;
  }
  return CCI_ERROR_FAILURE;
}

//=====================================
// Module
//=====================================

#include "cciIGenericFactory.h"

CCI_GENERIC_FACTORY_CONSTRUCTOR(cciResultSet)

static const cciModuleComponentInfo components[] = {
    {  CCI_RESULTSET_CLASSNAME,
       CCI_RESULTSET_CID,
       CCI_RESULTSET_CONTRACTID,
       cciResultSetConstructor
    },
};

CCI_IMPL_GETMODULE(ResultSetModule, components)


