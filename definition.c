#include "utils.h"

typedef struct {
    char  sep;
    int   col;
    char  *del;
    char  *title;
    char  *placehold;
} opt_t;

inline void def_opt_init(opt_t *opt){
    memset(opt, 0, sizeof(opt_t));
    opt->sep       = ',';
    opt->del       = "\t";
    opt->col       = 0;
    opt->title     = NULL;
    opt->placehold = NULL;
}

int definition_main(int argc, char *argv[]){
    
    opt_t opt;
    def_opt_init(&opt);
    int c;
    while ((c = getopt(argc, argv, "c:d:t:p:s:")) >= 0) {
        if (c == 'c') opt.col = atoi(optarg) - 1;
        else if (c == 't') opt.title = optarg;
        else if (c == 'p') opt.placehold = optarg;
        else if (c == 's') opt.sep   = optarg[0];
        else if (c == 'd'){
            if( strlen(optarg) == 0 ){
                fprintf(stderr, "error: not support delimitor == 0 [%s]\n", optarg);
                return 0;
            }
            opt.del = optarg;
        }
    }

    if ( optind == argc || argc != optind + 2) {
        
        fprintf(stderr, "\n");
        fprintf(stderr, "Usage: tsv-utils definition [options] <db> <tab>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -c INT     target col for annotation, default: [1]\n");
        fprintf(stderr, "  -d char*   delimitor between key and definition, default: ['\t']\n");
        fprintf(stderr, "  -s char    delimitor for duplicated values elements in db, default, [',']\n");
        fprintf(stderr, "  -t char*   title name, default: 'definition'\n");
        fprintf(stderr, "  -p char*   placehold, default: [-]\n");
        fprintf(stderr, "\n");

        return 1;
    }

    kstream_t   *ks;
    
    khash_t(kreg) *h;
    h  = kh_init(kreg);
    
    khint_t k;
    kstring_t kt = {0, 0, 0};

    gzFile     fp;
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r"); 
    
    int tab = ( strcmp(opt.del, "\t") == 0) ? 1 : 0;

    if (fp) {
        
        int *fields, n, ret;
        ks = ks_init(fp);

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            fields = ksplit(&kt, '\t', &n);
            if(n != 2 ) continue;
            k = kh_put(kreg, h, kt.s, &ret);

            if(ret){               
                
                kh_key(h, k)   = strdup(kt.s);                
                kstring_t ktmp = {0, 0, 0};
                kputs(kt.s + fields[1], &ktmp);
                kh_val(h, k) = ktmp;            }
            
            else if(ret == 0){

                kputc(opt.sep, &kh_val(h, k));
                kputs(kt.s + fields[1], &kh_val(h, k));
            
             }
        }

        ks_destroy(ks);
        gzclose(fp);
    
    }else{
        fprintf(stderr, "[ERR]: %s :file streaming error\n", argv[optind]);
        exit(-1);
    }

    fp = strcmp(argv[optind + 1], "-")? gzopen(argv[optind + 1], "r") : gzdopen(fileno(stdin), "r");  
    if (fp) {

        int *fields, n, i;
        ks = ks_init(fp);
        kstring_t kv = {0, 0, 0};

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            kv.l   = 0;
            fields = ksplit(&kt, '\t', &n);

            if(opt.col >= n){
                fprintf(stderr, "[ERR]: specified target column [%d] exceed filelds: [%d] \n", opt.col + 1, n);
                return 0;
            }

            for (i = 0; i <= opt.col; ++i){
                kputc('\t', &kv);
                kputs(kt.s + fields[i], &kv);
            }

            if(kt.s[0] == '#'){
                
                if(tab){
                   if(opt.title == NULL){
                        fprintf(stderr, "[WARN]: Not specified title name , using 'definition'!\n");
                        ksprintf(&kv, "\tdefinition");
                   }else{
                        ksprintf(&kv, "\t%s", opt.title);
                   }
                }
            }else{             
                k = kh_get(kreg, h, kt.s +  fields[ opt.col ]);           
                if(k != kh_end(h)){
                    kputs(opt.del, &kv);
                    kputs(kh_val(h, k).s, &kv);
                }else{
                    if(tab) ksprintf(&kv, "\t-");
                }    
            }

            for (i = opt.col + 1; i < n; ++i){
                kputc('\t', &kv);
                kputs(kt.s + fields[i], &kv);
            }
            puts(kv.s + 1);

        }

        free(kv.s);
        ks_destroy(ks);
        gzclose(fp);
    }else{
        fprintf(stderr, "[ERR]: %s file streaming error\n", argv[optind + 1]);
        exit(-1);
    }

    kh_kreg_destroy( h );
    free(kt.s);
    return 0;
}

