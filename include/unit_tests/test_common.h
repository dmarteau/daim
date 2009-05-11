
#if defined(DAIM_UTILITIES)
#include "daim_utilities.h"
#elif defined(DAIM_LIB)
#include "daim.h"
#elif defined(DAIM_GLUE)
#include "cciDaimGlue.h"
#endif

#include "dmCrt.h"

static dm_bool gFailed = dm_false;
static const char* mArgv[] = {
  "--enable-trace-memory",
  #if defined(DAIM_LIB) && defined(CCI_REGISTRY_LOCATION)
  "--registry-location", CCI_REGISTRY_LOCATION,
  #endif
};

#if defined(DAIM_UTILITIES)

#define DM_BEGIN_TEST_BLOCK \
  dmUtilities::Initialize(mArgv,sizeof(mArgv)/sizeof(char*)); \
  dmTRY

#define DM_END_TEST_BLOCK      \
  dmCATCH(_E)                  \
    dmTRACE_EXCEPTION(_E);     \
    gFailed = dm_true;         \
  dmDONE                       \
  end_test:                    \
  dmUtilities::Finalize();

#elif defined(DAIM_LIB)

#define DM_BEGIN_TEST_BLOCK \
cci_result rv = DM_Initialize(mArgv,sizeof(mArgv)/sizeof(char*),0); \
if(CCI_SUCCEEDED(rv)) {

#define DM_END_TEST_BLOCK    \
  end_test:                  \
  DM_Finalize(false);        \
} else {                     \
  fprintf(stderr,"urggh, failed to initialize DAIM library with error %d.\n",rv); \
  gFailed = dm_true;         \
}

#elif defined(DAIM_GLUE)

#define DM_BEGIN_TEST_BLOCK \
cci_result rv = DM_InitDaimGlue(mArgv,sizeof(mArgv)/sizeof(char*),0); \
if(CCI_SUCCEEDED(rv)) {

#define DM_END_TEST_BLOCK    \
  end_test:                  \
  DM_ReleaseDaimGlue(false); \
} else {                     \
  fprintf(stderr,"urggh, failed to initialize DAIM library with error %d.\n",rv); \
  gFailed = dm_true;         \
}


#endif //!DAIM_LIB

#define DM_EXIT_TEST \
  fprintf(stderr,"%s\n",gFailed?"******* TEST FAILED":"******* TEST PASSED"); \
  return gFailed?2:0;

#define DM_TEST_EXPECT_RESULT(_expr)                                  \
  DM_BEGIN_MACRO                                                      \
    fprintf(stderr,"%s...",#_expr);                                   \
    if (!(_expr)) {                                                   \
      fprintf(stderr,"failed at %s , line %d\n",__FILE__, __LINE__);    \
      gFailed = dm_true;                                              \
      goto end_test;                                                  \
    } else                                                            \
      fprintf(stderr,"passed\n");                                     \
  DM_END_MACRO
