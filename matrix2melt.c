#include "utils.h"

void matrix_melt(khash_t(kreg) *h);

int matrix2melt_main(int argc, char *argv[]){
    
    if ( optind == argc || argc != optind + 2) {
        fprintf(stderr, "\nUsage: tsv-utils matrix2melt <matrix> <map>\n\n");
        return 1;
    }

    khash_t(reg) *h;
    h = kh_init(reg);

    khash_t(kreg) *melt;
    melt = kh_init(kreg);

    khint_t k,t;
    int i, *fields, m, n, ret;
    kstream_t *ks;
    kstring_t  kt  = {0, 0, 0};

    gzFile fp;
    fp = strcmp(argv[optind + 1], "-")? gzopen(argv[optind + 1], "r") : gzdopen(fileno(stdin), "r");
    if (fp) {

        ks     = ks_init(fp);
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){

            if(kt.s[0] == '#') continue;
            
            fields = ksplit(&kt, '\t', &n);    
            k = kh_put(reg, h, kt.s, &ret);
            kh_key(h, k) = strdup(kt.s);
            kh_val(h, k) = strdup(kt.s + fields[1]);
     
            k = kh_put(kreg, melt, kt.s + fields[1], &ret);
            if(ret == 1){
                kh_key(melt, k) = strdup(kt.s + fields[1]);
                kstring_t ktmp={0, 0, 0};
                kh_val(melt, k) = ktmp;
            }

        }
        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind + 1]);
        exit(1);
    }

    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r");
    if (fp) {

        ks = ks_init(fp);       
        kstring_t  kv  = {0, 0, 0};
        int *elem;

        if( ks_getuntil(ks, '\n', &kt, 0) ){
            
            if(kt.s[0] == '\t') kputc('#',  &kv);
            kputs(kt.s, &kv);
            elem = ksplit(&kv, '\t', &m); 
        
        }else{
            fprintf(stderr, "[ERR]: file empty? \n");
            return 0;
        }

        int line = 0;
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){

            fields = ksplit(&kt, '\t', &n);
            for (i = line + 2; i < n; ++i){

                k = kh_get(reg, h, kt.s);
                t = kh_get(reg, h, kv.s + elem[i]);
                if(k == kh_end(h) || t == kh_end(h)){
                    fprintf(stderr, "[ERR]: %s or %s no catalog match!\n", kt.s, kv.s + elem[i]);
                    return -1;
                }

                if(strcmp( kh_val(h, k), kh_val(h, t) ) == 0){
                    khint_t z = kh_get(kreg, melt, kh_val(h, k));
                    ksprintf(&kh_val(melt, z), "\t%s", kt.s + fields[i]);
                }

            }
            ++line;
        }

        if(m != line + 1) fprintf(stderr, "[ERR]: not symmetric matrices, or format with errors!\n");
        matrix_melt(melt);

        free(kv.s);
        ks_destroy(ks);
        gzclose(fp);
    
    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind]);
        exit(1);
    }

    free(kt.s);
    kh_reg_destroy(h);
    kh_kreg_destroy(melt);

    return 0;

}

void matrix_melt (khash_t(kreg) *h){
    
    khint_t k;
    for (k = 0; k < kh_end(h); ++k)
        if (kh_exist(h, k)) 
            if(kh_val(h, k).l > 0) printf("%s%s\n", kh_key(h, k), kh_val(h, k).s);
    
}
