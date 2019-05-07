#ifndef CCI_StorageMap_h
#define CCI_StorageMap_h

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
// File         : CCI_StorageMap.h
// Date         : 01/2005
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

#define CCIC_StorageMap _TXT("storagemap.cci")
#include "daim_modules/CCI_Prolog.h"

#ifndef __doxygen

CCI_BEGIN_MODULE_DECL( StorageMap, Notify )
  
  CCI_MODULE_UUID( StorageMap, dc43f4c4-8ca9-4677-a317-0baa53f33e01)

  CCI_MODULE_VERSION( StorageMap, 1 ) 
 
  /* Methods   */
  CCI_DECL_METHOD_PP(StorageMap,  1,  GetCol ,
     _in  CONST char_t*          name 
     _in  dm_uint                size
     _out dm_array(dm_real,size) store
  )

  CCI_DECL_METHOD_NP(StorageMap,  2,  Clear )       
  CCI_DECL_METHOD_PP(StorageMap,  3,  Remove      , _in  CONST char_t*  name )

  CCI_DECL_METHOD_PP(StorageMap,  4,  GetRow      , 
      _in dm_cci(PropertyList) propertylist 
      _in dm_uint              key
      _in dm_uint              row 
      _in CONST char_t*        fmt
  )

  CCI_DECL_METHOD_PP(StorageMap,  5,  SetRow      ,      
      _in dm_cci(PropertyList) propertylist 
      _in dm_uint              key
      _in dm_uint              row 
  )

  CCI_DECL_METHOD_PP(StorageMap,  6,  GetCols     , _in dm_cci(StringList) stringlist )
  CCI_DECL_METHOD_PP(StorageMap,  7,  AddRow      , _in dm_uint  count  )
  CCI_DECL_METHOD_PP(StorageMap,  8,  DeleteRow   , _in dm_uint  row    )

  CCI_DECL_METHOD_PP(StorageMap,  9,  Export      ,
     _in dm_cci(StringList) stringlist 
     _in CONST char_t*      url
  )
 
  CCI_DECL_METHOD_PP(StorageMap, 10,  Rename      ,
     _in CONST char_t* oldname
     _in CONST char_t* newname
  )
 
  CCI_DECL_METHOD_PP(StorageMap, 11,  GetValue    ,
     _in  CONST char_t* column
     _in  dm_uint       row 
     _out dm_real       value
  )

  CCI_DECL_METHOD_PP(StorageMap, 12,  SetValue    ,
     _in CONST char_t* column
     _in dm_uint       row 
     _in dm_real       value
  )

  CCI_DECL_METHOD_PP(StorageMap, 13,  ResizeCol  ,
     _in CONST char_t* column
     _in dm_uint       newsize 
  )

  CCI_DECL_METHOD_PP(StorageMap, 14,  SetCols    ,
     _in dm_cci(StringList) stringlist 
     _in dm_uint            minsize 
  )

  CCI_DECL_METHOD_PP(StorageMap, 15,  PushValue  ,
     _in CONST char_t*   column
     _in dm_real         value
  )

  CCI_DECL_METHOD_PP(StorageMap, 16,  Copy  ,
     _in dm_cci(StorageMap) from   
     _in CONST char_t*      srccol
     _in CONST char_t*      dstcol
     _in dm_uint            start
     _in dm_uint            end
  )

  CCI_DECL_METHOD_PP(StorageMap, 17,  GetStat,
     _in  CONST char_t*   column
     _in  CONST char_t*   type
     _out dm_real         value
  )

  CCI_DECL_METHOD_PP(StorageMap, 18,  GetInterval,
     _in  CONST char_t*   column
     _in  dm_uint         start
     _in  dm_real         lower
     _in  dm_real         upper
     _out dm_uint         next
  )

  CCI_DECL_METHOD_PP(StorageMap, 19,  Reserve,
     _in  CONST char_t*   column
     _in  dm_uint         capacity
  )

  CCI_DECL_METHOD_PP(StorageMap, 20,  DoArithmetics,
     _in  CONST char_t*   leftcol
     _in  CONST char_t*   rightcol
     _in  CONST char_t*   destcol
     _in  CONST char_t*   op
  )  
  
  /* Attributs */
  CCI_DECL_ATTR(StorageMap ,1, Count    ,dm_uint , [..G.] )
  CCI_DECL_ATTR(StorageMap ,2, MaxRows  ,dm_uint , [..G.] )
  CCI_DECL_ATTR(StorageMap ,3, Modified ,dm_bool , [.SGN] )

  /* Special values */

CCI_END_MODULE_DECL( StorageMap )

#endif // __doxygen
//----------------------------------------------------------------
#ifdef __cplusplus

#include "daim_modules/CCI_Interface.h"
//------------------------------------------------------------------------------------
/*! \ingroup Interfaces
 *  \brief Implement methods for storing and retrieving numerical data 
 *
 *  The StorageMap interface expose methods for storing/retrieving 
 *  list of numerical data by name.
 */
//------------------------------------------------------------------------------------
class dmIStorageMap : public dmINotify
{ 
  public:
   dmDECLARE_GENERIC_IINTERFACE(StorageMap)

   //-------------------------------------------------------
   /*! \brief Return a pointer to a storage array               
    * 
    * \param name    name of the column or NULL
    * \param size    number of data, if the column does not exists and
    *                size is non zero then the column is created
    *
    * If the method succeed, it returns a pointer to the start of the
    * column. Note that the first value (index 0) contains the size
    * of the array.
    */
   //-------------------------------------------------------
   dm_real* GetCol( CONST char_t* name, dm_uint size ) {
     IF_CCI_IMETHOD_RP(StorageMap,GetCol,_in  name _in  size _out(store) NULL)
     THEN return CCI_RETVAL(store); ENDIF
     return NULL;
   }

   //-------------------------------------------------------
   /*! \brief Clear all data (including headers) */
   //-------------------------------------------------------
   bool Clear() { CCI_RETURN_IMETHOD_NP_OK(StorageMap,Clear) }

   //-------------------------------------------------------
   /*! \brief Remove specified column
    * 
    *  \param name  name of the column or NULL
    */
   //-------------------------------------------------------
   bool Remove( CONST char_t* name ) {
      CCI_RETURN_IMETHOD_PP_OK(StorageMap,Remove, _in name )
   }
  
   //-------------------------------------------------------
   /*! \brief Return row values in a dmIPropertyList object
    * 
    * \param propertylist instance of a dmIPropertyList object
    * \param key          the associated key 
    * \param row          row index 
    * \param fmt          printf like format for the stored values 
    *
    * The values will be stored as properties in the previously open section
    * or sequence of the propertylist object.
    */
   //-------------------------------------------------------
   bool GetRow( dm_cci(PropertyList) propertylist, dm_uint key, dm_uint row, CONST char_t* fmt=NULL ) {
      CCI_RETURN_IMETHOD_PP_OK(StorageMap,GetRow,_in propertylist _in key _in row _in fmt)
   }

   //-------------------------------------------------------
   /*! \brief Set row values from a dmIPropertyList object
    * 
    * \param propertylist instance of a dmIPropertyList object
    * \param key          the associated key 
    * \param row          row index 
    *
    * The values will be inserted at \a row from the corresponding properties previously 
    * open section or sequence of the propertylist object.
    */
   //-------------------------------------------------------
   bool SetRow( dm_cci(PropertyList) propertylist, dm_uint key, dm_uint row ) {
      CCI_RETURN_IMETHOD_PP_OK(StorageMap,SetRow,_in propertylist _in key _in row )
   }

   //-------------------------------------------------------
   /*! \brief Return the list of column names  */
   //-------------------------------------------------------
   bool GetCols( dm_cci(StringList) stringlist  ) {
      CCI_RETURN_IMETHOD_PP_OK(StorageMap,GetCols,_in stringlist )
   }

   //-------------------------------------------------------
   /*! \brief Add \a count rows to all lists  */
   //-------------------------------------------------------
   bool AddRow( dm_uint count  ) {
     CCI_RETURN_IMETHOD_PP_OK(StorageMap,AddRow,_in count )
   }

   //-------------------------------------------------------
   /*! \brief Delete rows \a index from all lists */
   //-------------------------------------------------------
   bool DeleteRow( dm_uint row  ) {
     CCI_RETURN_IMETHOD_PP_OK(StorageMap,DeleteRow,_in row  )
   }

   //-------------------------------------------------------
   /*! \brief Send 'Modified' notification */
   //-------------------------------------------------------
   bool SetModified() {
      return CCI_ISET(StorageMap,Modified,dm_true) &&
             CCI_ISET(StorageMap,Modified,dm_false);
   }

   //-------------------------------------------------------
   /*! \brief Export data to file */
   //-------------------------------------------------------
   bool Export(dm_cci(StringList) stringlist, const char_t* uri ) {
      CCI_RETURN_IMETHOD_PP_OK(StorageMap,Export,_in stringlist _in uri)
   }
   
   //-------------------------------------------------------
   /*! \brief Get value from column \a column at \a row */
   //-------------------------------------------------------
   dm_real GetValue(const char_t* column, dm_uint row, dm_real dflt ) {
     IF_CCI_IMETHOD_RP(StorageMap,GetValue,_in column _in row _out(value) dflt)
     THEN return CCI_RETVAL(value); ENDIF
     return dflt;
   }

   //-------------------------------------------------------
   /*! \brief Set value from column \a column at \a row */
   //-------------------------------------------------------
   bool SetValue(const char_t* column, dm_uint row, dm_real value) {
     CCI_RETURN_IMETHOD_PP_OK(StorageMap,SetValue,_in column _in row _in value)
   }
   
   //-------------------------------------------------------
   /*! \brief Resize the column \a column to \a newsize
    * 
    * \note The column will be created if it does not exists.
    */
   //-------------------------------------------------------
   bool ResizeCol(const char_t* column, dm_uint newsize) {
     CCI_RETURN_IMETHOD_PP_OK(StorageMap,ResizeCol,_in column _in newsize )
   }

   //-------------------------------------------------------
   /*! \brief Create columns
    * 
    * \param stringlist A list of columns to be created
    * \param minsize    Minimum size of the specified columns. 
    */
   //-------------------------------------------------------
   bool SetCols(dm_cci(StringList) stringlist, dm_uint minsize) {
     CCI_RETURN_IMETHOD_PP_OK(StorageMap,SetCols, _in stringlist _in minsize ) 
   }

   //-------------------------------------------------------
   /*! \brief Set column size to zero 
    *
    * This method is a shortcut for ResizeCol(column,0);
    */
   //-------------------------------------------------------
   bool ClearCol(const char_t* column) {
     CCI_RETURN_IMETHOD_PP_OK(StorageMap,ResizeCol,_in column _in 0)
   }
   
   //-------------------------------------------------------
   /*! \brief Add a value to the column \a column */
   //-------------------------------------------------------
   bool PushValue(const char_t* column, dm_real value) {
     CCI_RETURN_IMETHOD_PP_OK(StorageMap,PushValue,_in column _in value)
   }

   //-------------------------------------------------------
   /*! \brief Copy a range of data from a column to another  */
   //-------------------------------------------------------
   bool Copy(dm_cci(storagemap) from,const char_t* srccol,const char_t* dstcol,
                    dm_uint start,dm_uint  end)
   {
     CCI_RETURN_IMETHOD_PP_OK(StorageMap,Copy,
           _in from 
           _in srccol 
           _in dstcol 
           _in start
           _in end
     )
   }

   bool Duplicate( const char_t* srccol,const char_t* dstcol,
                   dm_uint start,dm_uint  end)
   {
     CCI_RETURN_IMETHOD_PP_OK(StorageMap,Copy,
           _in _IInstance 
           _in srccol 
           _in dstcol 
           _in start
           _in end
     )
   }
   
   //-------------------------------------------------------
   /*! \brief Create a zero sized column with a preallocated 
    *  storage capacity 
    */
   //-------------------------------------------------------
   bool Reserve( const char_t* column, dm_uint capacity) {
     CCI_RETURN_IMETHOD_PP_OK(StorageMap,Reserve,_in column _in capacity)
   }
   
   //-------------------------------------------------------
   /*! \brief return statistical value on the specified column
    */
   //-------------------------------------------------------   
   dm_real GetStat( const char_t* column, const char_t* type ) {
     IF_CCI_IMETHOD_RP(StorageMap,GetStat,_in column _in type _out(value) 0)
     THEN return CCI_RETVAL(value); ENDIF
     return 0;
   }

   //-------------------------------------------------------
   /*! \brief return the length of the the specified column
    */
   //-------------------------------------------------------        
   dm_uint GetCount( CONST char_t* column ) {
     return static_cast<dm_uint>(this->GetStat(column,_TXT("len")));
   }
  
   //-------------------------------------------------------
   /*! \brief Return the range of data lying in a specified interval
    * 
    *  Given the  /a start index, the method return the /a last index
    *  that define the range of consecutive data lying in the specified 
    *  interval.
    */
   //-------------------------------------------------------        
   dm_uint GetInterval( CONST char_t* column, dm_uint start, dm_real lower, dm_real upper )
   {
     IF_CCI_IMETHOD_RP(StorageMap,GetInterval,
       _in column 
       _in start
       _in lower
       _in upper )
     THEN return CCI_RETVAL(next); 
     ELSE return dm_noindex;
     ENDIF
  }
};
//-------------------------------------------------------------------------------------
#endif /* __cplusplus */

#include "daim_modules/CCI_Epilog.h"

#endif /* CCI_StorageMap_h */
