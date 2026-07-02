/*
 * Config the features of startup demo programs.
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

// Please put the defines shared by makefile projects and AndeSight projects

#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

// GCC < 4.8.2 (Before BSP400 gcc version)
// Check the GCC version for toolchain builtin macro compatible issue.
#if GCC_VERSION < 40802

#ifdef NDS32_BASELINE_V3
#define __NDS32_ISA_V3__	NDS32_BASELINE_V3
#endif

#ifdef NDS32_BASELINE_V3M
#define __NDS32_ISA_V3M__	NDS32_BASELINE_V3M
#endif

#endif

// Chkeck whether the following intrinsic functions are supported by toolchain.
// Some intrinsic functions are not supported by old toolchain and need to be wrapped.
#ifndef __ASSEMBLER__

#include <nds32_intrinsic.h>

#ifndef __nds32__mtsr_isb
#define __nds32__mtsr_isb(val, srname)  \
do {__nds32__mtsr(val, srname);__nds32__isb();} while(0)
#endif

#ifndef __nds32__mtsr_dsb
#define __nds32__mtsr_dsb(val, srname)	\
do {__nds32__mtsr(val, srname);__nds32__dsb();} while(0)
#endif

#endif

#ifndef	CFG_MAKEFILE

// The defines are only used by AndeSight projects

//----------------------------------------------------------------------------------------------------

// Users can configure the defines in this area
// to match different environment setting

// Platform select : AG101P / AE210P / AG102
//#define	CFG_AG102		// platform is AG102
#define	CFG_AE210P		// platform is AE210P
//#define	CFG_AG101P		// platform is AG101P

// Address mode select
//#define	CFG_16MB		// platform is 16MB, if it isn't defined, platform is 4GB

// Support ZOL select			// do ZOL supporting when CPU supports ZOL
//#define	CFG_HWZOL

// Code coverage select
//#define	CFG_GCOV		// do code coverage support

// Simulation environment select
//#define	CFG_SIMU		// do simulation on SID

// Build mode select
// The BUILD_MODE can be specified to BUILD_XIP/BUILD_BURN/BUILD_LOAD only.
//
// NOTE: The BUILD_SIMU is not supported any more. Please uncomment CFG_SIMU
//       to select simulation environment.
#define BUILD_MODE	BUILD_XIP	// NOTE: AE210P support BUILD_XIP only


//----------------------------------------------------------------------------------------------------
// The followings are predefined settings
// Please do not modify them

#define	BUILD_LOAD	1		// The program is loaded by GDB or eBIOS
#define	BUILD_BURN	2		// The program is burned to the flash, but run in RAM
							// demo-ls2 use BURN mode
#define	BUILD_XIP	3		// The program is burned to the flash and run in the flash
							// To use this mode, xip linker script files must be used
							// demo-ls1 use XIP mode

#ifdef DEMO_LS1
	// demo-ls1 needs to use the XIP mode
	#undef BUILD_MODE
	#define BUILD_MODE	BUILD_XIP
#endif

#ifdef DEMO_LS2
	// demo-ls2 needs to use the BURN mode
	#undef BUILD_MODE
	#define BUILD_MODE	BUILD_BURN
#endif

#ifdef DEMO_LS3
	// demo-ls3 needs to use the XIP mode without RAM
	#undef BUILD_MODE
	#define BUILD_MODE	BUILD_XIP
	#define CFG_NORAM
#endif

#ifndef __NDS32_EXT_EX9__
	// This toolchain cannot support EX9
	#define CONFIG_NO_NDS32_EXT_EX9
#endif

#if BUILD_MODE == BUILD_BURN
	// Burn mode
	#define CFG_LLINIT      // do low level init
	#define CFG_REMAP       // do remap
#elif BUILD_MODE == BUILD_XIP
	// XIP mode
	#define CFG_LLINIT      // do low level init
#else
	// Load mode
#endif

#ifdef __NDS32_ISA_V3M__
	// v3m toolchain only support 16MB
	#undef CFG_16MB
	#define CFG_16MB
#endif

// Platform configure check
#ifdef CFG_AG102
#ifdef CFG_16MB
#error "AG102 does NOT supports CFG_16MB"
#endif
#endif

#ifdef CFG_AE210P
#if BUILD_MODE != BUILD_XIP
#error "AE210P supports BUILD_XIP only"
#endif
#endif

#endif // CFG_MAKEFILE

#endif // __CONFIG_H__
