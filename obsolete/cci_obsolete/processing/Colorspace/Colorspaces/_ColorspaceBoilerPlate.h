#ifndef _ColorSpaceBoilerPlate_h
#define _ColorSpaceBoilerPlate_h

#define COLORSPACE_EXTRACTOR( n ) \
     COLORSPACE_NAMESPACE::channels::get<n,integer_type>()

#define COLORSPACE_EXTRACT( n ) \
  case n: \
     daim::transform(rgn,dmPoint(0,0),_src->Gen(),_dst->Gen(),COLORSPACE_EXTRACTOR(n));\
     CCI_RETURN_OK()


#define COLORSPACE_CHANNEL( n ) *_ImageList.GetBuffer(n-1)

#endif
