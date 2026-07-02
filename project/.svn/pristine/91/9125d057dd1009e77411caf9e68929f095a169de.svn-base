#include "config.h"

#ifdef CFG_GCOV

/* Put a word containing zero at the end of each of our two lists of function
   addresses.  Note that the words defined here go into the .ctors and .dtors
   sections of the crtend.o file, and since that file is always linked in
   last, these words naturally end up at the very ends of the two lists
   contained in these two sections.  */

	.section .ctors, "aw"
	.align	2
	.type	__CTOR_END__, @object
__CTOR_END__:
	.zero	4

	.section .dtors, "aw"
	.align	2
	.type	__DTOR_END__, @object
__DTOR_END__:
	.zero	4

/* Run all global constructors for the program.
   Note that they are run in reverse order.  */

	.section .text, "ax"
	.align	2
	.type	__do_global_ctors, @function
__do_global_ctors:
#if !(defined(__NDS32_ISA_V3M__) || defined(__NDS32_ISA_V3__))
	smw.adm	$r6, [$sp], $r6, 2
	la	$r6, __CTOR_END__-4
	b	2f
1:
	bral	$r0
2:
	lwi	$r0, [$r6+(0)]
	movi	$r1, -1
	addi	$r6, $r6, -4
	bne	$r0, $r1, 1b
	lmw.bim	$r6, [$sp], $r6, 2
#else
	push25	$r6, 0
	la	$r6, __CTOR_END__-4
1:
	lwi.bi	$r0, [$r6], -4
	beqc	$r0, -1, 2f
	bral	$r0
	b	1b
2:
	pop25   $r6, 0
#endif

/* .init section end.
   This must live at the end of the .init section.  */
	.section .init
	j	__do_global_ctors

/* .fini section end.
   This must live at the end of the .fini section.  */
	.section .fini

#endif 	// CFG_GCOV
