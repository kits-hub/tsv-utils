#include <stdio.h>
#include <string.h>
#include <zlib.h>
#include <stdlib.h>

#include "khash.h"
#include "kstring.h"

#include "kseq.h"
KSTREAM_INIT(gzFile, gzread, 16384)

typedef struct{
    FILE *fh;
    char *fn;
} handle_t;

KHASH_MAP_INIT_STR(reg, char*)
void kh_reg_destroy(khash_t(reg) *h);

KHASH_MAP_INIT_STR(reg32, int)
void kh_reg32_destroy(khash_t(reg32) *h);

KHASH_MAP_INIT_STR(double, double)
void kh_double_destroy(khash_t(double) *h);

KHASH_MAP_INIT_STR(kreg, kstring_t)
void kh_kreg_destroy(khash_t(kreg) *h);

KHASH_SET_INIT_STR(set)
void kh_set_destroy(khash_t(set) *h);

KHASH_SET_INIT_INT(set32)
void kh_set32_destroy(khash_t(set32) *h);

KHASH_MAP_INIT_STR(handle, handle_t *)
void kh_handle_destroy(khash_t(handle) *h);