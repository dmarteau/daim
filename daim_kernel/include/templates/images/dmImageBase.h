#ifndef dmImageBase_h
#define dmImageBase_h

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
// File         : dmImageBase.h
// Date         : 7/2004
// Description  : Part of DAIM Image processing library
// Author       : David Marteau
//--------------------------------------------------------

namespace daim {

template<class value_type>
struct alloc_image_data
{
  value_type* pointer;
  alloc_image_data( size_t n ) { 
     pointer = static_cast<value_type*>(dm_malloc_handler( n ));  
  }

  operator value_type* () { return pointer; }
};

template<class value_type>
inline void free_image_data( value_type* pointer ) {
  dm_free_handler(pointer);
}

//--------------------------------------------------------------------------
// generalized image class
//--------------------------------------------------------------------------
template<class T> 
class image
{
 public:
   // pixel types definitions
   typedef image<T> image_type;

   typedef T pixel_type;
   typedef daim::pixel_traits<T>                      traits_type;

   typedef typename traits_type::value_type           value_type;
   typedef typename traits_type::pointer_type         pointer_type;
   typedef typename traits_type::reference_type       reference_type;
   typedef typename traits_type::const_pointer_type   const_pointer_type;
   typedef typename traits_type::const_reference_type const_reference_type;

   typedef value_type*       __pointer;
   typedef const value_type* __const_pointer;

   typedef const __pointer*       line_type;
   typedef const __const_pointer* const_line_type;

   // iterator like type
   class reverse_line_type
   {
     private:
       line_type L;
     public:
       reverse_line_type( line_type l ) : L(l) {}
       reverse_line_type( const reverse_line_type& l ) : L(l.L) {}
  
       reverse_line_type& operator=( const reverse_line_type& l ) { L=l.L; return *this; }

       __pointer operator[]( int i) { return L[i]; }
      
       __pointer operator*() { return *L; } 
       reverse_line_type& operator++() { --L; return *this; }
       reverse_line_type& operator--() { ++L; return *this; } 
   };

   // const Iterator like type
   class const_reverse_line_type
   {
     private:
       const_line_type L;
     public:
       const_reverse_line_type( const_line_type l ) : L(l) {}
       const_reverse_line_type( const_reverse_line_type& l ) : L(l.L) {} 

       const_reverse_line_type& operator=( const_reverse_line_type& l ) { L=l.L; return *this; }
             
       __const_pointer operator[]( int i)  const { return L[i]; }

       __const_pointer  operator*() { return *L; } 
       const_reverse_line_type& operator++() { --L; return *this; }
       const_reverse_line_type& operator--() { ++L; return *this; } 
   };

 private:
   size_t       im_width;
   size_t       im_height;
   int          im_stride;
   value_type** im_lines;  // lines vector
   
 private:
   image<T>(const image<T>& ) {}

 protected: 
   // default protected constructor
   image() : im_width(0), im_height(0),im_stride(0), im_lines(NULL) {}
   image<T>& construct(value_type* _scan0,size_t _w,size_t _h,int _stride);
   
 public:
   virtual ~image();   
   
   size_t unitsize()    const { return sizeof(value_type);  }
   size_t width()       const { return im_width;            }
   size_t height()      const { return im_height;           }
   size_t size()        const { return im_width*im_height;  }
   int    stride()      const { return im_stride;           }

   dmRect  rect()       const { return dmRect(0,0,width(),height()); }
   dmPoint dimensions() const { return dmPoint(width(),height());    }

   image<T>& fill( value_type );
   image<T>& fill( const dmRect& , value_type );

   pointer_type   operator[](int y)               { return im_lines[y]; }
   reference_type operator[]( const dm_point& p ) { return (*this)[p.y][p.x]; }
   //-------------------------------------------------------------
   line_type begin() { return im_lines; }
   line_type end()   { return im_lines+height(); }
   line_type last()  { return end()-1; }

   line_type begin(const dmRect& r) { return im_lines+r.Top(); }
   line_type end  (const dmRect& r) { return im_lines+r.Bottom()+1; }
   line_type last (const dmRect& r) { return im_lines+r.Bottom(); }

   reverse_line_type rbegin() { return last();    }
   reverse_line_type rend()   { return begin()-1; }
   reverse_line_type rlast()  { return begin();   }

   reverse_line_type rbegin(const dmRect& r) { return last(r);    }
   reverse_line_type rend  (const dmRect& r) { return begin(r)-1; }
   reverse_line_type rlast (const dmRect& r) { return begin(r);   }
 
   // const version of operators ---------------------------------
   const_pointer_type   operator[](int y)               const { return im_lines[y]; }
   const_reference_type operator[]( const dm_point& p ) const { return (*this)[p.y][p.x]; }
   
   const_line_type begin() const { return im_lines; }
   const_line_type end()   const { return im_lines+height(); }
   const_line_type last()  const { return end()-1; }

   const_line_type begin(const dmRect& r) const { return im_lines+r.Top(); }
   const_line_type end  (const dmRect& r) const { return im_lines+r.Bottom()+1; }
   const_line_type last (const dmRect& r) const { return im_lines+r.Bottom(); }

   const_reverse_line_type rbegin() const { return last();    }
   const_reverse_line_type rend()   const { return begin()-1; }
   const_reverse_line_type rlast()  const { return begin();   }

   const_reverse_line_type rbegin(const dmRect& r) const { return last(r);    }
   const_reverse_line_type rend  (const dmRect& r) const { return begin(r)-1; }
   const_reverse_line_type rlast (const dmRect& r) const { return begin(r);   }
 
   //-------------------------------------------------------------
   // Copy rectangular area from source at location p 
   // WARNING : does not handle memory overlapping
   image<T>& copy( const image<T>&   , const dmRect&, const dmPoint& );
   image<T>& copy( const image<T>& im, const dmPoint& p=dmPoint(0,0) ) {
     return copy(im,im.rect(),p);
   }

   image<T>& operator=(const image<T>& im) { 
     copy(im); 
     return *this;
   }
   
   DM_DECL_ARENA_OPERATOR_NEW(image<T>)
};
//---------------------------------------------------------------------------
// container image
//---------------------------------------------------------------------------
template<class T>
class cont_image : public image<T>
{
 public:
   typedef typename image<T>::image_type image_type;

 private:
   typename image_type::value_type* im_buffer;

 public:
   cont_image<T>& reserve( size_t _width, size_t _height);
   cont_image<T>& reserve( const dmRect& r ) { 
     return reserve(r.Width(),r.Height()); 
   }
   cont_image<T>& reserve( size_t _width, size_t _height, 
                           typename image_type::value_type _val )
   {
     reserve(_width,_height);
     this->fill(_val);
     return *this;
   }

   cont_image<T>& operator=(const cont_image<T>& im) { 
      copy(im); 
      return *this; 
   }

   cont_image<T>& copy( const cont_image<T>& im,const dmRect& r ) {
     if(&im!=this) { 
       reserve(r); 
       copy(im);
     } 
	 return *this;
   }

   cont_image<T>() : im_buffer(NULL) {}

   cont_image(const dmRect& r) : im_buffer(NULL)                     { reserve(r.Width(),r.Height());     }
   cont_image(size_t w,size_t h) : im_buffer(NULL)                   { reserve(w, h);                     }

   cont_image(const image<T>& im, const dmRect& r) : im_buffer(NULL) { reserve(r);         image<T>::copy(im);}
   cont_image(const image<T>& im) : im_buffer(NULL)                  { reserve(im.rect()); image<T>::copy(im);}

   cont_image(size_t w,size_t h,typename image_type::value_type x) : im_buffer(NULL)      { reserve(w, h, x);                  }
   cont_image(const dmRect& r , typename image_type::value_type x) : im_buffer(NULL)       { reserve(r.Width(),r.Height(),x);   }

  ~cont_image() { if(im_buffer) free_image_data(im_buffer); } 
};
//---------------------------------------------------------------------------
// slice-image type
//---------------------------------------------------------------------------
template<class T>
class slice_image : public image<T>
{
  private:
    image<T>& im_parent;
    
  public:    
    slice_image<T>& operator=(const image<T>& im ) { 
      this->copy(im); 
      return *this; 
    }

    slice_image( image<T>& im,const dmRect& r);
   ~slice_image() {}
};
//---------------------------------------------------------------------------
// tmp-image type
//---------------------------------------------------------------------------
template<class T>
class tmp_image : public image<T>
{
 public:
   typedef typename image<T>::image_type image_type;

  ~tmp_image() {}
   tmp_image() {}
   tmp_image(typename image_type::value_type* _scan0,size_t _w,size_t _h,int _stride) { 
     this->construct(_scan0,_w,_h,_stride); 
   }
};
//------------------------------------------------------------------------

template<class line_type> 
inline int line_stride( const line_type l ) { 
  return reinterpret_cast<const dm_byte*>(*(l+1)) - reinterpret_cast<const dm_byte*>(*l);
} 

template<class T> 
inline T* offset_ptr( T* ptr, int _offset ) { 
  return reinterpret_cast<T*>( reinterpret_cast<dm_byte*>(ptr)+_offset );
} 

template<class T> 
inline T* const_offset_ptr( T* ptr, int _offset ) { 
  return reinterpret_cast<T*>( reinterpret_cast<const dm_byte*>(ptr)+_offset );
} 

template<class T1,class T2> 
inline bool equal_size( const image<T1>& _1, const image<T2>& _2 ) {
  return _1.dimensions() == _2.dimensions() ; 
}

}; // namespace daim

#include "dmImageBase.hxx"
//------------------------------------------------------------------------
#endif // dmImageBase_h
