
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

#define dmUserIncludes
#include "daim_kernel.h"
#include "regions/dmRegionLineBuffer.h"

//-------------------------------------------
dmRegionLineBuffer::dmRegionLineBuffer( const dmRgnHandle* prgn )
{
  _buffer = Obtain(prgn,NULL);
  ++_buffer->_usecount;
}
//-------------------------------------------
dmRegionLineBuffer::dmRegionLineBuffer( const dmRgnHandle* rgn1,
                                        const dmRgnHandle* rgn2,
                                        const dmRgnHandle* rgn3 )
{
  _buffer = NULL;
  if(rgn1) _buffer = Obtain(rgn1,_buffer);
  if(rgn2) _buffer = Obtain(rgn2,_buffer);
  if(rgn3) _buffer = Obtain(rgn3,_buffer);
  ++_buffer->_usecount;
}
//-------------------------------------------
dmRegionLineBuffer::~dmRegionLineBuffer()
{
  if(_buffer && --_buffer->_usecount <= 0) {
    _buffer->_buffer.Dealloc();
    delete _buffer;
  }
}
//-------------------------------------------
dm_size dmRegionLineBuffer::Allocate( dm_size size )
{
  dmDEBUG_ASSERT( _buffer != NULL );
  if(size >  _buffer->_size) {
    _buffer->_size = size;
    return _buffer->_buffer.Realloc(size);
  }
  return size;
}
//-------------------------------------------
dm_size dmRegionLineBuffer::Inflate( dm_size size )
{
  dmDEBUG_ASSERT( _buffer != NULL );
  return (_buffer->_size = _buffer->_buffer.Inflate(size));
}
//-------------------------------------------
dmRegionBuffer* dmRegionLineBuffer::Obtain( const dmRgnHandle* rgnptr, 
                                            dmRegionBuffer* bufptr )
{
  if(bufptr != NULL) 
  {
    // If the own the same buffer, just return it
    if(rgnptr->rgn_opbuf==bufptr) 
      return bufptr;

    // Check if we have already a buffer set
    if(rgnptr->rgn_opbuf!=NULL) { 
      if(rgnptr->rgn_opbuf->_size > bufptr->_size)
        return rgnptr->rgn_opbuf;             // return the largest one
      else
        return bufptr;
    }

    // No buffer is assigned yet,
    // so assign the buffer to the region
    rgnptr->rgn_opbuf = bufptr;
    ++bufptr->_usecount;
  } 
  else
   // There is no default buffer
   // Check if we have already a buffer set
   if(rgnptr->rgn_opbuf == NULL) {
     bufptr = new dmRegionBuffer;
     bufptr->_usecount = 1;
     bufptr->_size     = 0;
     bufptr->_context  = 0;      // This should be initialized with thread context id
     rgnptr->rgn_opbuf = bufptr;
   } else
     bufptr = rgnptr->rgn_opbuf; // Return the region buffer

  return bufptr;
}
//-------------------------------------------
void dmRegionLineBuffer::Release( const dmRgnHandle* rgnptr )
{
  dmRegionBuffer* bufptr = rgnptr->rgn_opbuf;
  rgnptr->rgn_opbuf = NULL;
  if(bufptr && --bufptr->_usecount <= 0) {
    bufptr->_buffer.Dealloc();
    delete bufptr;
  }
}
//-------------------------------------------
