#ifndef ASSERT_DEMOS_H
#define ASSERT_DEMOS_H

#ifdef USE_C_EXT
 //#ifdef ASSERT_V3M_ONLY
 #if !(defined(__NDS32_ISA_V3__) || defined(__NDS32_ISA_V3M__))
  #error USE_C_EXT is only supported by V3/V3M tool-chain!
 #endif
#endif //#ifdef USE_C_EXT

#ifdef __NDS32_ISA_V3M__
 //#ifdef ASSERT_16MB_ONLY
 #ifndef CFG_16MB
  #error V3M tool-chain only supports CFG_16MB (24-bit address)!
 #endif
#endif //#ifdef __NDS32_ISA_V3M__

#ifdef DEMO_CACHE
 #ifdef __NDS32_ISA_V3M__
  #error V3M tool-chain supports no CCTL by the demo implemented!
 #endif
#endif //#ifdef DEMO_CACHE

#endif //#ifdef ASSERT_DEMOS_H
