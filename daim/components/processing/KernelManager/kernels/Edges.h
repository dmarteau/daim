/*
 * Daim kernel definitions
 * Auto generated files DO NOT CHANGE
 *
 * 2008112113
 */
#ifndef Edges_h
#define Edges_h

namespace Edges {

//============================
// Kernel Family Sobel
//
//============================

BEGIN_KERNEL(Sobel,Y)
-1,-2,-1, 0, 0, 0, 1, 2, 1,
END_KERNEL

BEGIN_KERNEL(Sobel,X)
-1, 0, 1,-2, 0, 2,-1, 0, 1
END_KERNEL

BEGIN_KERNEL(Sobel,R)
-2,-1, 0,-1, 0, 1, 0, 1, 2
END_KERNEL

BEGIN_FAMILY(Sobel)
  KERNEL_ENTRY(Sobel,Y,1,1,3,3,0)
  KERNEL_ENTRY(Sobel,X,1,1,3,3,0)
  KERNEL_ENTRY(Sobel,R,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family Sobel-X
//
//============================

BEGIN_KERNEL(Sobel_X,X)
-1, 0, 1,-2, 0, 2,-1, 0, 1
END_KERNEL

BEGIN_FAMILY(Sobel_X)
  KERNEL_ENTRY(Sobel_X,X,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family Sobel-Y
//
//============================

BEGIN_KERNEL(Sobel_Y,Y)
-1,-2,-1, 0, 0, 0, 1, 2, 1,
END_KERNEL

BEGIN_FAMILY(Sobel_Y)
  KERNEL_ENTRY(Sobel_Y,Y,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family Harris
//
//============================

BEGIN_KERNEL(Harris,X)
-2, -1, 0, 1, 2
END_KERNEL

BEGIN_KERNEL(Harris,Y)
-2, -1, 0, 1, 2
END_KERNEL

BEGIN_FAMILY(Harris)
  KERNEL_ENTRY(Harris,X,2,0,5,1,0)
  KERNEL_ENTRY(Harris,Y,0,2,1,5,0)
END_FAMILY

//============================
// Kernel Family Prewitt
//
//============================

BEGIN_KERNEL(Prewitt,Y)
-1,-1,-1, 0, 0, 0, 1, 1, 1
END_KERNEL

BEGIN_KERNEL(Prewitt,X)
-1, 0, 1,-1, 0, 1,-1, 0, 1
END_KERNEL

BEGIN_KERNEL(Prewitt,R)
-1,-1, 0,-1, 0, 1, 0, 1, 1
END_KERNEL

BEGIN_FAMILY(Prewitt)
  KERNEL_ENTRY(Prewitt,Y,1,1,3,3,0)
  KERNEL_ENTRY(Prewitt,X,1,1,3,3,0)
  KERNEL_ENTRY(Prewitt,R,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family Robinson
//
//============================

BEGIN_KERNEL(Robinson,Y)
-1,-1,-1, 1,-2, 1, 1, 1, 1
END_KERNEL

BEGIN_KERNEL(Robinson,X)
-1, 1, 1,-1,-2, 1,-1, 1, 1
END_KERNEL

BEGIN_KERNEL(Robinson,R)
-1,-1, 1,-1,-2, 1, 1, 1, 1
END_KERNEL

BEGIN_FAMILY(Robinson)
  KERNEL_ENTRY(Robinson,Y,1,1,3,3,0)
  KERNEL_ENTRY(Robinson,X,1,1,3,3,0)
  KERNEL_ENTRY(Robinson,R,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family Kirsh
//
//============================

BEGIN_KERNEL(Kirsh,Y)
-5,-5,-5, 3, 0, 3, 3, 3, 3
END_KERNEL

BEGIN_KERNEL(Kirsh,X)
-5, 3 ,3,-5, 0, 3,-5, 3, 3
END_KERNEL

BEGIN_KERNEL(Kirsh,R)
-5,-5, 3,-5, 0, 3, 3, 3, 3
END_KERNEL

BEGIN_FAMILY(Kirsh)
  KERNEL_ENTRY(Kirsh,Y,1,1,3,3,0)
  KERNEL_ENTRY(Kirsh,X,1,1,3,3,0)
  KERNEL_ENTRY(Kirsh,R,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family Gradient
//
//============================

BEGIN_KERNEL(Gradient,X)
-1, 0, 1
END_KERNEL

BEGIN_KERNEL(Gradient,Y)
-1, 0, 1
END_KERNEL

BEGIN_FAMILY(Gradient)
  KERNEL_ENTRY(Gradient,X,1,0,3,1,0)
  KERNEL_ENTRY(Gradient,Y,0,1,1,3,0)
END_FAMILY

//============================
// Kernel Family Laplace(8)
//
//============================

BEGIN_KERNEL(Laplace_8,K1)
1, 1, 1, 1, -8, 1, 1, 1, 1
END_KERNEL

BEGIN_FAMILY(Laplace_8)
  KERNEL_ENTRY(Laplace_8,K1,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family Laplace(4)
//
//============================

BEGIN_KERNEL(Laplace_4,K1)
0, 1, 0, 1, -4, 1, 0, 1, 0
END_KERNEL

BEGIN_FAMILY(Laplace_4)
  KERNEL_ENTRY(Laplace_4,K1,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family Mexican Hat
//
//============================

BEGIN_KERNEL(Mexican_Hat,K1)
0, 0,-1, 0, 0,
    0,-1,-2,-1, 0,
   -1,-2,16,-2,-1,
    0,-1,-2,-1, 0,
    0, 0,-1, 0, 0
END_KERNEL

BEGIN_FAMILY(Mexican_Hat)
  KERNEL_ENTRY(Mexican_Hat,K1,2,2,5,5,0)
END_FAMILY

//============================
// Kernel Family x-backward-diff
//
//============================

BEGIN_KERNEL(x_backward_diff,x)
-1,1
END_KERNEL

BEGIN_FAMILY(x_backward_diff)
  KERNEL_ENTRY(x_backward_diff,x,0,0,2,1,0)
END_FAMILY

//============================
// Kernel Family y-backward-diff
//
//============================

BEGIN_KERNEL(y_backward_diff,y)
-1,1
END_KERNEL

BEGIN_FAMILY(y_backward_diff)
  KERNEL_ENTRY(y_backward_diff,y,0,0,1,2,0)
END_FAMILY

//============================
// Kernel Family x-forward-diff
//
//============================

BEGIN_KERNEL(x_forward_diff,x)
1,-1
END_KERNEL

BEGIN_FAMILY(x_forward_diff)
  KERNEL_ENTRY(x_forward_diff,x,0,0,2,1,0)
END_FAMILY

//============================
// Kernel Family y-forward-diff
//
//============================

BEGIN_KERNEL(y_forward_diff,y)
1,-1
END_KERNEL

BEGIN_FAMILY(y_forward_diff)
  KERNEL_ENTRY(y_forward_diff,y,0,0,1,2,0)
END_FAMILY

//============================
// Kernel Family x-centered-diff
//
//============================

BEGIN_KERNEL(x_centered_diff,x)
-1,0,1
END_KERNEL

BEGIN_FAMILY(x_centered_diff)
  KERNEL_ENTRY(x_centered_diff,x,1,0,3,1,0)
END_FAMILY

//============================
// Kernel Family y-centered-diff
//
//============================

BEGIN_KERNEL(y_centered_diff,y)
-1,0,1
END_KERNEL

BEGIN_FAMILY(y_centered_diff)
  KERNEL_ENTRY(y_centered_diff,y,0,1,1,3,0)
END_FAMILY

} // namespace Edges

/*
ADD_FAMILY_ENTRY(Edges,Sobel,"Edges/Sobel","ABS");
ADD_FAMILY_ENTRY(Edges,Sobel_X,"Edges/Sobel-X",dm_null);
ADD_FAMILY_ENTRY(Edges,Sobel_Y,"Edges/Sobel-Y",dm_null);
ADD_FAMILY_ENTRY(Edges,Harris,"Edges/Harris","ABS");
ADD_FAMILY_ENTRY(Edges,Prewitt,"Edges/Prewitt","ABS");
ADD_FAMILY_ENTRY(Edges,Robinson,"Edges/Robinson","ABS");
ADD_FAMILY_ENTRY(Edges,Kirsh,"Edges/Kirsh","ABS");
ADD_FAMILY_ENTRY(Edges,Gradient,"Edges/Gradient","ABS");
ADD_FAMILY_ENTRY(Edges,Laplace_8,"Edges/Laplace(8)",dm_null);
ADD_FAMILY_ENTRY(Edges,Laplace_4,"Edges/Laplace(4)",dm_null);
ADD_FAMILY_ENTRY(Edges,Mexican_Hat,"Edges/Mexican Hat",dm_null);
ADD_FAMILY_ENTRY(Edges,x_backward_diff,"Edges/x-backward-diff",dm_null);
ADD_FAMILY_ENTRY(Edges,y_backward_diff,"Edges/y-backward-diff",dm_null);
ADD_FAMILY_ENTRY(Edges,x_forward_diff,"Edges/x-forward-diff",dm_null);
ADD_FAMILY_ENTRY(Edges,y_forward_diff,"Edges/y-forward-diff",dm_null);
ADD_FAMILY_ENTRY(Edges,x_centered_diff,"Edges/x-centered-diff",dm_null);
ADD_FAMILY_ENTRY(Edges,y_centered_diff,"Edges/y-centered-diff",dm_null);
*/

#endif // Edges_h
