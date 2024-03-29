/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_KASLR_H_
#define _ASM_KASLR_H_

unsigned long kaslr_get_random_long(const char *purpose);

#ifdef CONFIG_RANDOMIZE_MEMORY
void kernel_randomize_memory(void);
void init_trampoline_kaslr(void);
#else
static inline void kernel_randomize_memory(void) { }
static inline void init_trampoline_kaslr(void) {}
#endif /* CONFIG_RANDOMIZE_MEMORY */

#ifdef CONFIG_GEM5_KASLR_MODULE_PROTECTION_HIGH
extern unsigned long gem5_module_high_delta;
#endif
#ifdef CONFIG_GEM5_ASLR_PROTECTION_HIGH
extern unsigned long gem5_user_high_offset;
#endif

#endif
