#include "utils.h"

typedef struct{
    kstring_t  vals;
    int        counts;
} Bin;

KHASH_MAP_INIT_STR(bin, Bin)

void bin_print(khash_t(bin) *h);
void kh_bin_destroy(khash_t(bin) *h);

static char *label     = "catalog";

int bins_main(int argc, char *argv[]){
    
    int   target    = 1;
    int   summary   = 0;
    char *delimiter = ",";
    int c;
    while ((c = getopt(argc, argv, "d:t:s:l:")) >= 0) {
        
        if (c == 't') target = atoi(optarg) - 1;
        else if (c == 's') summary   = atoi(optarg) - 1;
        else if (c == 'd') delimiter = optarg;
        else if (c == 'l') label = optarg;
    }

    if ( optind == argc || argc != optind + 1) {
        
        fprintf(stderr, "\n");
        fprintf(stderr, "Usage: tsv-utils bins  [options]  [table ...]\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -t INT  target to uniq and counts, default: [2]\n");
        fprintf(stderr, "  -s INT  target to summary, default: [1]\n");
        fprintf(stderr, "  -d STR  delimiter between elements, default: [,]\n");
        fprintf(stderr, "  -l STR  relabel target head title. default: [catalog]\n");
        fprintf(stderr, "\n");

        return 1;
    }

    khash_t(bin) *h;
    h = kh_init(bin);

    khash_t(set) *set;
    set = kh_init(set);

    khint_t k;
    int  *fields, i, n, ret;

    kstream_t *ks;
    kstring_t  kt  = {0, 0, 0};
    kstring_t  kv  = {0, 0, 0};

    for (i = optind; i < argc; ++i){

        gzFile     fp;
        fp = strcmp(argv[i], "-")? gzopen(argv[i], "r") : gzdopen(fileno(stdin), "r");
        if (fp) {

            ks = ks_init(fp);
            while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
                
                if(kt.l == 0 || kt.s[0] == '#') continue;
                fields  = ksplit(&kt, '\t', &n);
                
                kv.l = 0;
                ksprintf(&kv, "%s\t%s", kt.s + fields[summary], kt.s + fields[target]);
                k = kh_get(set, set, kv.s);

                if(k == kh_end(set)){
                    k  = kh_put( set, set, kv.s, &ret);
                    kh_key(set, k) = strdup(kv.s);
                }else continue;
                
                
                k   = kh_put(bin, h, kt.s +  fields[target], &ret);
                
                if(ret){

                    Bin bin;
                    kstring_t ktmp  = {0, 0, 0};
                    kputs(kt.s +  fields[summary], &ktmp);
                    bin.vals   = ktmp;
                    bin.counts = 1;
                    
                    kh_key(h, k) = strdup( kt.s +  fields[target] );
                    kh_val(h, k) = bin;
                
                }else if(ret == 0){
                    
                    kh_val(h, k).counts++;
                    kputs(delimiter , &kh_val(h, k).vals);
                    kputs(kt.s +  fields[summary], &kh_val(h, k).vals);
                
                }
             }
             ks_destroy(ks);
             gzclose(fp);
        }else{
            fprintf(stderr, "[ERR]: can't open file %s\n", argv[i]);
            exit(1);
        }
    }

    bin_print(h);
    kh_bin_destroy(h);
    kh_set_destroy(set);

    return 0;
}

void bin_print(khash_t(bin) *h){

    printf("#%s\tnumber\tmembers\n", label);
    khint_t k;
    for (k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)) {
            printf("%s\t%d\t%s\n", kh_key(h, k), kh_val(h, k).counts, kh_val(h, k).vals.s);
        }
    }
}

void kh_bin_destroy(khash_t(bin) *h){

    khint_t k;
    if (h == 0) return;
    for (k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)) {
            free((char*)kh_key(h, k));
            free(kh_val(h, k).vals.s);
        }
    }
    kh_destroy(bin, h);
}