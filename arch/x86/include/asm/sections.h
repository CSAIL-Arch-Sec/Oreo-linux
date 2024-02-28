/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_SECTIONS_H
#define _ASM_X86_SECTIONS_H

#include <asm-generic/sections.h>
#include <asm/extable.h>

extern char __brk_base[], __brk_limit[];
extern char __end_rodata_aligned[];

#if defined(CONFIG_X86_64)
extern char __end_rodata_hpage_align[];

#ifdef CONFIG_X86_PIE
extern char __start_got[], __end_got[];
#endif

#endif

extern char __end_of_kernel_reserve[];

extern unsigned long _brk_start, _brk_end;

#endif	/* _ASM_X86_SECTIONS_H */
