#include "utils.h"

typedef struct {
    int   col;
    char  type;
    int   key;
} opt_t;

void opt_init(opt_t *opt);

int subset_main(int argc, char *argv[]){
    
    opt_t opt;
    opt_init(&opt);
    int c;
    while ((c = getopt(argc, argv, "c:rk")) >= 0) {

        if (c == 'c')   opt.col  = atoi(optarg) - 1;
        else if(c=='r') opt.type = 0;
        else if(c=='k') opt.key  = 1;

    }

    if ( optind == argc || argc != optind + 2) {
        fprintf(stderr, "\nUsage: tsv-utils subset [options] <tsv> <list>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -c INT   target col for select, default: 1\n");
        fprintf(stderr, "  -r       subset option: -r for supplementary set, default: intersection.\n");
        fprintf(stderr, "  -k       query using key id.\n\n");
        return 1;
    }

    khash_t(set) *h;
    h = kh_init(set);
    
    khint_t k;

    kstream_t  *ks;
    kstring_t   kt  = {0, 0, 0};
    int *fields, i, absent, n;
    gzFile     fp;

    if(opt.key == 1){
     
        kputs(argv[optind + 1], &kt);
        fields = ksplit(&kt, ',', &n);
        for (i = 0; i < n; ++i){
            k = kh_put(set, h, kt.s + fields[i], &absent);
            if(absent == 1){
               kh_key(h, k) = strdup( kt.s + fields[i] );
            }else if(absent == 0){
               fprintf(stderr, "[WARN]: ID %s repeat, will ignor.\n", kt.s + fields[i]);
            }
        }

    }else{
        fp = strcmp(argv[optind + 1], "-")? gzopen(argv[optind + 1], "r") : gzdopen(fileno(stdin), "r");  
        if(fp){         
            ks = ks_init(fp);
            while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
                if( kt.l == 0 ) continue;
                k = kh_put(set, h, kt.s, &absent);                
                if(absent) kh_key(h, k) = strdup(kt.s);

            }
            ks_destroy(ks);
            gzclose(fp);
        
        }else{
            fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind + 1]);
            exit(1);
        }
    }

    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r");    
    if(fp){

        ks             = ks_init(fp);
        kstring_t  kk  = {0, 0, 0};
        int  i;
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            if(kt.l == 0) continue;
            if(kt.s[0] == '#') {         
                puts(kt.s);
                continue;     
            }

            kk.l = 0;
            kputs(kt.s, &kk);

            ks_tokaux_t aux;
            char *p;
            for (p = kstrtok(kt.s, "\t", &aux), i = 0; p; p = kstrtok(0, 0, &aux), ++i){
                *(char*)aux.p = 0;
                if(i == opt.col){
                     k = kh_get(set, h, p);
                     if(k != kh_end(h) &&  opt.type == 1){                     
                         puts(kk.s);
                     }else if(k == kh_end(h) && opt.type == 0){
                         puts(kk.s);
                     }            
                }
            }
        }
        free(kk.s);
        ks_destroy(ks);
        gzclose(fp);
    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind]);
        exit(1);        
    }
    
    free(kt.s);
    kh_set_destroy(h);
    return 0;
}

void opt_init(opt_t *opt){
    memset(opt, 0, sizeof(opt_t));
    opt->col    = 0;
    opt->type   = 1;
    opt->key    = 0;
}