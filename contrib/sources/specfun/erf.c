
#ifndef HAVE_ERF

double erf( double x )
{
  int fsign = (x >= 0);

  x = igamma(0.5, (x) * (x));
  if (x == -1.0) {
      //undefined = TRUE;
      x = 0.0;
  } else {
      if (! fsign)
    x = -x;
  }
  return x;
}

#endif //HAVE_ERF