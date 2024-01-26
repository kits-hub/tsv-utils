#include "utils.h"

KHASH_MAP_INIT_STR(map, kstring_t)
#include "kvec.h"

typedef struct {
    char *keys;
    char *titles;
    int   target;
    char *placehold;
    int   head_line;
    char *delimiter;
} opt_t;
void agg_opt_init(opt_t *opt);

static kvec_t( const char* ) vs;
static kvec_t(khash_t(map)*) repository;
static khash_t(set) *h;

void agg_print_title( opt_t *opt, int n_fields,  kstring_t *kt);
void agg_print_values( opt_t *opt, int optind, int argc );

int agg_main(int argc, char *argv[]){
    
    opt_t opt;
    agg_opt_init(&opt);
    int c;
    while ((c = getopt(argc, argv, "k:t:c:p:d:i")) >= 0) {
        
        if (c == 'k') {
             opt.keys = optarg;
        }else if (c == 't') {
             opt.titles = optarg;
        }else if (c == 'c') {
             opt.target = atoi(optarg) - 1;
        }else if (c == 'i') {
             opt.head_line = 1;
        }else if (c == 'p') {
             opt.placehold = optarg;
        }else if (c == 'd') {
             opt.delimiter = optarg;
        }
    }
    if ( optind == argc || argc < optind + 1) {
        fprintf(stderr, "\nUsage: tsv-utils agg [options] [label:text ...]\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -k STR  the keys fields pattern: 1:2:3, default: [1];\n");
        fprintf(stderr, "  -t STR  the titles for keys: key_1:key_2:key_3, default: [catalog];\n");
        fprintf(stderr, "  -c INT  the target column default: [2];\n");
        fprintf(stderr, "  -p CHAR placehold for missing value: default ['0'];\n");
        fprintf(stderr, "  -d STR  delimiter between same key, default: [\\cA]\n");
        fprintf(stderr, "  -i      ignore the head line;\n\n");
        return 1;
    }

    //construct key vector
    kvec_t(int) keys;
    kv_init(keys);

    int i, j, *fields, n_fields, n;
    kstring_t  kt  = {0, 0, 0};
    n_fields = 0;

    if( opt.keys == NULL)
        kv_push(int, keys, 0);
    else{
        kputs(opt.keys, &kt);
        fields = ksplit(&kt, ':',  &n);
        n_fields = n - 1;
        for (i = 0; i < n; ++i) kv_push(int, keys, atoi(kt.s + fields[i]) - 1);
    }

    // initialize containors
    kv_init(repository);
    kv_init(vs);
    h = kh_init(set);
    
    khint_t k;
    kstring_t   kk = {0, 0, 0};
    gzFile fp;

    for (i = optind; i < argc; ++i){

        ks_tokaux_t aux;  char *p;
        p = kstrtok(argv[i], ":", &aux);
        argv[i][ aux.p - p] = '\0';

        kputc('\t',    &kk);
        kputs(argv[i], &kk);

        fp = strcmp(aux.p + 1, "-")? gzopen(aux.p + 1, "r") : gzdopen(fileno(stdin), "r");
        if (fp) {
            
            khash_t(map) *map;
            map = kh_init(map);
            kv_push(  khash_t(map)*,  repository, map);

            kstream_t *ks;
            ks = ks_init(fp);
            kstring_t   kv = {0, 0, 0};

            int ret;

            if(opt.head_line) ks_getuntil( ks, '\n', &kt, 0);
            
            while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
                
                if(kt.s[0] == '#') continue; 
                
                fields  = ksplit(&kt, '\t', &n);
                
                // key values
                kv.l    = 0;
                for (j = 0; j <= n_fields;  ++j){
                    kputs(kt.s + fields[ kv_A(keys, j)], &kv);
                    kputc('\t', &kv);
                }
                kv.s[ kv.l - 1 ] = '\0';

                k = kh_get(set, h, kv.s);
                if(k == kh_end(h)){
                    khint_t   t   = kh_put( set, h, kv.s,  &ret);
                    kh_key(h, t)  = strdup( kv.s);
                    kv_push( const char *, vs, kh_key(h, t) );
                }

                k = kh_get(map, kv_A(repository, i - optind), kv.s);
                if(k == kh_end(map)){
                    khint_t   t   = kh_put( map, kv_A(repository, i - optind), kv.s,  &ret);
                    kh_key(kv_A(repository, i - optind), t)  = strdup( kv.s);
                    kstring_t k_tmp = {0, 0, 0};
                    kputs(kt.s + fields[ opt.target ], &k_tmp); 
                    kh_val(kv_A(repository, i - optind), t)  = k_tmp; 
                }else{
                    if(opt.delimiter == NULL)
                       ksprintf( &kh_val( kv_A(repository, i - optind), k ), "%c", 0x01); 
                    else
                       ksprintf( &kh_val( kv_A(repository, i - optind), k ), opt.delimiter); 

                    kputs(kt.s + fields[ opt.target ],  &kh_val( kv_A(repository, i - optind), k ));
                }

             }
             
             ks_destroy(ks);
             gzclose(fp);
             free(kv.s);

        }else{
            fprintf(stderr, "[ERR]: can't open file %s\n", aux.p + 1);
            exit(1);
        }
    }

    agg_print_title(&opt, n_fields, &kk);
    agg_print_values(&opt, optind, argc);
  
    for (i = optind; i < argc; ++i) kh_destroy(map, kv_A(repository, i - optind));

    free(kt.s);
    free(kk.s);

    kv_destroy(repository);
    kv_destroy(keys);
    kh_destroy(set, h);
    kv_destroy(vs);
    return 0;
}

void agg_opt_init(opt_t *opt){
    
    memset(opt, 0, sizeof(opt_t));
    opt->keys      = NULL;
    opt->titles    = NULL;
    opt->target    = 1;
    opt->placehold = "0";
    opt->head_line = 0;
    opt->delimiter = NULL;

}

void agg_print_title( opt_t *opt, int n_fields,  kstring_t *kt){

    int i;
    fputs("#", stdout);
    if(opt->titles == NULL){
        (n_fields == 0) ? fputs("catalog", stdout) :
                          fputs("key_1", stdout);

        for (i = 1; i <= n_fields; ++i) printf("\tkey_%d", i + 1 );
    }else{
        for (i = 0; i <= strlen(opt->titles) ; ++i)
            if(opt->titles[i] == ':') opt->titles[i] = '\t';
        printf("%s",  opt->titles);
    }
    printf("%s\n",  kt->s);

}

void agg_print_values( opt_t *opt, int optind, int argc ){

    int i, j;
    khint_t k;
    for (i = 0; i < kv_size(vs); ++i) {
         k = kh_get(set, h, kv_A(vs, i));
         if( k != kh_end(h) ){
            printf("%s", kh_key(h, k));
            for (j = optind; j < argc; ++j){
                khint_t t = kh_get(map, kv_A(repository, j - optind), kh_key(h, k));
                if( t != kh_end( kv_A(repository, j - optind) ) ){
                    printf("\t%s",  kh_val(kv_A(repository, j - optind), t).s);
                    free(  (char*)kh_key(kv_A(repository, j - optind), t) );
                    free(  (char*)kh_val(kv_A(repository, j - optind), t).s );
                }else{
                    printf("\t%s", opt->placehold);
                }
            }
            putchar('\n');
            free( (char*)kh_key(h, k));
         }
    }
}
