#include "utils.h"
#include "ksort.h"

typedef struct {
    double   v;
    char    *s; 
} node_t;

KHASH_MAP_INIT_INT(ksort, node_t *)
static khash_t(ksort)  *h;

#define khint_lt(a, b) (kh_val(h, (a))->v > kh_val(h, (b))->v)
KSORT_INIT(ksort, khint_t, khint_lt)

void kh_ksort_destroy(khash_t(ksort) *h){

    khint_t k;
    if  (h == 0) return;
    for (k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)){
            free( kh_val(h, k)->s );
            free( (node_t *)kh_val(h, k));
        }
    }
    kh_destroy(ksort, h);

}

int ksort_main (int argc, char *argv[]){
    
    if ( optind == argc || argc != optind + 2) {
        
        fprintf(stderr, "\nUsage: tsv-utils ksort <tsv> <collum>\n\n");
        return 1;
    
    }
    
    int collum = atoi(argv[optind + 1]) - 1;
    khint_t k;

    gzFile     fp;
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r");

    kstream_t *ks;
    h = kh_init(ksort);
    int num = 0;

    ks = ks_init(fp);   
    if (fp) {

        kstring_t  kswap  = {0, 0, 0};
        kstring_t  kt     = {0, 0, 0};
        int *fields, n, ret;

        ks = ks_init(fp);

        while( ks_getuntil(ks, '\n', &kt, 0) >=  0 ){
            
            if(kt.s[0] == '#'){
                printf("%s\n", kt.s);
                continue;
            }

            kswap.l = 0;
            kputs(kt.s, &kswap);
            fields =  ksplit(&kt, '\t', &n);

            if( n <= collum){
                fprintf(stderr, "[ERR]: not enough collums %d > %d ;\n",  n, collum + 1);
                exit(-1);
            }

            k = kh_put(ksort, h, num, &ret);
            if(ret){

                kh_key(h, k) = num;
                node_t *node = (node_t *)malloc(sizeof(node_t));
                node->v = atof(kt.s + fields[collum]);
                node->s = strdup(kswap.s);
                kh_val(h, k) = node; 
                
            }
            ++num;
        
        }

        free(kswap.s);
        free(kt.s);
        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind]);
        exit(1);
    }

    khint_t kv[num];
    int i = 0;
    for (k = 0; k < kh_end(h); ++k){
        if (kh_exist(h, k)) kv[i++] = k;
    }

    ks_mergesort(ksort, num, kv, 0);
    for (i = 0; i < num; ++i) puts(kh_val(h, kv[i])->s);

    kh_ksort_destroy(h);
    return 0;
}