#ifndef dmModuleLoader_h
#define dmModuleLoader_h


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


//--------------------------------------------------------------------
class dmModuleLoader : public dmGenericLoader
{
  public:
    dmILoader           _Self;
    dmIFileInputStream  _InputStream;
    dmIFileOutputStream _OutputStream;
    dmIStringList       _StringList;

    static dmFastMemory<dmModuleLoader> _MemPool;

    dmModuleLoader( dmLOADER_DESCRIPTION* ); 
   ~dmModuleLoader();

    virtual bool          GetInfo( dmFILE_BUFFER_INFO& );  
    virtual const char_t* GetText( const char_t* attribute );
    virtual size_t        SetText( const char_t* attribute, const char_t* value );

    virtual void Close();
    virtual bool BufferToDisk( dmLink<dmImage>& , dmFILE_BUFFER_INFO& );
    virtual bool DiskToBuffer( dmLink<dmImage>& , dmFILE_BUFFER_INFO& );

    virtual bool IsSupportedFormat( EPixelFormat );

    virtual void Release();

    static dmGenericLoader* CreateInstance( dmLOADER_DESCRIPTION*, int );

    DM_DECL_ALLOCATOR_NEW(dmModuleLoader,20)
    
    void* operator new(size_t)       { return _MemPool.Allocate(20); }
    void  operator delete( void* p ) { _MemPool.Free(p);  }
};
//-------------------------------------------------------------- 


#endif // dmModuleLoader_h

