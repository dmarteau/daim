#ifndef dmToolPrefsVisitor_h
#define dmToolPrefsVisitor_h

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
// File         : dmToolPrefsVisitor.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

class __dmTools dmToolPrefsVisitor : public dmToolVisitor
{
 private:
   dmString            _id;
   dmLink<dmToolPrefs> _prefs;
   dmServiceManager    _mManager;
   
 public:
   dmToolPrefsVisitor( const dmString& id );
  ~dmToolPrefsVisitor();

   const dmString& Id() const { return _id; }

   dmServices* GetServices( dmClassInfo* );

   dmLink<dmToolPrefs> CoercePrefs( const dmString& _PrefsId );
   dmLink<dmToolPrefs> GetPrefs( dmTool* );

   void SetDefaults();

   virtual void Visit( dmTool& );

   static dmToolPrefsVisitor& GetPrefs ( const dmString::E* _id );
};
//---------------------------------------------------------------------------
// Helper class 
// Will act as a service manager
//---------------------------------------------------------------------------
template<class T> class dmToolServices
{
  private:
    T* _Instance;

  public:
   ~dmToolServices() {}
    dmToolServices( const dmString::E* _service ) 
    { 
       _Instance = reinterpret_cast<T*>(
          dmToolPrefsVisitor::GetPrefs(_service).GetServices( T::ClassInfo() )
       ); 
    }

    operator T*()   { return _Instance; }
    T* operator->() { return _Instance; }
};
//----------------------------------------------------------------
#endif // dmToolPrefsVisitor_h
