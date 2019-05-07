
#define COLORSPACE_CLSID    CCI_GET_CLSID(ColorSpaceYIQ)
#define COLORSPACE_VERSION  CCI_GET_VERSION(ColorSpaceYIQ)
#define COLORSPACE_REVISION 0
#define COLORSPACE_AUTHOR   _TXT("David Marteau")

#define COLORSPACE_NUMCHANNELS 3
#define COLORSPACE_NAMESPACE daim::colorspace::YIQ

#define COLORSPACE_CAN_MERGE dm_true

#include "_ColorspaceBoilerPlate.h"

#define COLORSPACE_EXTRACT_CHANNELS()   \
          COLORSPACE_EXTRACT(1) \
          COLORSPACE_EXTRACT(2) \
          COLORSPACE_EXTRACT(3)

#define COLORSPACE_SPLIT(src,r,p)  dmSplitYIQ(src,\
          COLORSPACE_CHANNEL(1),\
          COLORSPACE_CHANNEL(2),\
          COLORSPACE_CHANNEL(3),\
          r,p)

#define COLORSPACE_MERGE(src,r,p)  dmMergeYIQ(src,\
          COLORSPACE_CHANNEL(1),\
          COLORSPACE_CHANNEL(2),\
          COLORSPACE_CHANNEL(3),\
          r,p)


#include "_ColorspaceTemplate.cpp"
