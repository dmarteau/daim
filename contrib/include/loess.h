#ifndef LOESS_H
#define LOESS_H
/* for the meaning of these fields, see struct.m */
/* longs are used here so that the codes can be called from S */

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif


#ifdef __cplusplus
extern "C" {
#define extern_struct
#else // __cplusplus
#define extern_struct extern
#endif // __cplusplus


extern_struct struct loess_struct {
	struct {
		long    n;
	        long    p;
	        double  *y;
	        double  *x;
		double	*weights;
	} in;
	struct {
	        double  span;
	        long    degree;
	        long    normalize;
	        long    parametric[8];
	        long    drop_square[8];
	        char    *family;
	} model;
	struct {
	        char    *surface;
	        char    *statistics;
	        double  cell;
	        char    *trace_hat;
	        long    iterations;
	} control;
	struct {
		long	*parameter;
		long	*a;
		double	*xi;
		double	*vert;
		double	*vval;
	} kd_tree;
	struct {
		double	*fitted_values;
    double  *fitted_residuals;
		double  enp;
		double	s;
		double  one_delta;
		double	two_delta;
		double	*pseudovalues;
		double	trace_hat;
		double	*diagonal;
		double	*robust;
		double  *divisor;
	} out;
};

extern_struct struct pred_struct {
	double	*fit;
	double	*se_fit;
	double  residual_scale;
	double  df;
};

extern_struct struct anova_struct {
	double	dfn;
	double	dfd;
	double  F_value;
	double  Pr_F;
};

extern_struct struct ci_struct {
	double	*fit;
	double	*upper;
	double  *lower;
};

void loess_setup(double *x,double *y, long n, long p, struct  loess_struct *lo);
void loess(struct loess_struct *lo);
void loess_summary(struct loess_struct *lo);
void loess_free_mem(struct loess_struct *lo);

void pw_free_mem(struct ci_struct *ci);

void predict(double *eval,long m, struct loess_struct *lo, 
             struct pred_struct *pre,long se);
void pointwise(struct pred_struct *pre,long m, double coverage,
               struct ci_struct *ci);
void anova(struct loess_struct *one,struct loess_struct * two, 
           struct anova_struct *out);

void pred_free_mem(struct pred_struct *pre);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#undef extern_struct

#endif // LOESS_H
