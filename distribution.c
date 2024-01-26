#include "utils.h"
#include "kvec.h"

typedef struct{
    char   *feature;
    double value;
} DIST;

static kvec_t( const char* ) head;

KHASH_MAP_INIT_STR(dist, DIST *)

void kh_dist_destroy(khash_t(dist) *h){
    
    khint_t k;
    if (h == 0) return;
    for (k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)) {
            free((char*)kh_key(h, k));
            free((char*)kh_val(h, k)->feature);
        }
    }
    kh_destroy(dist, h);
}

void dist_head_print(){
    
    if (kv_size(head) == 0) return;
    int i;
    printf("#labels");
    for (i = 0; i < kv_size(head); ++i) printf("\t%s", kv_A(head, i));
    putchar('\n');

}

void dist_dv_print (int n, double *dv){int i;
    for (i = 0; i < n; ++i) printf("\t%lf", dv[i]);
    putchar('\n');
}

void dist_dv_zero (int n, double *dv){
    int i;
    for (i = 0; i < n; ++i) dv[i] = 0.0;
}

double dist_dv_sum (int n, double *dv){
    int i;
    double t = 0;
    for (i = 0; i < n; ++i) t += dv[i];
    return t;
}

/*distribution*/

int distribution_main(int argc, char const *argv[]){

    if(argc != 3){
        fprintf(stderr, "\n\nUsage: tsv-utils distribution <bins>  <abundance>\n\n");
        exit(1);
    }

    khash_t(dist) *db;
    db    = kh_init(dist);

    khash_t(reg32) *feature;
    feature = kh_init(reg32);
    
    int *fields, ret, n, i, inc;
    inc = 0; 

    kstring_t kt = {0, 0, 0};
    khint_t  k, t;

    gzFile fp;
    fp = strcmp(argv[2], "-" ) ? gzopen(argv[2], "r") : gzdopen(fileno(stdin), "r");
    kstream_t *ks;

    if(fp){
        
        ks = ks_init(fp);
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            if(kt.s[0] == '#') continue;

            fields = ksplit(&kt, '\t',  &n);
            if(strcmp(kt.s + fields[1], "-") == 0) continue;

            k = kh_put(dist, db, kt.s, &ret);
            
            DIST *node   = (DIST*)malloc( sizeof(DIST) );
            if( node != NULL ){
                
                kh_key(db, k)  = strdup( kt.s);
                node->feature  = strdup( kt.s + fields[1] );
                node->value    = atof( kt.s + fields[2] );
                kh_val(db, k)  = node;
            }

        }
        ks_destroy(ks);
        gzclose(fp);
    }else{
    
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[2]);
        exit(1);
    
    }

    fp = strcmp(argv[1], "-" ) ? gzopen(argv[1], "r") : gzdopen(fileno(stdin), "r");
    if(fp){

        kv_init(head);
        ks = ks_init(fp);
        kstring_t kv = {0, 0, 0};

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            if(kt.s[0] == '#') continue;
            kv.l = 0;
            fields = ksplit(&kt, '\t',  &n);

            if(n != 2){
                fprintf(stderr, "[ERR]: bin file need two column, key/members, and sperated using ',' \n" );
                exit(-1);
            }

            ksprintf(&kv, kt.s + fields[1]);

            fields = ksplit(&kv, ',', &n);
            for (i = 0; i < n; ++i){
                k  = kh_get(dist, db, kv.s + fields[i]);
                if(  k != kh_end(db) ){
                     t = kh_get(reg32, feature, kh_val(db, k)->feature);
                     if( t == kh_end(feature) ){
                         
                         t = kh_put(reg32, feature, kh_val(db, k)->feature, &ret);
                         kh_key(feature, t) = strdup( kh_val(db, k)->feature);
                         kh_val(feature, t) = inc++;
                         kv_push( const char *, head, kh_key(feature, t) );
                        
                     }
                }
                
            }

        }

        dist_head_print();
        gzrewind(fp);
        ks = ks_init(fp);

        int size     = kh_size(feature);
        double *dv   = (double *) malloc(size * sizeof(double));

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            if(kt.s[0] == '#') continue;
            fields = ksplit(&kt, '\t',  &n);
            kv.l = 0;
            ksprintf(&kv, kt.s + fields[1]);
            fields = ksplit(&kv, ',', &n);
            dist_dv_zero(size, dv);
            
            for (i = 0; i < n; ++i){
                k  = kh_get(dist, db, kv.s + fields[i]);
                if(  k != kh_end(db) ){
                     t = kh_get(reg32, feature, strdup( kh_val(db, k)->feature));
                     if( t != kh_end(feature) ) dv[ kh_val(feature, t) ] += kh_val(db, k)->value;
                }
            }

            fputs(kt.s, stdout);
            dist_dv_print(size, dv);
        }

        free(dv);
        ks_destroy(ks);
        gzclose(fp);
        free(kv.s);

    }else{
    
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[1]);
        exit(1);
    
    }
    
    kh_dist_destroy(db);
    kh_reg32_destroy(feature);
    kv_destroy(head);

    free(kt.s);
    return 0;
}
