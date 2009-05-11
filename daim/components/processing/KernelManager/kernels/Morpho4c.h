/*
 * Daim kernel definitions
 * Auto generated files DO NOT CHANGE
 *
 * 2008112113
 */
#ifndef Morpho4c_h
#define Morpho4c_h

namespace Morpho4c {

//============================
// Kernel Family L(4)
// Squelette homotopique
//============================

BEGIN_KERNEL(L_4,L1)
-1,-1,-1,
     0, 1, 0,
     1, 1, 1
END_KERNEL

BEGIN_KERNEL(L_4,L2)
0,-1, 0,
     1, 1,-1,
     1, 1, 0
END_KERNEL

BEGIN_KERNEL(L_4,L3)
1, 0,-1,
     1, 1,-1,
     1, 0,-1
END_KERNEL

BEGIN_KERNEL(L_4,L4)
1, 1, 0,
     1, 1,-1,
     0,-1, 0
END_KERNEL

BEGIN_KERNEL(L_4,L5)
1, 1, 1,
     0, 1, 0,
    -1,-1,-1
END_KERNEL

BEGIN_KERNEL(L_4,L6)
0, 1, 1,
    -1, 1, 1,
     0,-1, 0
END_KERNEL

BEGIN_KERNEL(L_4,L7)
-1, 0, 1,
    -1, 1 ,1,
    -1, 0, 1
END_KERNEL

BEGIN_KERNEL(L_4,L8)
0,-1, 0,
    -1, 1, 1,
     0, 1, 1
END_KERNEL

BEGIN_FAMILY(L_4)
  KERNEL_ENTRY(L_4,L1,1,1,3,3,0)
  KERNEL_ENTRY(L_4,L2,1,1,3,3,0)
  KERNEL_ENTRY(L_4,L3,1,1,3,3,0)
  KERNEL_ENTRY(L_4,L4,1,1,3,3,0)
  KERNEL_ENTRY(L_4,L5,1,1,3,3,0)
  KERNEL_ENTRY(L_4,L6,1,1,3,3,0)
  KERNEL_ENTRY(L_4,L7,1,1,3,3,0)
  KERNEL_ENTRY(L_4,L8,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family I(4)
// Points isoles
//============================

BEGIN_KERNEL(I_4,I)
0,-1, 0,
   -1, 1,-1,
    0,-1, 0
END_KERNEL

BEGIN_FAMILY(I_4)
  KERNEL_ENTRY(I_4,I,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family E(4)
// Points extremes
//============================

BEGIN_KERNEL(E_4,E1)
0, 1, 0,
   -1, 1,-1,
    0,-1, 0
END_KERNEL

BEGIN_KERNEL(E_4,E2)
0,-1, 0,
   -1, 1, 1,
    0,-1, 0
END_KERNEL

BEGIN_KERNEL(E_4,E3)
0,-1, 0,
   -1, 1,-1,
    0, 1, 0
END_KERNEL

BEGIN_KERNEL(E_4,E4)
0,-1,-1,
    1, 1,-1,
    0,-1,-1
END_KERNEL

BEGIN_FAMILY(E_4)
  KERNEL_ENTRY(E_4,E1,1,1,3,3,0)
  KERNEL_ENTRY(E_4,E2,1,1,3,3,0)
  KERNEL_ENTRY(E_4,E3,1,1,3,3,0)
  KERNEL_ENTRY(E_4,E4,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family H(4)
// Boule 3x3
//============================

BEGIN_KERNEL(H_4,H)
0,1,0,
    1,1,1,
    0,1,0
END_KERNEL

BEGIN_FAMILY(H_4)
  KERNEL_ENTRY(H_4,H,1,1,3,3,0)
END_FAMILY

} // namespace Morpho4c

/*
ADD_FAMILY_ENTRY(Morpho4c,L_4,"Morpho4c/L(4)",dm_null);
ADD_FAMILY_ENTRY(Morpho4c,I_4,"Morpho4c/I(4)",dm_null);
ADD_FAMILY_ENTRY(Morpho4c,E_4,"Morpho4c/E(4)",dm_null);
ADD_FAMILY_ENTRY(Morpho4c,H_4,"Morpho4c/H(4)",dm_null);
*/

#endif // Morpho4c_h
