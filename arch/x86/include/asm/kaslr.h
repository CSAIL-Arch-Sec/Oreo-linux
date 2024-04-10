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

extern unsigned long gem5_module_high_delta;
extern unsigned long long gem5_user_high_offset;

void set_gem5_module_high_delta(unsigned long v);
void set_gem5_user_high_offset(unsigned long v);

#endif
