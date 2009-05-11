#ifndef dmArrows_h
#define dmArrows_h

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
// File         : dmArrows.h
// Date         : 7/2004
// Author       : David Marteau
//--------------------------------------------------------

class __dmTools dmLineTerminator : public dmObject
{
  public:
    dmDeclareObject( dmLineTerminator );
    virtual dmLineTerminator* Clone() const = 0;
    virtual size_t            Size()  const = 0; 

    virtual void Display(  dmGraphics& , const dmPoint& ptStart, const dmPoint& ptSend )  const = 0;
};
//---------------------------------------------------------------------------
class __dmTools dmArrowTerminator : public dmLineTerminator
{
  private:
    int nA,nB,nC;
  public:
   typedef dm_point PtArray[5];

   dmArrowTerminator() {}
   dmArrowTerminator( const dmArrowTerminator& );
   dmArrowTerminator( int a, int b, int c );
   virtual ~dmArrowTerminator();

   dmArrowTerminator& operator=( const dmArrowTerminator& );

   virtual dmLineTerminator* Clone() const;
   virtual void Display(  dmGraphics& , const dmPoint& ptStart, const dmPoint& ptSend ) const;

   virtual size_t Size() const { return nB; } 

   void GetPoints( PtArray&, const dmPoint& ptStart, const dmPoint& ptEnd ) const; 
   void SetParameters( int  a, int  b, int  c );
   void GetParameters( int& a, int& b, int& c ) const;
   
   dmDeclareClassInfo( dmArrowTerminator );
};
//////////////////////////////////////////////////////////////////////////////
#endif // dmArrows.h
