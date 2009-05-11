
#ifndef HAVE_ERFC

double erfc(double x)
{
  int fsign = (x >= 0);

  x = igamma(0.5, (x) * (x));
  if (x == -1.0) {
      //undefined = TRUE;
      x = 0.0;
  } else { 
      x = fsign ? (1.0 - x) : (1.0 + x);
  }
  return x;
}

#endif // HAVE_ERFC