#include "utils.h"
#include "kvec.h"

KHASH_MAP_INIT_STR(map, kstring_t)
static khash_t(set) *h;
static kvec_t( const char* ) vs;
static kvec_t(khash_t(map)*) repository;
static kvec_t(int) sizes;
static char  placehold = '-';
static char *label = "catalog";

void print_values(kstring_t *title, int optind, int argc);

int join_main(int argc, char *argv[]){
    
    int warn  = 1;
   
    int c;
    while ((c = getopt(argc, argv, "p:l:i")) >= 0) {
        if (c == 'p') placehold = optarg[0];
        else if (c == 'l') label = optarg;
        else if (c == 'i') warn = 0;

    }

    if ( optind == argc || argc < optind + 1) {
        fprintf(stderr, "\nUsage: tsv-utils join [options] [text ...]\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -p CHAR  placehold for missing value, default: ['-']\n");
        fprintf(stderr, "  -l STR   label for feature, default['catalog']\n");
        fprintf(stderr, "  -i       ignore duplicates id warn.;\n\n");
        return 1;
    }

    h = kh_init(set);
    kv_init(sizes);
    kv_init(repository);
    kv_init(vs);

    kstring_t  title = {0, 0, 0};
    
    khint_t k;
    int i, j;

    gzFile fp;
    for (i = optind; i < argc; ++i){
    
        kstring_t   kv = {0, 0, 0};
        kstring_t   kt = {0, 0, 0};

        fp = strcmp(argv[i], "-")? gzopen(argv[i], "r") : gzdopen(fileno(stdin), "r");
        if (fp) {
            
            khash_t(map) *map;
            map = kh_init(map);
            kv_push(  khash_t(map)*,  repository, map);

            kstream_t *ks;
            ks = ks_init(fp);

            int  *fields, n, ret;

            if(ks_getuntil( ks, '\n', &kt, 0) >=  0){

                if(kt.s[0] == '#'){
                   
                    kv.l  = 0;
                    fields = ksplit(&kt, '\t', &n);
                    for (j = 1; j < n; ++j){
                        kputc('\t', &kv);
                        kputs(kt.s + fields[j], &kv);
                    }
                    kputs(kv.s, &title);
                    kv_push(int, sizes, n - 1 );
                
                }else{
                    fprintf(stderr, "[ERR]: Please provide title line start with '#', %s ...\n" , argv[i]);
                    exit(-1);
                }

            }

            while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
                
                fields  = ksplit(&kt, '\t', &n);

                kv.l  = 0;
                for (j = 1; j < n; ++j){
                    kputc('\t', &kv);
                    kputs(kt.s + fields[j], &kv);
                }

                k = kh_get(set, h, kt.s);
                if(k == kh_end(h)){
                    khint_t   t   = kh_put( set, h, kt.s,  &ret);
                    kh_key(h, t)  = strdup( kt.s);
                    kv_push( const char *, vs, kh_key(h, t) );
                }

                k = kh_get(map, kv_A(repository, i - optind), kt.s);
                if(k == kh_end(map)){
                    
                    khint_t   t   = kh_put( map, kv_A(repository, i - optind), kt.s,  &ret);
                    kh_key(kv_A(repository, i - optind), t)  = strdup( kt.s );
                    kstring_t k_tmp = {0, 0, 0};
                    kputs(kv.s, &k_tmp); 
                    kh_val(kv_A(repository, i - optind), t)  = k_tmp;

                }else{
                    if( warn ) fprintf(stderr, "[WARN]: key [%s] duplicates in file %s, ignore ...\n", kt.s , argv[i]);
                }

             }
             
             ks_destroy(ks);
             gzclose(fp);

        }else{
            fprintf(stderr, "[ERR]: can't open file %s\n", argv[i]);
            exit(1);
        }
  
        free(kt.s);
        free(kv.s);
    }

    print_values(&title, optind, argc);

    for (i = optind; i < argc; ++i) kh_destroy(map, kv_A(repository, i - optind));

    free(title.s);

    kv_destroy(repository);
    kv_destroy(sizes);
    kh_destroy(set, h);
    kv_destroy(vs);
    return 0;
}

void print_values(kstring_t *title, int optind, int argc){

    int i, j;
    kstring_t kt = {0, 0, 0};

    if(title->l != 0) printf("#%s%s\n", label, title->s);
    
    for (i = 0; i < kv_size(vs); ++i) {
         khint_t k;
         k = kh_get(set, h, kv_A(vs, i));
         if( k != kh_end(h) ){
            printf("%s", kh_key(h, k));
            for (j = optind; j < argc; ++j){
                int index = j - optind;
                khint_t t = kh_get(map, kv_A(repository, index), kh_key(h, k));
                if( t != kh_end( kv_A(repository, index) ) ){
                    printf("%s",  kh_val(kv_A(repository, index), t).s);
                    free(  (char*)kh_key(kv_A(repository, index), t) );
                    free(  (char*)kh_val(kv_A(repository, index), t).s );
                }else{
                    
                    kt.l   = 0;
                    int  k = 0;
                    for (k = 0; k < kv_A(sizes, index); ++k){
                        kputc('\t', &kt);
                        kputc(placehold, &kt);
                    }
                    printf("%s", kt.s);
                
                }
            }
            putchar('\n');
            free( (char*)kh_key(h, k));
         }
    }

    free(kt.s);

}