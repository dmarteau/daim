
#define ENC_OK    0
#define ENC_INVAL 1
#define ENC_E2BIG 2

#include <stddef.h>

int utf8_to_latin1(const char** inbuf  , size_t *inbytesleft,
                         char** outbuf , size_t *outbytesleft
                  ) 
{
    if(inbuf && *inbuf)
    {  
      while((*inbytesleft)) 
      {  
        if(*outbytesleft == 0) return ENC_E2BIG; // not enough bytes in output buffer

        char utf8_chr = (*inbuf)[0];
        if(utf8_chr>='\300' && utf8_chr<='\377') 
        {
          ++(*inbuf);
          if(--(*inbytesleft)) 
          {
            if((*inbuf)[0]>='\200' && (*inbuf)[0]<='\277')
            {
              switch(utf8_chr) 
              {
                case '\302' : (*outbuf)[0] = (*inbuf)[0];      break;
                case '\303' : (*outbuf)[0] = ((unsigned char)(*inbuf)[0]) + 64; break;
                default  : 
                 (*outbuf)[0] = '\177';
                 (*outbuf)[1] = (*inbuf)[0];
                 ++(*outbuf); --(*outbytesleft);
              }
              ++(*outbuf); --(*outbytesleft);
              ++(*inbuf);  --(*inbytesleft);
            }
          }
        } else {
           (*outbuf)[0] = (*inbuf)[0];
           ++(*outbuf); --(*outbytesleft);
           ++(*inbuf);  --(*inbytesleft);
        }
      }  
      return ENC_OK; // OK
    }

  return ENC_INVAL; // Not input data
}
