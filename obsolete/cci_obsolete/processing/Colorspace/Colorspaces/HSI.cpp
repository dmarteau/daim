
#define COLORSPACE_CLSID    CCI_GET_CLSID(ColorSpaceIHS)
#define COLORSPACE_VERSION  CCI_GET_VERSION(ColorSpaceIHS)
#define COLORSPACE_REVISION 0
#define COLORSPACE_AUTHOR   _TXT("David Marteau")

#define COLORSPACE_NUMCHANNELS 3
#define COLORSPACE_NAMESPACE daim::colorspace::HSI

#define COLORSPACE_CAN_MERGE dm_true

#include "_ColorspaceBoilerPlate.h"

#define COLORSPACE_EXTRACT_CHANNELS()   \
          COLORSPACE_EXTRACT(3) \
          COLORSPACE_EXTRACT(1) \
          COLORSPACE_EXTRACT(2)

#define COLORSPACE_SPLIT(src,r,p)  dmSplitHSI(src,\
          COLORSPACE_CHANNEL(3),\
          COLORSPACE_CHANNEL(1),\
          COLORSPACE_CHANNEL(2),\
          r,p)

#define COLORSPACE_MERGE(src,r,p)  dmMergeHSI(src,\
          COLORSPACE_CHANNEL(3),\
          COLORSPACE_CHANNEL(1),\
          COLORSPACE_CHANNEL(2),\
          r,p)

#include "_ColorspaceTemplate.cpp"
