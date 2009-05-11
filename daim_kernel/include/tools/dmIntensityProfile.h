#ifndef dmIntensityProfile_h
#define dmIntensityProfile_h

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
// File         : dmIntensityProfile.h
// Date         : 10/2005
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//
//--------------------------------------------------------

#include <vector>

class dmIntensityProfile
{	
   public:
     struct value_type { 
        dmPoint p; 
        dm_real value;
        value_type()  : value(0) {}
        value_type( dm_int x, dm_int y , dm_real v=0 ) : p(x,y),value(v) {}  
        value_type( const dm_point& _p , dm_real v=0 ) : p(_p) ,value(v) {} 
     };

   typedef std::vector<value_type> vector_type;

   private:
     vector_type     _Data;
     dmSpatialUnits  _Units;

   public:
     typedef vector_type::const_iterator const_iterator;
     typedef vector_type::iterator       iterator;
   
     const_iterator Begin() const { return _Data.begin(); }
     const_iterator End()   const { return _Data.end();   }

   public:
     dmIntensityProfile();
    ~dmIntensityProfile();
         
     dmIntensityProfile& SetCoordinates(const dmLine&    );
     dmIntensityProfile& SetCoordinates(const dmPoly&    );
     dmIntensityProfile& SetCoordinates(const dmEllipse& );
     dmIntensityProfile& SetCoordinates(const dmRect&    );
     dmIntensityProfile& SetCoordinates(const dmCircle&  );

     void Clear();
     int  Count() const { return _Data.size();  }
     int  Size()  const { return _Data.size();  }
     bool Empty() const { return _Data.empty(); }
     
     dm_real        GetValue( int i ) const { return _Data[i].value; }
     const dmPoint& GetPoint( int i ) const { return _Data[i].p;     }

     const dmSpatialUnits& Units() const { return _Units; }
     void  SetUnits( dmSpatialUnits& aUnits ) { _Units = aUnits; }

     daim::gap<dm_real> GetRange() const;
     
     //--------------------------
     // Generic scalar operation
     //--------------------------

     template<class T> 
     void ComputeProfile( const daim::image<T>& src ) {
       typedef typename daim::image<T>::traits_type traits_type;
       dmRect r = src.rect();
       for(iterator it=_Data.begin();it!=_Data.end();++it) {
         if(dmPointInRectangle((*it).p,r)) 
           (*it).value = static_cast<dm_real>(traits_type::scalar_value(src[(*it).p]));
       }
     }
     
     bool ComputeProfile( const dmImage& aSource, bool bCopyUnits );

};
//--------------------------------------------------------
#endif // dmIntensityProfile_h
