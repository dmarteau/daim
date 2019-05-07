#ifndef dmMatrixUtils_h
#define dmMatrixUtils_h

namespace dm_numerical 
{
//-----------------------------------------------------------------
// These are not always defined in some stl ports
//-----------------------------------------------------------------
template<class T> inline T max( const T& x, const T& y) { return (x >= y ? x : y); } 
template<class T> inline T min( const T& x, const T& y) { return (x <= y ? x : y); } 
template<class T> inline T abs( const T& x )     { return (x<0?-x:x); }
template<class T> inline T sgn( const T& x )     { return (x >= 0 ? 1 : -1); }  

template<class T> inline T absdiff( const T& x, const T& y ) {
  return ( x >= y ? x - y : y - x );   
}   

template<class T> inline T minimum( const T& x, const T& y, const T& z ) {
  return dm_numerical::min( x , dm_numerical::min( y, z ) );
}

template<class T> inline T maximum( const T& x, const T& y, const T& z ) {
  return dm_numerical::max( x , dm_numerical::max( y, z ) );
}

template<class T> inline T range( const T& x, const T& _min, const T& _max ) {
  return (x > _max) ? _max : (x < _min ? _min : x);
}

template<class T>
inline T sqr( const T& x ) { return x*x; }

template<class T>
inline T sign(const T&  a, const T&  b) { 
   return static_cast<T>(b >= 0.0 ? fabs(a) : -fabs(a)); }

template<class T>
inline bool in_range( const T& x, const T& _min, const T& _max ) { 
  return x >= _min && x <= _max; 
}

template<class T>
inline bool out_range( const T& x, const T& _min, const T& _max ) { 
  return x < _min || x > _max; 
}

} // namespace dm_numerical

#endif // dmMatrixUtils_h
