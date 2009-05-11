
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "specfun.h"

#define ITMAX   200

#ifdef FLT_EPSILON
# define MACHEPS FLT_EPSILON  /* 1.0E-08 */
#else
# define MACHEPS 1.0E-08
#endif


/* AS239 value, e^-88 = 2^-127 */
#define MINEXP  (-88.0)

#ifdef FLT_MAX
# define OFLOW   FLT_MAX    /* 1.0E+37 */
#else
# define OFLOW   1.0E+37
#endif

/* AS239 value for igamma(a,x>=XBIG) = 1.0 */
#define XBIG    1.0E+08

/*
 * Mathematical constants
 */
#define LNPI 1.14472988584940016
#define LNSQRT2PI 0.9189385332046727
#ifdef PI
# undef PI
#endif
#define PI 3.14159265358979323846
#define PNT68 0.6796875
#define SQRT_TWO 1.41421356237309504880168872420969809  /* JG */

/* Prefer lgamma */
#ifndef GAMMA
# ifdef HAVE_LGAMMA
#  define GAMMA(x) lgamma (x)
# elif defined(HAVE_GAMMA)
#  define GAMMA(x) gamma (x)
# else
#  undef GAMMA
# endif
#endif


/* Macros to configure routines taken from CEPHES: */

/* Unknown arithmetic, invokes coefficients given in
 * normal decimal format.  Beware of range boundary
 * problems (MACHEP, MAXLOG, etc. in const.c) and
 * roundoff problems in pow.c:
 * (Sun SPARCstation)
 */
 

#define UNK 1

/* If you define UNK, then be sure to set BIGENDIAN properly. */

#ifdef FLOAT_WORDS_BIGENDIAN
#define BIGENDIAN 1
#else
#define BIGENDIAN 0
#endif


/* Define to support tiny denormal numbers, else undefine. */
#define DENORMAL 1

/* Define to ask for infinity support, else undefine. */
#define INFINITIES 1

/* Define to ask for support of numbers that are Not-a-Number,
   else undefine.  This may automatically define INFINITIES in some files. */
#define NANS 1

/* Define to distinguish between -0.0 and +0.0.  */
#define MINUSZERO 1

#define GPMAX(a,b) ( (a) > (b) ? (a) : (b) )
#define GPMIN(a,b) ( (a) < (b) ? (a) : (b) )

/* some machines have trouble with exp(-x) for large x
 * if MINEXP is defined at compile time, use gp_exp(x) instead,
 * which returns 0 for exp(x) with x < MINEXP
 * exp(x) will already have been defined as gp_exp(x) in plot.h
 */

static double gp_exp(double x)
{
#ifdef MINEXP    
    return (x < (MINEXP)) ? 0.0 : exp(x);
#else  /* MINEXP */
    int old_errno = errno;
    double result = exp(x);
    
    /* exp(-large) quite uselessly raises ERANGE --- stop that */
    if (result == 0.0)
  errno = old_errno;
    return result;
#endif /* MINEXP */
}

#include "matherr.c"
#include "polevl.c"

#ifndef GAMMA
#include "lngamma.c"
#endif //GAMMA

#include "igamma.c"
#include "erfc.c"
#include "erf.c"

