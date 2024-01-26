#include "utils.h"
#include "kvec.h"

typedef struct {
    int   counts;
    char *fields;
    int   ignore;
} opt_t;

void uniq_opt_init(opt_t *opt);
void uniq_buckets (khash_t(reg32) *h,  opt_t *opt);

int uniq_main (int argc, char *argv[]){
    
    opt_t opt;
    uniq_opt_init(&opt);
    int c;
    while ((c = getopt(argc, argv, "f:icv")) >= 0) {
        
        if (c == 'c') {
            opt.counts = 1;
        }if (c == 'i') {
            opt.ignore = 1;
        }else if (c == 'f') {
            opt.fields = optarg;
        }

    }

    if ( optind == argc || argc != optind + 1) {
        
        fprintf(stderr, "\nUsage: tsv-utils uniq [options] <text>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -f STR fields pattern: 1:2:3, [1];\n");
        fprintf(stderr, "  -c     display the counts;\n");
        fprintf(stderr, "  -i     flag to ignore lines start with '#';\n\n");
        return 1;
    
    }

    khash_t(reg32) *h;
    h = kh_init(reg32);
    khint_t k;
 
    kvec_t(int) vt;
    kv_init(vt);

    int i, *fields, n_fields, n;
    kstring_t  kt  = {0, 0, 0};
    n_fields = 0;

    if( opt.fields == NULL)
        kv_push(int, vt, 0);
    else{
        kputs(opt.fields, &kt);
        fields = ksplit(&kt, ':',  &n);
        n_fields = n - 1;
        for (i = 0; i < n; ++i) kv_push(int, vt, atoi(kt.s + fields[i]) - 1);
    }

    gzFile fp;
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r"); 

    if(fp){

        kstream_t *ks;
        ks = ks_init(fp);
        kstring_t  kv  = {0, 0, 0};
        
        int absent;

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
           
           if( kt.l == 0 ) continue;          
           if( kt.s[0] == '#' && opt.ignore == 1 ) continue;

           fields = ksplit(&kt, '\t' , &n);
           if(kv_A(vt, n_fields ) > n - 1){
              fprintf(stderr, "[ERR]: specified column value exceed the max fields !\n");
              break;
           }

           kv.l = 0;
           kputs(kt.s + fields[kv_A(vt, 0)], &kv);

           for (i = 1; i <= n_fields; ++i){
               kputc('\t', &kv);
               kputs(kt.s + fields[kv_A(vt, i)], &kv);
           }

           k = kh_put(reg32, h, kv.s, &absent);
           
           if(absent) {
               kh_key(h, k)  = strdup(kv.s);
               kh_val(h, k) = 1;
           }else kh_val(h, k)++;
        
        }

        free(kv.s);
        ks_destroy(ks);
        gzclose(fp);
        uniq_buckets(h, &opt);
    }
    
    free(kt.s);
    kh_reg32_destroy(h);
    return 0;
}

void uniq_opt_init(opt_t *opt){
    
    memset(opt, 0, sizeof(opt_t));
    opt->counts  = 0;
    opt->fields  = NULL;
    opt->ignore  = 0;

}

void uniq_buckets  (khash_t(reg32) *h,  opt_t *opt){
    
    khint_t k;
    if (h == 0) return;
    for (k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)) {
           (opt->counts == 1) ? printf("%s\t%d\n", kh_key(h, k),  kh_val(h, k) ) :
                                puts(kh_key(h, k)); 
        }
    }

}