
#ifndef daimCImg
#define daimCImg 1.0
//-----------------------------------------
// Attach a slice of a CImg to a dmImageData
//-----------------------------------------
void getImageData( dmImageData& data, const unsigned int z=0, const unsigned int v=0 )
{
  if(z >= depth || v >= dim ) 
    throw CImgArgumentException("CImg<%s>::getImageData() : Cannot get data from (%u,%u) from"
                                "(%u,%u,%u;%u) image",pixel_type(),z,v,width,height,depth,dim);
       
  data.Scan0  = ptr(0,0,z,v);
  data.Stride = width * sizeof(T);
  data.Width  = width;
  data.Height = height;
}

#endif // daimCImg
