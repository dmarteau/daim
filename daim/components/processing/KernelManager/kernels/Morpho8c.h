/*
 * Daim kernel definitions
 * Auto generated files DO NOT CHANGE
 *
 * 2008112113
 */
#ifndef Morpho8c_h
#define Morpho8c_h

namespace Morpho8c {

//============================
// Kernel Family L(8)
// Homotopic Squeleton
//============================

BEGIN_KERNEL(L_8,L1)
-1,-1,-1,
     0, 1, 0,
     1, 1, 1
END_KERNEL

BEGIN_KERNEL(L_8,L2)
0,-1,-1,
     1, 1,-1,
     0, 1, 0
END_KERNEL

BEGIN_KERNEL(L_8,L3)
1, 0,-1, 1, 1,-1, 1, 0,-1
END_KERNEL

BEGIN_KERNEL(L_8,L4)
0, 1, 0, 1, 1,-1, 0,-1,-1
END_KERNEL

BEGIN_KERNEL(L_8,L5)
1, 1, 1, 0, 1, 0,-1,-1,-1
END_KERNEL

BEGIN_KERNEL(L_8,L6)
0, 1, 0,-1, 1, 1,-1,-1, 0
END_KERNEL

BEGIN_KERNEL(L_8,L7)
-1, 0, 1,-1, 1 ,1,-1, 0, 1
END_KERNEL

BEGIN_KERNEL(L_8,L8)
-1,-1, 0,-1, 1, 1, 0, 1, 0
END_KERNEL

BEGIN_FAMILY(L_8)
  KERNEL_ENTRY(L_8,L1,1,1,3,3,0)
  KERNEL_ENTRY(L_8,L2,1,1,3,3,0)
  KERNEL_ENTRY(L_8,L3,1,1,3,3,0)
  KERNEL_ENTRY(L_8,L4,1,1,3,3,0)
  KERNEL_ENTRY(L_8,L5,1,1,3,3,0)
  KERNEL_ENTRY(L_8,L6,1,1,3,3,0)
  KERNEL_ENTRY(L_8,L7,1,1,3,3,0)
  KERNEL_ENTRY(L_8,L8,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family D(8)
// Homotopique Marker
//============================

BEGIN_KERNEL(D_8,D1)
-1, 0, 0,
    -1, 1, 1,
    -1, 0, 0
END_KERNEL

BEGIN_KERNEL(D_8,D2)
-1,-1, 0,
    -1, 1, 0,
     0, 0, 1
END_KERNEL

BEGIN_KERNEL(D_8,D3)
-1,-1,-1,
     0, 1, 0,
     0, 1, 0
END_KERNEL

BEGIN_KERNEL(D_8,D4)
0,-1,-1,
     0, 1,-1,
     1, 0, 0
END_KERNEL

BEGIN_KERNEL(D_8,D5)
0, 0,-1,
     1, 1,-1,
     0, 0,-1
END_KERNEL

BEGIN_KERNEL(D_8,D6)
1, 0, 0,
     0, 1,-1,
     0,-1,-1
END_KERNEL

BEGIN_KERNEL(D_8,D7)
0, 1, 0,
     0, 1, 0,
    -1,-1,-1
END_KERNEL

BEGIN_KERNEL(D_8,D8)
0, 0, 1,
    -1, 1, 0,
    -1,-1, 0
END_KERNEL

BEGIN_FAMILY(D_8)
  KERNEL_ENTRY(D_8,D1,1,1,3,3,0)
  KERNEL_ENTRY(D_8,D2,1,1,3,3,0)
  KERNEL_ENTRY(D_8,D3,1,1,3,3,0)
  KERNEL_ENTRY(D_8,D4,1,1,3,3,0)
  KERNEL_ENTRY(D_8,D5,1,1,3,3,0)
  KERNEL_ENTRY(D_8,D6,1,1,3,3,0)
  KERNEL_ENTRY(D_8,D7,1,1,3,3,0)
  KERNEL_ENTRY(D_8,D8,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family C(8)
// Convex hull
//============================

BEGIN_KERNEL(C_8,C1)
0, 1, 0,
     1,-1, 0,
     0, 1, 0
END_KERNEL

BEGIN_KERNEL(C_8,C2)
0, 1, 0,
     1,-1, 0,
     1, 0, 0
END_KERNEL

BEGIN_KERNEL(C_8,C3)
0, 1, 0,
     1,-1, 1,
     0, 0, 0
END_KERNEL

BEGIN_KERNEL(C_8,C4)
1, 1, 0,
     0,-1, 1,
     0, 0, 0
END_KERNEL

BEGIN_KERNEL(C_8,C5)
0, 1, 0,
     0,-1, 1,
     0, 1, 0
END_KERNEL

BEGIN_KERNEL(C_8,C6)
0, 0, 1,
     0,-1, 1,
     0, 1, 0
END_KERNEL

BEGIN_KERNEL(C_8,C7)
0, 0, 0,
     1,-1, 1,
     0, 1, 0
END_KERNEL

BEGIN_KERNEL(C_8,C8)
0, 0, 0,
     1,-1, 0,
     0, 1, 1
END_KERNEL

BEGIN_FAMILY(C_8)
  KERNEL_ENTRY(C_8,C1,1,1,3,3,0)
  KERNEL_ENTRY(C_8,C2,1,1,3,3,0)
  KERNEL_ENTRY(C_8,C3,1,1,3,3,0)
  KERNEL_ENTRY(C_8,C4,1,1,3,3,0)
  KERNEL_ENTRY(C_8,C5,1,1,3,3,0)
  KERNEL_ENTRY(C_8,C6,1,1,3,3,0)
  KERNEL_ENTRY(C_8,C7,1,1,3,3,0)
  KERNEL_ENTRY(C_8,C8,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family E(8)
// Extremal Points
//============================

BEGIN_KERNEL(E_8,E1)
0, 1, 0,
    -1, 1,-1,
    -1,-1,-1
END_KERNEL

BEGIN_KERNEL(E_8,E2)
-1, 0, 1,
    -1, 1, 0,
    -1,-1,-1
END_KERNEL

BEGIN_KERNEL(E_8,E3)
-1,-1, 0,
    -1, 1, 1,
    -1,-1, 0
END_KERNEL

BEGIN_KERNEL(E_8,E4)
-1,-1,-1,
    -1, 1, 0,
    -1, 0, 1
END_KERNEL

BEGIN_KERNEL(E_8,E5)
-1,-1,-1,
    -1, 1,-1,
     0, 1, 0
END_KERNEL

BEGIN_KERNEL(E_8,E6)
-1,-1,-1,
     0, 1,-1,
     1, 0,-1
END_KERNEL

BEGIN_KERNEL(E_8,E7)
0,-1,-1,
     1, 1,-1,
     0,-1,-1
END_KERNEL

BEGIN_KERNEL(E_8,E8)
1, 0,-1,
     0, 1,-1,
    -1,-1,-1
END_KERNEL

BEGIN_FAMILY(E_8)
  KERNEL_ENTRY(E_8,E1,1,1,3,3,0)
  KERNEL_ENTRY(E_8,E2,1,1,3,3,0)
  KERNEL_ENTRY(E_8,E3,1,1,3,3,0)
  KERNEL_ENTRY(E_8,E4,1,1,3,3,0)
  KERNEL_ENTRY(E_8,E5,1,1,3,3,0)
  KERNEL_ENTRY(E_8,E6,1,1,3,3,0)
  KERNEL_ENTRY(E_8,E7,1,1,3,3,0)
  KERNEL_ENTRY(E_8,E8,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family E(4)
// Isolated/Extremal Points
//============================

BEGIN_KERNEL(E_4,E1)
0, 0, 0,
   -1, 1,-1,
   -1,-1,-1
END_KERNEL

BEGIN_KERNEL(E_4,E2)
-1,-1, 0,
   -1, 1, 0,
   -1,-1, 0
END_KERNEL

BEGIN_KERNEL(E_4,E3)
-1,-1,-1,
   -1, 1,-1,
    0, 0, 0
END_KERNEL

BEGIN_KERNEL(E_4,E4)
0,-1,-1,
    0, 1,-1,
    0,-1,-1
END_KERNEL

BEGIN_FAMILY(E_4)
  KERNEL_ENTRY(E_4,E1,1,1,3,3,0)
  KERNEL_ENTRY(E_4,E2,1,1,3,3,0)
  KERNEL_ENTRY(E_4,E3,1,1,3,3,0)
  KERNEL_ENTRY(E_4,E4,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family I(8)
// Isolated points
//============================

BEGIN_KERNEL(I_8,K1)
-1,-1,-1,
    -1, 1,-1,
    -1,-1,-1
END_KERNEL

BEGIN_FAMILY(I_8)
  KERNEL_ENTRY(I_8,K1,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family H(8)
// Ball 3x3
//============================

BEGIN_KERNEL(H_8,H)
1,1,1,1,1,1,1,1,1
END_KERNEL

BEGIN_FAMILY(H_8)
  KERNEL_ENTRY(H_8,H,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family B(8)
// Ball 5x5
//============================

BEGIN_KERNEL(B_8,K1)
0,1,1,1,0,
     1,1,1,1,1,
     1,1,1,1,1,
     1,1,1,1,1,
     0,1,1,1,0,
END_KERNEL

BEGIN_FAMILY(B_8)
  KERNEL_ENTRY(B_8,K1,2,2,5,5,0)
END_FAMILY

} // namespace Morpho8c

/*
ADD_FAMILY_ENTRY(Morpho8c,L_8,"Morpho8c/L(8)",dm_null);
ADD_FAMILY_ENTRY(Morpho8c,D_8,"Morpho8c/D(8)",dm_null);
ADD_FAMILY_ENTRY(Morpho8c,C_8,"Morpho8c/C(8)",dm_null);
ADD_FAMILY_ENTRY(Morpho8c,E_8,"Morpho8c/E(8)",dm_null);
ADD_FAMILY_ENTRY(Morpho8c,E_4,"Morpho8c/E(4)",dm_null);
ADD_FAMILY_ENTRY(Morpho8c,I_8,"Morpho8c/I(8)",dm_null);
ADD_FAMILY_ENTRY(Morpho8c,H_8,"Morpho8c/H(8)",dm_null);
ADD_FAMILY_ENTRY(Morpho8c,B_8,"Morpho8c/B(8)",dm_null);
*/

#endif // Morpho8c_h
