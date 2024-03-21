/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_PAGE_DEFS_H
#define _ASM_X86_PAGE_DEFS_H

#include <linux/const.h>
#include <linux/types.h>
#include <linux/mem_encrypt.h>

/* PAGE_SHIFT determines the page size */
#define PAGE_SHIFT		12
#define PAGE_SIZE		(_AC(1,UL) << PAGE_SHIFT)
#define PAGE_MASK		(~(PAGE_SIZE-1))

#define __VIRTUAL_MASK		((1UL << __VIRTUAL_MASK_SHIFT) - 1)

/* Cast P*D_MASK to a signed type so that it is sign-extended if
   virtual addresses are 32-bits but physical addresses are larger
   (ie, 32-bit PAE). */
#define PHYSICAL_PAGE_MASK	(((signed long)PAGE_MASK) & __PHYSICAL_MASK)
#define PHYSICAL_PMD_PAGE_MASK	(((signed long)PMD_MASK) & __PHYSICAL_MASK)
#define PHYSICAL_PUD_PAGE_MASK	(((signed long)PUD_MASK) & __PHYSICAL_MASK)

#define HPAGE_SHIFT		PMD_SHIFT
#define HPAGE_SIZE		(_AC(1,UL) << HPAGE_SHIFT)
#define HPAGE_MASK		(~(HPAGE_SIZE - 1))
#define HUGETLB_PAGE_ORDER	(HPAGE_SHIFT - PAGE_SHIFT)

#define HUGE_MAX_HSTATE 2

#define PAGE_OFFSET		((unsigned long)__PAGE_OFFSET)

#define VM_DATA_DEFAULT_FLAGS	VM_DATA_FLAGS_TSK_EXEC

#define __PHYSICAL_START	ALIGN(CONFIG_PHYSICAL_START, \
				      CONFIG_PHYSICAL_ALIGN)

#ifdef CONFIG_GEM5_KASLR_PROTECTION_HIGH
#define GEM5_KASLR_GET_DELTA_MASK ((_AC(1,ULL) << CONFIG_GEM5_KASLR_MAX_BIT) - (_AC(1,ULL) << CONFIG_GEM5_KASLR_ALIGN_BIT))
#define GEM5_KASLR_CLEAR_MASK (~GEM5_KASLR_GET_DELTA_MASK)
#define gem5_kaslr_get_delta(addr) \
	((addr & GEM5_KASLR_GET_DELTA_MASK) >> CONFIG_GEM5_KASLR_ALIGN_BIT)
#define gem5_kaslr_remove_rand_offset(addr) ((uint64_t) addr & GEM5_KASLR_CLEAR_MASK)
#endif

#ifdef CONFIG_GEM5_KASLR_MODULE_PROTECTION_HIGH
#define GEM5_KASLR_MODULE_GET_DELTA_MASK ((_AC(1,ULL) << CONFIG_GEM5_KASLR_MODULE_MAX_BIT) - (_AC(1,ULL) << CONFIG_GEM5_KASLR_MODULE_ALIGN_BIT))
#define GEM5_KASLR_MODULE_CLEAR_MASK (~GEM5_KASLR_MODULE_GET_DELTA_MASK)
#define gem5_kaslr_module_get_delta(addr) \
	((addr & GEM5_KASLR_MODULE_GET_DELTA_MASK) >> CONFIG_GEM5_KASLR_MODULE_ALIGN_BIT)
#endif

#if defined(CONFIG_GEM5_KASLR_PROTECTION_HIGH) || defined(CONFIG_GEM5_KASLR_MODULE_PROTECTION_HIGH)
/* TODO: This take bits that are used by Linux, which is not ok (especially in user space). FIXME! */
#define gem5_kaslr_get_delta_pte(delta) (((delta & 0x7f) << 52) + ((delta & 0x180) << 9))
#endif

/* TODO: Optimize the implementation here! */

#ifdef CONFIG_GEM5_KASLR_PROTECTION_HIGH
#define text_mask(addr) \
	((-(!!((addr ^ _AC(0xffffff8000000000, UL)) >> 39) | ! ((addr ^ _AC(0xffffffe000000000, UL)) >> 37))) | GEM5_KASLR_CLEAR_MASK)
#endif

#ifdef CONFIG_GEM5_KASLR_MODULE_PROTECTION_HIGH
/*#define module_mask(addr) \
	((-(!!((addr ^ MODULES_VADDR) >> 39))) | GEM5_KASLR_MODULE_CLEAR_MASK)*/
#define module_mask(addr) \
	((-(!!((addr ^ _AC(0xffffff8000000000, UL)) >> 39) | ! ((addr ^ _AC(0xffffffe000000000, UL)) >> 37))) | GEM5_KASLR_MODULE_CLEAR_MASK)
#endif

#ifdef CONFIG_GEM5_KASLR_PROTECTION_HIGH
#ifdef CONFIG_GEM5_KASLR_MODULE_PROTECTION_HIGH
/* Protect text + module */
#define gem5_kaslr_mask(addr) (((uint64_t) addr) & text_mask((uint64_t) addr) & module_mask((uint64_t) addr))
#else
/* Protect text */
#define gem5_kaslr_mask(addr) (((uint64_t) addr) & text_mask((uint64_t) addr))
#endif
#else
#ifdef CONFIG_GEM5_KASLR_MODULE_PROTECTION_HIGH
/* Protect module*/
#define gem5_kaslr_mask(addr) (((uint64_t) addr) & module_mask((uint64_t) addr))
#else
/* No KASLR protection */
#define gem5_kaslr_mask(addr) addr
#endif
#endif

#ifdef CONFIG_GEM5_ASLR_PROTECTION_HIGH
#define GEM5_ASLR_GET_DELTA_MASK ((_AC(1,ULL) << CONFIG_GEM5_ASLR_MAX_BIT) - (_AC(1,ULL) << CONFIG_GEM5_ASLR_ALIGN_BIT))
#define GEM5_ASLR_CLEAR_MASK (~GEM5_ASLR_GET_DELTA_MASK)
#define GEM5_ASLR_GET_PTE_DELTA_MASK \
    ((_AC(1,ULL) << (CONFIG_GEM5_ASLR_MAX_BIT - CONFIG_GEM5_ASLR_ALIGN_BIT + 52)) - (_AC(1,ULL) << 52))
#define gem5_aslr_offset(addr) (((uint64_t) addr) & GEM5_ASLR_GET_DELTA_MASK)
#define gem5_aslr_delta_pte_from_offset(offset) (offset << (52 - CONFIG_GEM5_ASLR_ALIGN_BIT))
#define gem5_aslr_delta_pte_from_addr(addr) (gem5_aslr_offset(addr) << (52 - CONFIG_GEM5_ASLR_ALIGN_BIT))
#define gem5_aslr_remove_rand_offset(addr) ((uint64_t) addr & GEM5_ASLR_CLEAR_MASK)
#define gem5_aslr_get_offset(delta) (gem5_aslr_offset(delta << CONFIG_GEM5_ASLR_ALIGN_BIT))
#endif

#define __START_KERNEL		(__START_KERNEL_map + __PHYSICAL_START)

#ifdef CONFIG_X86_64
#include <asm/page_64_types.h>
#define IOREMAP_MAX_ORDER       (PUD_SHIFT)
#else
#include <asm/page_32_types.h>
#define IOREMAP_MAX_ORDER       (PMD_SHIFT)
#endif	/* CONFIG_X86_64 */

#ifndef __ASSEMBLY__

#ifdef CONFIG_DYNAMIC_PHYSICAL_MASK
extern phys_addr_t physical_mask;
#define __PHYSICAL_MASK		physical_mask
#else
#define __PHYSICAL_MASK		((phys_addr_t)((1ULL << __PHYSICAL_MASK_SHIFT) - 1))
#endif

extern int devmem_is_allowed(unsigned long pagenr);

extern unsigned long max_low_pfn_mapped;
extern unsigned long max_pfn_mapped;

static inline phys_addr_t get_max_mapped(void)
{
	return (phys_addr_t)max_pfn_mapped << PAGE_SHIFT;
}

bool pfn_range_is_mapped(unsigned long start_pfn, unsigned long end_pfn);

extern void initmem_init(void);

#endif	/* !__ASSEMBLY__ */

#endif	/* _ASM_X86_PAGE_DEFS_H */
