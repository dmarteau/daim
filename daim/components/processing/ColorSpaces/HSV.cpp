
#define COLORSPACE_NAME HSV
#define COLORSPACE_CLASSNAME cciHSVColorSpace

#define COLORSPACE_NUMCHANNELS 3
#define COLORSPACE_NAMESPACE daim::colorspace::HSV

#define COLORSPACE_CAN_MERGE true

#include "_ColorspaceBoilerPlate.h"

#define COLORSPACE_EXTRACT_CHANNELS()   \
          COLORSPACE_EXTRACT(1) \
          COLORSPACE_EXTRACT(2) \
          COLORSPACE_EXTRACT(3)

#define COLORSPACE_SPLIT(src,r,p)  dmSplitHSV(src,\
          COLORSPACE_CHANNEL(1),\
          COLORSPACE_CHANNEL(2),\
          COLORSPACE_CHANNEL(3),\
          r,p)

#define COLORSPACE_MERGE(src,r,p)  dmMergeHSV(src,\
          COLORSPACE_CHANNEL(1),\
          COLORSPACE_CHANNEL(2),\
          COLORSPACE_CHANNEL(3),\
          r,p)

#include "_ColorspaceTemplate.cpp"
