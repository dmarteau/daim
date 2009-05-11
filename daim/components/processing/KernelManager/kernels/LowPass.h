/*
 * Daim kernel definitions
 * Auto generated files DO NOT CHANGE
 *
 * 2008112113
 */
#ifndef LowPass_h
#define LowPass_h

namespace LowPass {

//============================
// Kernel Family Gauss(8)-Isotrope
//
//============================

BEGIN_KERNEL(Gauss_8__Isotrope,K1)
1, 1, 1,
    1, 2, 1,
    1, 1, 1
END_KERNEL

BEGIN_FAMILY(Gauss_8__Isotrope)
  KERNEL_ENTRY(Gauss_8__Isotrope,K1,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family Gauss(8)
//
//============================

BEGIN_KERNEL(Gauss_8,K1)
1, 2, 1,
    2, 4, 2,
    1, 2, 1
END_KERNEL

BEGIN_FAMILY(Gauss_8)
  KERNEL_ENTRY(Gauss_8,K1,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family Gauss(4)
//
//============================

BEGIN_KERNEL(Gauss_4,K1)
0, 1, 0,
    1, 2, 1,
    0, 1, 0
END_KERNEL

BEGIN_FAMILY(Gauss_4)
  KERNEL_ENTRY(Gauss_4,K1,1,1,3,3,0)
END_FAMILY

//============================
// Kernel Family Gauss5x5
//
//============================

BEGIN_KERNEL(Gauss5x5,K1)
1,  4,  6,  4,  1,
    4, 16, 24, 16,  4,
    6, 24, 36, 24,  6,
    4, 16, 24, 16,  4,
    1,  4,  6,  4,  1
END_KERNEL

BEGIN_FAMILY(Gauss5x5)
  KERNEL_ENTRY(Gauss5x5,K1,2,2,5,5,0)
END_FAMILY

//============================
// Kernel Family Gauss5x5-Separable
//
//============================

BEGIN_KERNEL(Gauss5x5_Separable,K1)
1,  4,  6,  4,  1
END_KERNEL

BEGIN_KERNEL(Gauss5x5_Separable,K2)
1,  4,  6,  4,  1
END_KERNEL

BEGIN_FAMILY(Gauss5x5_Separable)
  KERNEL_ENTRY(Gauss5x5_Separable,K1,0,2,1,5,0)
  KERNEL_ENTRY(Gauss5x5_Separable,K2,2,0,5,1,0)
END_FAMILY

} // namespace LowPass

/*
ADD_FAMILY_ENTRY(LowPass,Gauss_8__Isotrope,"LowPass/Gauss(8)-Isotrope",dm_null);
ADD_FAMILY_ENTRY(LowPass,Gauss_8,"LowPass/Gauss(8)",dm_null);
ADD_FAMILY_ENTRY(LowPass,Gauss_4,"LowPass/Gauss(4)",dm_null);
ADD_FAMILY_ENTRY(LowPass,Gauss5x5,"LowPass/Gauss5x5",dm_null);
ADD_FAMILY_ENTRY(LowPass,Gauss5x5_Separable,"LowPass/Gauss5x5-Separable",dm_null);
*/

#endif // LowPass_h
