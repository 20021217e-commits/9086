#include "config.h"

#ifdef CFG_GCOV

	.section .ctors, "aw"
	.align 2
	.type	__CTOR_LIST__, @object
__CTOR_LIST__:
	.word	-1

	.section .dtors, "aw"
	.align	2
	.type	__DTOR_LIST__, @object
__DTOR_LIST__:
	.word	-1

/* Run all the global destructors on exit from the program.  */
	.section .text, "ax"
	.align	2
	.type	__do_global_dtors, @function
__do_global_dtors:
#if !(defined(__NDS32_ISA_V3M__) || defined(__NDS32_ISA_V3__))
	smw.adm	$r6, [$sp], $r6, 2
#else
	push25	$r6, 0
#endif
	la	$r6, __DTOR_LIST__+4
1:
	lwi333.bi $r0, [$r6], 4
	beqz	$r0, 2f
	bral	$r0
	b	1b
2:
#if !(defined(__NDS32_ISA_V3M__) || defined(__NDS32_ISA_V3__))
	lmw.bim	$r6, [$sp], $r6, 2
#else
	pop25   $r6, 0
#endif

/* .init section start.
   This must appear at the start of the .init section.  */
	.section .init
	.global _crt_init
	.type	_init, @function
_crt_init:

/* .fini section start.
   This must appear at the start of the .fini section.  */
	.section .fini
	.global _crt_fini
	.type	_fini, @function
_crt_fini:
	b	__do_global_dtors

#endif	// CFG_GCOV
