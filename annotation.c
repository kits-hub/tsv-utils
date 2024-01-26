#include "utils.h"

int annotation_main(int argc, char *argv[]){
    
    int c;
    int target  = 0;
    char *place = "-";
    int R = 0;

    while ((c = getopt(argc, argv, "c:rp:")) >= 0) {
        
        if(c == 'r') R = 1;
        else if (c == 'c') target = atoi(optarg) - 1;
        else if (c == 'p') place  = optarg;


    }

    if ( optind == argc || argc != optind + 2 ) {
        
        fprintf(stderr, "\nUsage: tsv-utils annotation [options] <db> <text>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -p  STR string for missing data. default: [-]\n");
        fprintf(stderr, "  -c  INT column for annotation. default: [1]\n");
        fprintf(stderr, "  -r      remove unmapped target. default: [NOT]\n\n");
        return 1;
    
    }

    kstring_t kt    = {0, 0, 0};
    kstring_t title = {0, 0, 0};
    kstring_t aux   = {0, 0, 0};

    int *fields, i, ret, n;

    khash_t(kreg) *h;
    h = kh_init(kreg);
    khint_t k;

    kstream_t *ks;

    gzFile fp;
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r"); 
    int n_col = 0;

    if(fp){
        
        ks             = ks_init(fp);
        kstring_t kk   = {0, 0, 0};

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){

            fields = ksplit(&kt, '\t', &n_col);
            if(n_col < 2) continue;
            
            if(kt.s[0] == '#') {
                
                if(title.l > 0) continue;
                for (i = 1; i < n_col; ++i){
                    kputc('\t', &title);
                    kputs(kt.s + fields[i], &title);
                }
                continue;
            }

            k = kh_put(kreg, h, kt.s, &ret);

            kk.l = 0;
            for (i = 1; i < n_col; ++i){
                kputc('\t', &kk);
                kputs(kt.s + fields[i], &kk);
            }

            if(ret){
                kh_key(h, k)   = strdup(kt.s);
                kstring_t ktmp = {0, 0, 0};
                kputs(kk.s, &ktmp);
                kh_value(h, k) = ktmp;

            }else if(ret == 0){
                ksprintf(&kh_val(h, k), "%c", 0x01 );
                kputs(kk.s, &kh_val(h, k));                
            }
        }
        free(kk.s);
        ks_destroy(ks);
        gzclose(fp);
    
    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind]);
        exit(1);
    }

    for (i = 1; i < n_col; ++i)  ksprintf(&aux, "\t%s", place);
    
    fp = strcmp(argv[optind + 1], "-")? gzopen(argv[optind + 1], "r") : gzdopen(fileno(stdin), "r"); 
    if (fp) {
        
        ks  = ks_init(fp);
        kstring_t line  = {0, 0, 0};
        int *items;
        kstring_t kswap = {0, 0, 0};
       
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            if(kt.s[0] == '#') {
                fputs(kt.s, stdout);
                (title.l > 0) ? puts(title.s) : putchar('\n');
                continue;
            }
            
            
            line.l = 0;
            kputs(kt.s, &line);
        
            fields = ksplit(&kt, '\t', &n);
            k = kh_get(kreg, h, kt.s +  fields[ target ]);

            if( k != kh_end(h) ){
                kswap.l = 0;
                kputs(kh_value(h, k).s, &kswap);
                items = ksplit(&kswap, 0x01, &n);
                for (i = 0; i < n; ++i) printf("%s%s\n", line.s, kswap.s + items[i]);
            }else{
                if(R == 0) printf("%s%s\n", line.s, aux.s);
            }
        
        }
        
        free(line.s);
        free(kswap.s);
        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind + 1]);
        exit(1);
    }

    kh_kreg_destroy( h );
    free(kt.s);
    free(title.s);
    free(aux.s);
    return 0;
}