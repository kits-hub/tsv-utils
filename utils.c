#include "utils.h"

void kh_reg_destroy(khash_t(reg) *h){
    
    khint_t k;
    if  (h == 0) return;
    for (k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)) {
            free((char*)kh_key(h, k));
            free((char*)kh_val(h, k));
        }
    }
    kh_destroy(reg, h);

}

void kh_kreg_destroy(khash_t(kreg) *h){
    
    khint_t k;
    if  (h == 0) return;
    for (k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)) {
            free((char*)kh_key(h, k));
            free((char*)kh_val(h, k).s);
        }
    }
    kh_destroy(kreg, h);

}

void kh_set_destroy(khash_t(set) *h){

    khint_t k;
    if (h == 0) return;
    for (k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)) free((char*)kh_key(h, k));
    }
    kh_destroy(set, h);
}

void kh_set32_destroy(khash_t(set32) *h){

    if (h == 0) return;
    kh_destroy(set32, h);
}

void kh_handle_destroy(khash_t(handle) *h){

    khint_t k;
    if  (h == 0) return;
    for (k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)){
            free( (char *)kh_key(h, k) );
            free( kh_val(h, k)->fn );
            fclose( kh_val(h, k)->fh );
            free( (handle_t *)kh_val(h, k));
        }
    }
    kh_destroy(handle, h);

}

void kh_reg32_destroy(khash_t(reg32) *h){
    
    khint_t k;
    if (h == 0) return;
    for (k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)) free((char*)kh_key(h, k));
    }
    kh_destroy(reg32, h);

}

void kh_double_destroy(khash_t(double) *h){
    
    khint_t k;
    if (h == 0) return;
    for (k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)) free((char*)kh_key(h, k));
    }
    kh_destroy(double, h);

}
