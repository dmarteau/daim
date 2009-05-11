/*-----------------------------------------------------------------------------

    File        : greycstoration.cpp

    Description : GREYCstoration - A tool to denoise, inpaint and resize images.

    The GREYCstoration algorithm is an implementation of diffusion tensor-directed
    diffusion PDE's for image regularization and interpolation, published in

    "Fast Anisotropic Smoothing of Multi-Valued Images 
    using Curvature-Preserving PDE's"
    (D. Tschumperlé)
    Research Report "Cahiers du GREYC 05/01", February 2005),
    GREYC, CNRS UMR 6072
    (see also http://www.greyc.ensicaen.fr/~dtschump/greycstoration)
    
    "Vector-Valued Image Regularization with PDE's : A Common Framework
    for Different Applications"
    (D. Tschumperlé, R. Deriche).
    IEEE Transactions on Pattern Analysis and Machine Intelligence,
    Vol 27, No 4, pp 506-517, April 2005.

    Copyright  : David Tschumperle - http://www.greyc.ensicaen.fr/~dtschump/

  This software is governed by the CeCILL  license under French law and
  abiding by the rules of distribution of free software.  You can  use, 
  modify and/ or redistribute the software under the terms of the CeCILL
  license as circulated by CEA, CNRS and INRIA at the following URL
  "http://www.cecill.info". 
  
  As a counterpart to the access to the source code and  rights to copy,
  modify and redistribute granted by the license, users are provided only
  with a limited warranty  and the software's author,  the holder of the
  economic rights,  and the successive licensors  have only  limited
  liability. 
  
  In this respect, the user's attention is drawn to the risks associated
  with loading,  using,  modifying and/or developing or reproducing the
  software by the user in light of its specific status of free software,
  that may mean  that it is complicated to manipulate,  and  that  also
  therefore means  that it is reserved for developers  and  experienced
  professionals having in-depth computer knowledge. Users are therefore
  encouraged to load and test the software's suitability as regards their
  requirements in conditions enabling the security of their systems and/or 
  data to be ensured and,  more generally, to use and operate it in the 
  same conditions as regards security. 
  
  The fact that you are presently reading this means that you have had
  knowledge of the CeCILL license and that you accept its terms.

  ------------------------------------------------------------------------------*/

#ifndef greycstoration_version
#define greycstoration_version 2.4

namespace cimg_library {

struct GREYCStoration
{
  typedef unsigned char uchar;
  
  unsigned int iter, nb_iter,visu;
 
  // Inpainting
  unsigned int inpaint_init; // Inpainting init (0=black, 1=white, 2=noise, 3=unchanged, 4=interpol)
 
  // Resize
  const char *resize_geom0;  // Input image geometry
  const char *resize_geom;   // Output image geometry
  bool  resize_anchor;       // Anchor original pixels
  unsigned int resize_init;  // Initial estimate (1=block, 3=linear, 5=bicubic)

  float valmin,valmax;

  bool stopflag;

  CImg<uchar> mask;

  enum {
    restore = 0,
    inpaint = 1,
    resize  = 2,
  };

  GREYCStoration() : iter(0), nb_iter(0), inpaint_init(4), visu(0), stopflag(false),
                     resize_geom0((const char*)0),resize_geom((const char*)0),resize_anchor(true)
  {}

// get_geom() : read geometry from a string ('320x256' or '200%x200%' are valid geometries).
//------------

void get_geom(const char *geom, int &geom_w, int &geom_h) {
  char tmp[16];
  CIMG_STD::sscanf(geom,"%d%7[^0-9]%d%7[^0-9]",&geom_w,tmp,&geom_h,tmp+1);
  if (tmp[0]=='%') geom_w=-geom_w;
  if (tmp[1]=='%') geom_h=-geom_h;
}

void set_inpaint_parameters( const unsigned int init )  
{
  inpaint_init = init;
}

void set_resize_parameters(   
  const char *geom0       , // Input image geometry;
  const char *geom        , // Output image geometry;
  const bool  anchor     ,
  const unsigned int init ) 
{
  resize_geom0  = geom0;
  resize_geom   = geom;
  resize_anchor = anchor;
  resize_init   = init;
}
// Do one iteration of a Curvature-preserving PDE's.
//--------------------------------------------------
template<typename T,typename t> 
inline CImg<T>& blur_anisotropicPDE(
            CImg<T>& img,const CImg<t>& G, const bool curv_pres=true,
            const float amplitude=10.0f, const float dl=0.8f,const float da=30.0f,
            const float gauss_prec=2.0f, const unsigned int interpolation=0,
            const bool fast_approx=true) {
  
  // Check arguments and init variables
  if (img.is_empty())
    throw CImgArgumentException("GREYCstoration : Specified input image is empty.");
    
  if (amplitude==0) return img;

  CImg<float> dest(img.width,img.height,1,img.dim,0), tmp(img.dim), W(img.width,img.height,1,3);
  
  const float sqrt2amplitude = (float)CIMG_STD::sqrt(2*amplitude);
  const int dx1 = img.dimx()-1, dy1 = img.dimy()-1;
  int counter=0, countermax = (int)(img.width*img.height*(360/da)), N = 0;

  for (float theta=(360%(int)da)/2.0f; theta<360; (theta+=da),N++) 
  {
    const float
      thetar = (float)(theta*cimg::PI/180),
      vx = (float)(CIMG_STD::cos(thetar)),
      vy = (float)(CIMG_STD::sin(thetar));
    
      const t *pa = G.ptr(0,0,0,0), *pb = G.ptr(0,0,0,1), *pc = G.ptr(0,0,0,2);
      t *pd0 = W.ptr(0,0,0,0), *pd1 = W.ptr(0,0,0,1), *pd2 = W.ptr(0,0,0,2);
      cimg_mapXY(G,xg,yg) 
      {
        const t
         a = *(pa++), b = *(pb++), c = *(pc++), 
         u = a*vx + b*vy, v = b*vx + c*vy,
         n = (t)CIMG_STD::sqrt(1e-5+u*u+v*v),
         dln = dl/n;
         *(pd0++) = u*dln;
         *(pd1++) = v*dln;
         *(pd2++) = n;
       }

       cimg_mapXY(img,x,y) {
     
         if(!((counter++)%200000)) {
            const unsigned int pc = (unsigned int)(100.0*(counter+iter*countermax)/(nb_iter*countermax));
            setprogress( pc, iter+1 );
            if(stopflag)
               return img;
         }
      
      tmp.fill(0);
      const t cu = W(x,y,0,0), cv = W(x,y,0,1), n = W(x,y,0,2);
      const float
          fsigma = (float)(n*sqrt2amplitude),
          length = gauss_prec*fsigma,
          fsigma2 = 2*fsigma*fsigma;
          float l, pu=cu, pv=cv, X=(float)x, Y=(float)y;
      
      if (fast_approx) 
      {
        unsigned int S=0;
        switch (interpolation) {
          case 0: // Nearest-neighbor interpolation
            for (l=0; l<length; l+=dl) {
                 const float 
                   Xn = X<0?0:(X>=dx1?dx1:X),
                   Yn = Y<0?0:(Y>=dy1?dy1:Y);
                   const int xi = (int)(Xn+0.5f), yi = (int)(Yn+0.5f);
                   t u = W(xi,yi,0,0), v = W(xi,yi,0,1);
                   if ((pu*u+pv*v)<0) { u=-u; v=-v; }
                      cimg_mapV(img,k) tmp[k]+=(t)img(xi,yi,0,k);
                      X+=(pu=u); Y+=(pv=v); ++S;
            } break;
          case 1: // Linear interpolation
            for (l=0; l<length; l+=dl) {
                 t u = (t)(W.linear_pix2d(X,Y,0,0)), v = (t)(W.linear_pix2d(X,Y,0,1));
                 if ((pu*u+pv*v)<0) { u=-u; v=-v; }
                 cimg_mapV(img,k) tmp[k]+=(t)img.linear_pix2d(X,Y,0,k);
                 X+=(pu=u); Y+=(pv=v); ++S;
            } break;
          default: // 2nd-order Runge-kutta interpolation
            for (l=0; l<length; l+=dl) {
                 t u0 = (t)(0.5f*dl*W.linear_pix2d(X,Y,0,0)), v0 = (t)(0.5f*dl*W.linear_pix2d(X,Y,0,1));
                 if ((pu*u0+pv*v0)<0) { u0=-u0; v0=-v0; }
                 t u = (t)(W.linear_pix2d(X+u0,Y+v0,0,0)), v = (t)(W.linear_pix2d(X+u0,Y+v0,0,1));
                 if ((pu*u+pv*v)<0) { u=-u; v=-v; }
                 cimg_mapV(img,k) tmp[k]+=(t)img.linear_pix2d(X,Y,0,k);
                 X+=(pu=u); Y+=(pv=v); ++S;
            } break;
        }
        if (S>0) cimg_mapV(dest,k) dest(x,y,0,k)+=tmp[k]/S;
        else cimg_mapV(dest,k) dest(x,y,0,k)+=(t)(img(x,y,0,k));
      }
      else 
      {
        float S=0;
        switch (interpolation) {
          case 0: // Nearest-neighbor interpolation
            for (l=0; l<length; l+=dl) {
                 const float 
                 coef = (float)CIMG_STD::exp(-l*l/fsigma2),
                 Xn = X<0?0:(X>=dx1?dx1:X),
                 Yn = Y<0?0:(Y>=dy1?dy1:Y);
                 const int xi = (int)(Xn+0.5f), yi = (int)(Yn+0.5f);
                 t u = W(xi,yi,0,0), v = W(xi,yi,0,1);
                 if ((pu*u+pv*v)<0) { u=-u; v=-v; }
                 cimg_mapV(img,k) tmp[k]+=(t)(coef*img(xi,yi,0,k));
                 X+=(pu=u); Y+=(pv=v); S+=coef;
            } break;
          case 1: // Linear interpolation
            for (l=0; l<length; l+=dl) {
                 const float coef = (float)CIMG_STD::exp(-l*l/fsigma2);
                 t u = (t)(W.linear_pix2d(X,Y,0,0)), v = (t)(W.linear_pix2d(X,Y,0,1));
                 if ((pu*u+pv*v)<0) { u=-u; v=-v; }
                 cimg_mapV(img,k) tmp[k]+=(t)(coef*img.linear_pix2d(X,Y,0,k));
                 X+=(pu=u); Y+=(pv=v); S+=coef;
          } break;
          default: // 2nd-order Runge-kutta interpolation
            for (l=0; l<length; l+=dl) {
                 const float coef = (float)CIMG_STD::exp(-l*l/fsigma2);
                 t u0 = (t)(0.5f*dl*W.linear_pix2d(X,Y,0,0)), v0 = (t)(0.5f*dl*W.linear_pix2d(X,Y,0,1));
                 if ((pu*u0+pv*v0)<0) { u0=-u0; v0=-v0; }
                 t u = (t)(W.linear_pix2d(X+u0,Y+v0,0,0)), v = (t)(W.linear_pix2d(X+u0,Y+v0,0,1));
                 if ((pu*u+pv*v)<0) { u=-u; v=-v; }
                 cimg_mapV(img,k) tmp[k]+=(t)(coef*img.linear_pix2d(X,Y,0,k));
                 X+=(pu=u); Y+=(pv=v); S+=coef;
          } break;
        }
        if (S>0) cimg_mapV(dest,k) dest(x,y,0,k)+=tmp[k]/S;
        else cimg_mapV(dest,k) dest(x,y,0,k)+=(t)(img(x,y,0,k));
      }
    }    
  }
    
  const float *ptrs = dest.data+dest.size(); cimg_map(img,ptrd,T) *ptrd = (T)(*(--ptrs)/N);
  return img;
}

// Do one iteration of Shock-Filter PDE
//--------------------------------------
template<typename T,typename t> 
inline CImg<T>& shock_filterPDE(CImg<T>& img, const CImg<t>& G, const float dt=10.0f, const float edge=1.0f) 
{
  CImg<float> val,vec,U(img.dimx(),img.dimy(),1,3),veloc(img);
  CImg_3x3(I,float);
  const float nedge = 0.5f*edge;
  cimg_mapXY(img,x,y) {
    if (G(x,y,0)==G(x,y,1) && G(x,y,1)==G(x,y,2) && G(x,y,2)==0) 
      U(x,y,0)=U(x,y,1)=U(x,y,2)=0;
    else {
      G.get_tensor(x,y).symeigen(val,vec);
      U(x,y,0) = vec(0,0);
      U(x,y,1) = vec(0,1);
      U(x,y,2) = 1.0f-(float)CIMG_STD::pow(1.0f+val[0]+val[1],-nedge);
    }
  } 
  cimg_mapV(img,k) cimg_map3x3(img,x,y,0,k,I) {
    const float 
      a = U(x,y,0), b = U(x,y,1), norm = U(x,y,2),
      ixx = Inc+Ipc-2*Icc, iyy = Icn+Icp-2*Icc, ixy = 0.25f*(Inn+Ipp-Inp-Ipn),
      ixf = Inc-Icc, ixb = Icc-Ipc, iyf = Icn-Icc, iyb = Icc-Icp,
      itt = a*a*ixx + 2*a*b*ixy + b*b*iyy,
      it = a*cimg::minmod(ixf,ixb) + b*cimg::minmod(iyf,iyb);
    veloc(x,y,k) = -norm*cimg::sign(itt)*cimg::abs(it);
  }
  const CImgStats stats(veloc,false);
  const float vmax = (float)cimg::max(cimg::abs(stats.min),cimg::abs(stats.max));
  if (vmax>1e-5f) img+=veloc*dt/vmax;
  return img;
}

// Init parameters and variables for image restoration
//-----------------------------------------------------
bool init_restore( CImg<float>& img )
{
  return true;
}

// Init parameters and variables for image inpainting
//----------------------------------------------------
bool init_inpaint( CImg<float>& img )
{
  if(mask.is_empty()) {
     return false;
  }

  mask.resize(img.dimx(),img.dimy(),1,1);
  switch (inpaint_init) {
    case 0 : { cimg_mapXYV(img,x,y,k) if (mask(x,y)) img(x,y,k) = valmin; } break;
    case 1 : { cimg_mapXYV(img,x,y,k) if (mask(x,y)) img(x,y,k) = valmax; } break;
    case 2 : { cimg_mapXYV(img,x,y,k) if (mask(x,y)) img(x,y,k) = (float)(valmax*cimg::rand() + valmin); } break;
    case 3 : break;
    default: {
      CImg<uchar> tmask(mask),ntmask(tmask);
      CImg_3x3(M,uchar);
      CImg_3x3(I,float);
      while (CImgStats(ntmask,false).max>0) 
      {
        cimg_map3x3(tmask,x,y,0,0,M) if (Mcc && (!Mpc || !Mnc || !Mcp || !Mcn)) 
        {
          const float ccp = Mcp?0.0f:1.0f, cpc = Mpc?0.0f:1.0f,
          cnc = Mnc?0.0f:1.0f, ccn = Mcn?0.0f:1.0f, csum = ccp + cpc + cnc + ccn;
          cimg_mapV(img,k) {
            cimg_get3x3(img,x,y,0,k,I);
            img(x,y,k) = (ccp*Icp + cpc*Ipc + cnc*Inc + ccn*Icn)/csum;
          }
          ntmask(x,y) = 0;
        }
        tmask = ntmask;
      }
    } break;    
  }  
  
  return true;
}

// Init parameters and variables for image resizing
//--------------------------------------------------

bool init_resize( CImg<float>& img )
{
  //CIMG_STD::fprintf(stderr,"- Image Resizing mode\n");
  if(!resize_geom && !resize_geom0) {
    throw CImgArgumentException("GREYCstoration : You need to specify an output geometry or an input geometry.");
  }
  
  int w,h;
  if (resize_geom0) {
     int w0,h0;
     get_geom(resize_geom0,w0,h0);
     w0 = w0>0?w0:-w0*img.dimx()/100;
     h0 = h0>0?h0:-h0*img.dimy()/100;
     w = img.dimx();
     h = img.dimy();
     img.resize(w0,h0,-100,-100,5);
  }
  
   if(resize_geom) {
      get_geom(resize_geom,w,h);
      w = w>0?w:-w*img.dimx()/100;
      h = h>0?h:-h*img.dimy()/100;
   }    
   mask.assign(img.dimx(),img.dimy(),1,1,255);
   if (!resize_anchor) mask.resize(w,h,1,1,1); else mask = ~mask.resize(w,h,1,1,4);
   img.resize(w,h,1,-100,resize_init);
   img.cut(valmin,valmax);
   
   return true;
}

/*-----------------
  Main procedure
  ----------------*/

int greycstoration( 
    CImg<float>& img,
    const unsigned int operation,//= 0 restore, 1 inpaint, 2 resize
    const float dt              ,//= cimg_option("-dt",restore?50.0f:(inpaint?20.0f:20.0f),"Regularization strength for one iteration (>=0)");
    const float p               ,//= cimg_option("-p",restore?0.9f:(inpaint?0.3f:0.2f),"Contour preservation for regularization (>=0)");
    const float anisotropy      ,//= cimg_option("-a",restore?0.2f:(inpaint?1.0f:0.9f),"Regularization anisotropy (0<=a<=1)");
    const float alpha           ,//= cimg_option("-alpha",restore?0.5f:(inpaint?0.8f:0.1f),"Noise scale(>=0)");
    const float sigma           ,//= cimg_option("-sigma",restore?1.0f:(inpaint?2.0f:1.5f),"Geometry regularity (>=0)");
    const bool  fast_approx     ,//= cimg_option("-fast",true,"Use fast approximation for regularization (0 or 1)");
    const float gauss_prec      ,//= cimg_option("-prec",2.0f,"Precision of the gaussian function for regularization (>0)");
    const float dl              ,//= cimg_option("-dl",0.8f,"Spatial integration step for regularization (0<=dl<=1)");
    const float da              ,//= cimg_option("-da",30.0f,"Angular integration step for regulatization (0<=da<=90)");
    const unsigned int interp   ,//= cimg_option("-interp",0,"Interpolation type (0=Nearest-neighbor, 1=Linear, 2=Runge-Kutta)");
    const float sdt             ,//= cimg_option("-sdt",restore?0.0f:(inpaint?0.0f:10.0f),"Sharpening strength for one iteration (>=0)");
    const float sp               //= cimg_option("-sp",restore?0.5f:(inpaint?0.5f:3.0f)  ,"Sharpening edge threshold (>=0)");
//  const unsigned int nb_iter   //= cimg_option("-iter",restore?1:(inpaint?1000:3),"Number of regularization iterations (>0)");
  ) 
  
  
{  
  // Init variables
  //----------------
  ;
  
  valmin=0, valmax=0;

  const CImgStats stats(img,false);
  valmin = (float)stats.min; valmax = (float)stats.max;
  
  bool init = false;
  
  switch(operation) {
     case  inpaint : init = init_inpaint(img); break;
     case  resize  : init = init_resize (img); break;
     default       : init = init_restore(img); break;
  }
  
  if(!init)  return false;
    
  CImg<float>& dest = img;
    
  //-------------------------------------
  // Begin regularization PDE iterations
  //-------------------------------------
  //CIMG_STD::fprintf(stderr,"\r- Processing : initialization...    \t\t");

  notify_iter(0);

  for (iter=0; iter<nb_iter && !stopflag; iter++) 
  {  
    // Compute smoothed structure tensor field G
    //-------------------------------------------
    CImg_3x3(I,float);
    CImg<float> G(dest.dimx(),dest.dimy(),1,3,0);
    
    // XXX normalization ?????
    const CImg<float> blur = dest.get_blur(alpha).normalize(valmin,valmax);
    
    cimg_mapV(blur,k) cimg_map3x3(blur,x,y,0,k,I) {
      const float ixf = Inc-Icc, iyf = Icn-Icc, ixb = Icc-Ipc, iyb = Icc-Icp;
      G(x,y,0) += 0.5f*(ixf*ixf+ixb*ixb);
      G(x,y,1) += 0.25f*(ixf*iyf+ixf*iyb+ixb*iyf+ixb*iyb);
      G(x,y,2) += 0.5f*(iyf*iyf+iyb*iyb);
    } 
    G.blur(sigma);
    if (mask.data) cimg_mapXY(G,x,y) if (!mask(x,y)) G(x,y,0)=G(x,y,1)=G(x,y,2)=0;

    // Compute tensor field sqrt(T)
    //------------------------------
    if (dt>0)
    {
      CImg<float> T(G.dimx(),G.dimy(),1,3), val, vec;
      const float 
        np1 = 0.5f*p,
        np2 = static_cast<float>(np1/(1e-7+1-anisotropy));
      float nmax = 0;  
      cimg_mapXY(G,x,y) 
      {
        G.get_tensor(x,y).symeigen(val,vec);
        if (G(x,y,0)==G(x,y,1) && G(x,y,1)==G(x,y,2) && G(x,y,2)==0) T(x,y,0)=T(x,y,1)=T(x,y,2)=0;
        else {
          const float l1 = val[1], l2 = val[0], u = vec(1,0), v = vec(1,1),      
                      n1 = (float)CIMG_STD::pow(1.0f+l1+l2,-np1),
                      n2 = (float)CIMG_STD::pow(1.0f+l1+l2,-np2);
          T(x,y,0) = n1*u*u + n2*v*v;
          T(x,y,1) = u*v*(n1-n2);
          T(x,y,2) = n1*v*v + n2*u*u;
          if (n1>nmax) nmax=n1;
        }
      }    
      T/=nmax;   
      blur_anisotropicPDE(dest,T,false,dt,dl,da,gauss_prec,interp,fast_approx);
    }
    
    if (sdt>0) {
      shock_filterPDE(dest,G,sdt,sp);
      dest.cut(valmin,valmax);
    }

    // Prepare for next iteration
    //---------------------------
    notify_iter(iter);
  }
  
  setprogress( 100, iter );

  // Save result and exit
  //----------------------
  dest.cut(valmin,valmax);
  return true;
}

  #ifdef greycstoration_plugin
    #include greycstoration_plugin
  #endif // greycstoration_plugin

}; // struct GREYCStoration

} // namespace cimg_library

#endif //greycstoration_version

