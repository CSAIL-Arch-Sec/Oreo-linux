## Note for protect user ASLR

Functions to check:
* `access_ok` does not need mask
* `find_vma`, `find_vma_intersection`, `vma_lookup`, `find_vma_prev` sometimes take masked address, so we cannot check delta in them.

Functions that use `untagged_addr`
* `strncpy_from_user`, `strnlen_user`
* `mincore`
* `gup` TODO!!!
* `msync`
* `munmap`
* `mempolicy` three cases done
* `mlock` `do_mlock`
* `mremap`
* `mprotect`
