/*-----------------------------------------------------------------------------

  File        : greycstoration4integration.h

  Description : GREYCstoration PLUG-IN allowing easy integration in
  third parties softwares.
    
  (see http://www.greyc.ensicaen.fr/~dtschump/greycstoration/)

  THIS VERSION IS FOR DEVELOPERS ONLY. IT EASES THE INTEGRATION ALGORITHM IN
  THIRD PARTIES SOFTWARES. IF YOU ARE A USER OF GREYCSTORATION, PLEASE LOOK
  AT THE FILE 'greycstoration.cpp' WHICH IS THE SOURCE OF THE COMPLETE
  COMMAND LINE GREYCSTORATION TOOL.

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

#ifndef greycstoration4integration
// This tells you about the version of the GREYCstoration algorithm implemented
#define greycstoration4integration 2.3

// GREYCstoration parameters & state structure
//---------------------------------------------
template<typename t> struct _state {
  
  // Algorithm parameters
  const float amplitude, sharpness, anisotropy, alpha, sigma, dl, da, gauss_prec;
  const unsigned int interpolation, progression_step;
  const bool fast_approx;
  
  // Algorithm current state
  int x, y, N, state;
  float theta, nmax;
  bool compute_thetafield;
  CImg<t> dest, W, G, tmp;
  
  // Constructor
  _state(const float _amplitude,
	 const float _sharpness=0.8f,
	 const float _anisotropy=0.5f,
	 const float _alpha=0.2f,
	 const float _sigma=0.8f,
	 const float _dl=0.8f,
	 const float _da=30.0f,
	 const float _gauss_prec=2.0f,
	 const unsigned int _interpolation=0,
	 const bool _fast_approx=true,
	 const unsigned int _progression_step=1000):
    amplitude(_amplitude), sharpness(_sharpness), anisotropy(_anisotropy), alpha(_alpha), sigma(_sigma),
       dl(_dl), da(_da), gauss_prec(_gauss_prec), interpolation(_interpolation), progression_step(_progression_step),
       fast_approx(_fast_approx),      
       x(0),y(0),N(0),state(0),theta(0),nmax(0),compute_thetafield(false) {}
};

_state<float> *state;

// Initialization function. To be called before each GREYCstoration iteration.
//-----------------------------------------------------------------------------
unsigned int greycstoration(const float amplitude, const float sharpness=0.8f, const float anisotropy=0.5f,
			    const float alpha=0.2f,const float sigma=0.8f, const float dl=0.8f,const float da=30.0f,
			    const float gauss_prec=2.0f, const unsigned int interpolation=0, const bool fast_approx=true,
			    const unsigned int progression_step=1000) {
  if (is_empty())
    throw CImgInstanceException("CImg<%s>::greycstoration() : Instance image is empty.", pixel_type());
  if (sharpness<0 || anisotropy<0 || anisotropy>1 || alpha<0 || sigma<0 || dl<0 || da<0 || gauss_prec<0)
    throw CImgArgumentException("CImg<%s>::greycstoration() : Given parameters are amplitude(%g), sharpness(%g), "
				"anisotropy(%g), alpha(%g), sigma(%g), dl(%g), da(%g), gauss_prec(%g).\n"
				"Admissible parameters are in the range : amplitude>0, sharpness>0, anisotropy in [0,1], "
				"alpha>0, sigma>0, dl>0, da>0, gauss_prec>0.",
				pixel_type(),sharpness,anisotropy,alpha,sigma,dl,da,gauss_prec);
  state = new _state<float>(amplitude,sharpness,anisotropy,alpha,sigma,dl,da,
			    gauss_prec,interpolation,fast_approx,progression_step);
  return 0;
}

// The GREYCstoration algorithm itself
//-------------------------------------
unsigned int greycstoration() {  
  
  static CImg<float> val(2), vec(2,2);
  if (!state) return 100;
  
  switch (state->state) {
    
    // Computation of the diffusion tensor field T (initialization)
  case 0: { 
    state->G.assign(width,height,1,3,0);
    state->nmax = 0;
    state->x = state->y = 0;
    const CImg<float> blurred = get_blur(state->alpha);
    CImg_3x3x1(I,float);
    CImg<float> &G=state->G;
    cimg_mapV(*this,k) cimg_map3x3x1(blurred,xx,yy,0,k,I) {
      const float
	ixf = Inc-Icc, iyf = Icn-Icc,
	ixb = Icc-Ipc, iyb = Icc-Icp;
      G(xx,yy,0) += 0.5f*(ixf*ixf+ixb*ixb);
      G(xx,yy,1) += 0.25f*(ixf*iyf+ixf*iyb+ixb*iyf+ixb*iyb);
      G(xx,yy,2) += 0.5f*(iyf*iyf+iyb*iyb);
    }    
    G.blur(state->sigma);
    state->state++;    
    return 5;
  } break;
  
  // Computation of the diffusion tensor field T (running)
  case 1: {
    CImg<float> &G=state->G;
    int &x=state->x, &y=state->y;
    float &nmax=state->nmax;    
    const float
      power1 = 0.5f*state->sharpness, 
      power2 = static_cast<float>(power1/(1e-7+1-state->anisotropy));      
    for (unsigned int i=0; i<state->progression_step; i++) {
      G.get_tensor(x,y).symeigen(val,vec);
      const float l1 = val[1], l2 = val[0],
	ux = vec(1,0), uy = vec(1,1),
	vx = vec(0,0), vy = vec(0,1),
	n1 = (float)CIMG_STD::pow(1.0f+l1+l2,-power1),
	n2 = (float)CIMG_STD::pow(1.0f+l1+l2,-power2);
      G(x,y,0) = n1*ux*ux + n2*vx*vx;
      G(x,y,1) = n1*ux*uy + n2*vx*vy;
      G(x,y,2) = n1*uy*uy + n2*vy*vy;
      if (n1>nmax) nmax=n1;
      if (++x>=dimx()) { 
	x=0;
	if (++y>=dimy()) {
	  i=state->progression_step; 
	  G/=nmax;
	  state->state++;
	}
      }
    }
    return (unsigned int)(5.0f + (x + y*width)*5.0f/(width*height));
  } break;

  // Image smoothing along diffusion tensor field T (initialization)
  case 2: {
    state->dest.assign(width,height,1,dim,0);
    state->tmp.assign(dim);
    state->W.assign(width,height,1,3);
    state->x = state->y = state->N = 0;
    state->theta = (360%(int)state->da)/2.0f;
    state->compute_thetafield = true;
    state->state++;
    return 10;
  } break;

  // Image smoothing along diffusion tensor field T (running)
  case 3: {
    int &x=state->x, &y=state->y, &N=state->N;
    float &theta=state->theta;
    bool &compute_thetafield=state->compute_thetafield;
    CImg<float> &G=state->G, &dest=state->dest, &tmp=state->tmp, &W=state->W;
    const int dx1 = dimx()-1, dy1 = dimy()-1;
    const float sqrt2amplitude = (float)CIMG_STD::sqrt(2*state->amplitude), &dl = state->dl, &gauss_prec=state->gauss_prec,
      &da = state->da;
    
    for (unsigned int i = 0; i<state->progression_step; i++) {
      
      if (compute_thetafield) {
	compute_thetafield = false;
	const float thetar = (float)(theta*cimg::PI/180), vx = (float)(CIMG_STD::cos(thetar)), vy = (float)(CIMG_STD::sin(thetar));
	const float *pa = G.ptr(0,0,0,0), *pb = G.ptr(0,0,0,1), *pc = G.ptr(0,0,0,2);
	float *pd0 = W.ptr(0,0,0,0), *pd1 = W.ptr(0,0,0,1), *pd2 = W.ptr(0,0,0,2);
	cimg_mapXY(G,xg,yg) {
	  const float
	    a = *(pa++), b = *(pb++), c = *(pc++), 
	    u = a*vx + b*vy, v = b*vx + c*vy,
	    n = (float)CIMG_STD::sqrt(1e-5+u*u+v*v),
	    dln = dl/n;
	  *(pd0++) = u*dln;
	  *(pd1++) = v*dln;
	  *(pd2++) = n;
	}
      }
    
      tmp.fill(0);
      const float cu = W(x,y,0,0), cv = W(x,y,0,1), n = W(x,y,0,2);
      const float
	fsigma = (float)(n*sqrt2amplitude),
	length = gauss_prec*fsigma,
	fsigma2 = 2*fsigma*fsigma;
      float l, S=0, pu=cu, pv=cv, X=(float)x, Y=(float)y;
      if (state->fast_approx) switch (state->interpolation) {		  
      case 0: // Nearest-neighbor interpolation
	for (l=0; l<length; l+=dl) {
	  const float 
	    Xn = X<0?0:(X>=dx1?dx1:X),
	    Yn = Y<0?0:(Y>=dy1?dy1:Y);
	  const int xi = (int)(Xn+0.5f), yi = (int)(Yn+0.5f);
	  float u = W(xi,yi,0,0), v = W(xi,yi,0,1);
	  if ((pu*u+pv*v)<0) { u=-u; v=-v; }
	  cimg_mapV(*this,k) tmp[k]+=(float)(*this)(xi,yi,0,k);
	  X+=(pu=u); Y+=(pv=v); S++;
	} break;
      case 1: // Linear interpolation
	for (l=0; l<length; l+=dl) {
	  float u = (float)(W.linear_pix2d(X,Y,0,0)), v = (float)(W.linear_pix2d(X,Y,0,1));
	  if ((pu*u+pv*v)<0) { u=-u; v=-v; }
	  cimg_mapV(*this,k) tmp[k]+=(float)linear_pix2d(X,Y,0,k);
	  X+=(pu=u); Y+=(pv=v); S++;
	} break;
      default: // 2nd-order Runge-kutta interpolation
	for (l=0; l<length; l+=dl) {
	  float u0 = (float)(0.5f*dl*W.linear_pix2d(X,Y,0,0)), v0 = (float)(0.5f*dl*W.linear_pix2d(X,Y,0,1));
	  if ((pu*u0+pv*v0)<0) { u0=-u0; v0=-v0; }
	  float u = (float)(W.linear_pix2d(X+u0,Y+v0,0,0)), v = (float)(W.linear_pix2d(X+u0,Y+v0,0,1));
	  if ((pu*u+pv*v)<0) { u=-u; v=-v; }
	  cimg_mapV(*this,k) tmp[k]+=(float)linear_pix2d(X,Y,0,k);
	  X+=(pu=u); Y+=(pv=v); S++;
	} break;
      }
      else switch (state->interpolation) {		  
      case 0: // Nearest-neighbor interpolation
	for (l=0; l<length; l+=dl) {
	  const float 
	    coef = (float)CIMG_STD::exp(-l*l/fsigma2),
	    Xn = X<0?0:(X>=dx1?dx1:X),
	    Yn = Y<0?0:(Y>=dy1?dy1:Y);
	  const int xi = (int)(Xn+0.5f), yi = (int)(Yn+0.5f);
	  float u = W(xi,yi,0,0), v = W(xi,yi,0,1);
	  if ((pu*u+pv*v)<0) { u=-u; v=-v; }
	  cimg_mapV(*this,k) tmp[k]+=(float)(coef*(*this)(xi,yi,0,k));
	  X+=(pu=u); Y+=(pv=v); S+=coef;
	} break;
      case 1: // Linear interpolation
	for (l=0; l<length; l+=dl) {
	  const float coef = (float)CIMG_STD::exp(-l*l/fsigma2);
	  float u = (float)(W.linear_pix2d(X,Y,0,0)), v = (float)(W.linear_pix2d(X,Y,0,1));
	  if ((pu*u+pv*v)<0) { u=-u; v=-v; }
	  cimg_mapV(*this,k) tmp[k]+=(float)(coef*linear_pix2d(X,Y,0,k));
	  X+=(pu=u); Y+=(pv=v); S+=coef;
	} break;
      default: // 2nd-order Runge-kutta interpolation
	for (l=0; l<length; l+=dl) {
	  const float coef = (float)CIMG_STD::exp(-l*l/fsigma2);
	  float u0 = (float)(0.5f*dl*W.linear_pix2d(X,Y,0,0)), v0 = (float)(0.5f*dl*W.linear_pix2d(X,Y,0,1));
	  if ((pu*u0+pv*v0)<0) { u0=-u0; v0=-v0; }
	  float u = (float)(W.linear_pix2d(X+u0,Y+v0,0,0)), v = (float)(W.linear_pix2d(X+u0,Y+v0,0,1));
	  if ((pu*u+pv*v)<0) { u=-u; v=-v; }
	  cimg_mapV(*this,k) tmp[k]+=(float)(coef*linear_pix2d(X,Y,0,k));
	  X+=(pu=u); Y+=(pv=v); S+=coef;
	} break;
      }
      if (S>0) cimg_mapV(dest,k) dest(x,y,0,k)+=tmp[k]/S;
      else cimg_mapV(dest,k) dest(x,y,0,k)+=(float)((*this)(x,y,0,k));
    
      if (++x>=dimx()) { 
	x=0;
	if (++y>=dimy()) {
	  y=0;
	  theta+=da;
	  N++;
	  compute_thetafield = true;
	  if (theta>=360) {
	    i=state->progression_step;
	    const float *ptrs = dest.data+dest.size(); cimg_map(*this,ptrd,T) *ptrd = (T)(*(--ptrs)/N);
	    G.empty(); dest.empty(); tmp.empty(); W.empty();	  
	    delete state;
	    state = 0;
	    return 100;
	  }
	}
      }
    }
    return (unsigned int)(10.0f + (x + y*width + theta*width*height)*90.0f/(width*height*360));
  } break;
  
  }
  return 0;
}

#endif

