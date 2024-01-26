#include "utils.h"

int accumulation_main(int argc, char const *argv[]){

    if(argc != 3){
        fprintf(stderr, "\nUsage: tsv-utils accumulation <bins> <key/val>\n\n");
        exit(1);
    }
        

    khash_t(double) *db;
    db = kh_init(double);
    khint_t  k;    

    int *fields, ret, n, i;
    kstream_t *ks;

    kstring_t kt = {0, 0, 0};

    gzFile fp;
    fp = strcmp(argv[2], "-" ) ? gzopen(argv[2], "r") : gzdopen(fileno(stdin), "r");
  
    if(fp){

        ks = ks_init(fp);       
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            fields = ksplit(&kt, '\t',  &n);
            k = kh_put(double, db, kt.s + fields[0], &ret);
            if( k != kh_end(db) ){
                
                kh_key(db, k) = strdup( kt.s + fields[0]);
                kh_val(db, k) = atof(kt.s + fields[1]);
            
            }

        }
        ks_destroy(ks);
        gzclose(fp);
    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[2]);
        exit(1);
    }

    fp = strcmp(argv[1], "-" )  ? gzopen(argv[1], "r") : gzdopen(fileno(stdin), "r");
    if(fp){

        kstring_t kv = {0, 0, 0};
        ks = ks_init(fp);       
        double val = 0.0;

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            val = 0.0;
            if(kt.s[0] == '#') continue;
            
            fields = ksplit(&kt, '\t',  &n);
            kv.l   = 0;
            ksprintf(&kv, kt.s + fields[1]);
            fields = ksplit(&kv, ',',   &n);
            
            for (i = 0; i < n; ++i){
                k  = kh_get(double, db, kv.s + fields[i]);
                if(  k != kh_end(db) ) val += kh_val(db, k);
            }

            printf("%s\t%.4g\n", kt.s, val);
        }
        free(kv.s);
        ks_destroy(ks);
        gzclose(fp);

    }
    
    kh_double_destroy(db);
    free(kt.s);
    return 0;
}
