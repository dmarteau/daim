#ifndef daim_embed_h
#define daim_embed_h

extern "C" {
  
#ifndef DAIM_GLUE

  #define DM_Malloc  malloc
  #define DM_Free    free
  #define DM_Memcpy  memcpy
  #define DM_Realloc realloc
  #define DM_Memmove memmove
  #define DM_Memset  memset
  #define DM_Calloc  calloc

#else // DAIM_GLUE 

  // Use with 
  extern void* DM_Malloc  ( size_t size    );
  extern void  DM_Free    ( void *memblock );
  extern void* DM_Realloc ( void *memblock, size_t size );
  extern void* DM_Memmove ( void *dest, void *src, size_t count );
  extern void* DM_Memcpy  ( void *dest, void *src, size_t count );
  extern void* DM_Memset  ( void *dest, int c, size_t count );
  extern void* DM_Calloc  ( size_t nmemb, size_t size );
  
#endif // DAIM_GLUE 

}

#endif // daim_embed_h
