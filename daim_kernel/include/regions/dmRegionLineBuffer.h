#ifndef dmRegionLineBuffer_h
#define dmRegionLineBuffer_h

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
// File         : dmRegionLineBuffer.h
// Date         : 7/2004
// Author       : David Marteau
//
// NOTE         : This class is used as a shared buffer
//                for regions operation, il will be shared
//                among several instance of dmRgnHandle to store
//                auxiliary results. This is a alternative to static
//                buffer and should be safe in multithreading 
//                environment.
//                    
//--------------------------------------------------------

class dmRgnHandle;
//------------------------------------------------
struct dmRegionBuffer
{
  dm_uint        _context;
  dm_uint        _usecount;
  dm_size        _size;
  dmRgnLineArray _buffer;

  DM_DECL_ARENA_OPERATOR_NEW(dmRegionBuffer)
};
//------------------------------------------------
class dmRegionLineBuffer
{
  private:
    dmRegionBuffer* _buffer;

  public:
    dmRegionLineBuffer( const dmRgnHandle* );
    dmRegionLineBuffer( const dmRgnHandle* , const dmRgnHandle* , const dmRgnHandle* );
   ~dmRegionLineBuffer();

    dmRgnLinePtr operator()() const { return _buffer->_buffer(); }

    operator dmRgnLineArray& () { return _buffer->_buffer; }

    dm_size Allocate( dm_size size = dmRgnLineArray::MaxDfltArraySize );
    dm_size Inflate ( dm_size size );

    static dmRegionBuffer* Obtain ( const dmRgnHandle*, dmRegionBuffer* );
    static void            Release( const dmRgnHandle* );
}; 
//------------------------------------------------
#endif
