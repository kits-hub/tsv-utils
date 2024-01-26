#include "utils.h"
#include "kvec.h"

int reorder_main(int argc, char *argv[]){
 
    int T = 0;
    int c;
    while ((c = getopt(argc, argv, "t:")) >= 0) {
        if (c == 't') T = atoi(optarg) - 1;
    }

    if ( optind == argc || argc != optind + 2) {
        fprintf(stderr, "\nUsage: tsv-utils reorder [options] <tsv> <list>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -t INT Target in fields to reorder, default: [1]\n\n");
        exit(1);
    }

    khash_t(set) *h;
    h = kh_init(set);
    khint_t k;

    int *fields, i, absent, n;

    kstream_t  *ks;
    kstring_t  kt  = {0, 0, 0};

    kvec_t( const char* ) vs;
    kv_init(vs);
    
    gzFile     fp; 
    fp = strcmp(argv[ optind + 1], "-")? gzopen(argv[optind + 1], "r") : gzdopen(fileno(stdin), "r"); 
    if( fp ){
        ks = ks_init(fp);
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            if( kt.l == 0 ) continue;            
            k = kh_put(set, h, kt.s, &absent);
            kh_key(h, k) = strdup(kt.s);
            kv_push( const char *, vs, kh_key(h, k) );
        }
        ks_destroy(ks);
        gzclose(fp);
    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind + 1]);
        exit(1);
    }
    
    if(kh_size(h) == 0) return 0;
    
    khash_t(reg) *map;
    map = kh_init(reg);

    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r"); 
    if( fp ){
        ks  = ks_init(fp);
        kstring_t  kk  = {0, 0, 0};
        
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            if( kt.l == 0 ) continue;
            if(kt.s[0] == '#') {
                puts(kt.s); continue;
            }
            
            kk.l = 0;
            kputs(kt.s, &kk);

            fields = ksplit(&kt, '\t', &n);
            k = kh_get(set, h, kt.s);
            if( k == kh_end(h) ) continue;

            k = kh_put(reg, map, kt.s + fields[T], &absent);
            kh_key(map, k) = strdup(kt.s + fields[T]);
            kh_val(map, k) = strdup(kk.s);
        }
        
        free(kk.s);
        gzclose(fp);
        ks_destroy(ks);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind]);
        exit(1);
    }

    for (i = 0; i < kv_size(vs); ++i) {
        k = kh_get(reg, map, kv_A(vs, i));
        if( k != kh_end(map) ) puts(kh_val(map, k));
    }    

    for (k = 0; k < kh_end(h); ++k)
        if (kh_exist(h, k)) free((char*)kh_key(h, k));
    
    free(kt.s);
    
    kv_destroy(vs);
    kh_destroy(set, h);
    kh_reg_destroy(map);
    return 0;
}
