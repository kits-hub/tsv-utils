#include "utils.h"

#include "kvec.h"

static kvec_t( const char* ) vs;

void reshape_resize(khash_t(kreg) *h);
void reshape_print(const char* key, khash_t(kreg) *h);

int reshape_main(int argc, char *argv[]){
    
    if ( optind == argc || argc != optind + 2) {
        fprintf(stderr, "\nUsage: tsv-utils reshape <tsv> <map>\n\n");
        return 1;
    }

    khash_t(reg) *h;
    h = kh_init(reg);

    khash_t(kreg) *melt;
    melt = kh_init(kreg);

    khint_t k;
    int i, *fields, m, n, ret;
    kstream_t *ks;
    kstring_t  kt  = {0, 0, 0};

    gzFile fp;
    fp = strcmp(argv[optind+1], "-")? gzopen(argv[optind+1], "r") : gzdopen(fileno(stdin), "r");
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
                kv_push( const char *, vs, kh_key(melt, k) );
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
        kstring_t kv  = {0, 0, 0};
        int *elem;

        if( ks_getuntil(ks, '\n', &kt, 0) > 0 ){
            if(kt.s[0] == '#'){
                kputs(kt.s, &kv);
                elem = ksplit(&kv, '\t', &m);
            }else{
                fprintf(stderr, "[Warn]: No headline \n");
                return -1;
            }
        
        }else{
            fprintf(stderr, "[ERR]: file empty? \n");
            return -1;
        }

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){

            fields = ksplit(&kt, '\t', &n);
            
            for (i =1; i < n; ++i){   
                
                k = kh_get(reg, h, kv.s + elem[i]);
                if( k == kh_end(h) ){
                    fprintf(stderr, "[ERR]: %s no catalog match!\n", kv.s + elem[i]);
                    return -1;
                }

                khint_t t = kh_get(kreg, melt, kh_val(h, k));
                ksprintf(&kh_val(melt, t), "\t%s", kt.s + fields[i]);

            }
        
            reshape_print(kt.s, melt);
            reshape_resize(melt);

        }

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
    kv_destroy(vs);

    return 0;

}

void reshape_resize (khash_t(kreg) *h){
    
    khint_t k;
    for (k = 0; k < kh_end(h); ++k) 
        if (kh_exist(h, k)) kh_val(h, k).l = 0;
    
}

void reshape_print (const char* key, khash_t(kreg) *h){
    
    khint_t k;
    int i;
    for (i = 0; i < kv_size(vs); ++i) {
        k = kh_get(kreg, h, kv_A(vs, i) );
        if( k != kh_end(h) && kh_val(h, k).l > 0) printf("%s\t%s%s\n", key, kh_key(h, k), kh_val(h, k).s);
    }

}
