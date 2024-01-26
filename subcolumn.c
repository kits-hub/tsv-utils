#include "utils.h"

int subcolumn_main(int argc, char *argv[]){
    
    int key   = 0;
    int order = 0;

    int c;
    while ((c = getopt(argc, argv, "kr")) >= 0) {
        if(c =='k') key  = 1;
        else if(c =='r') order  = 1;
    }

    if ( optind == argc || argc != optind + 2) {
        fprintf(stderr, "\n\nUsage:tsv-utils subcolumn [options] <tsv> <id|list>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -k       query using key id.\n");
        fprintf(stderr, "  -r       reorder by specified id order.\n\n");
        return 1;
    }

    khash_t(reg32) *h;
    h = kh_init(reg32);

    khash_t(set) *set;
    set = kh_init(set);


    kstream_t *ks;
    kstring_t  kt    = {0, 0, 0};
    khint_t k;
    int ncols  = 0;
    int i, *fields, n, ret;

    gzFile fp;
    if(key == 1){        
        
        kputs(argv[optind + 1], &kt);
        fields = ksplit(&kt, ',', &n);
        for (i = 0; i < n; ++i){
            k = kh_put(reg32, h, kt.s + fields[i], &ret);
            if(ret == 1){
               kh_key(h, k) = strdup( kt.s + fields[i] );
               kh_val(h, k) = ncols++;
            }else if(ret == 0){
               fprintf(stderr, "[WARN]: ID %s repeat, will ignor.\n", kt.s + fields[i]);
            }
        }

    }else{

        fp = strcmp(argv[optind + 1], "-")? gzopen(argv[optind + 1], "r") : gzdopen(fileno(stdin), "r");
        if (fp) {
            ks = ks_init(fp);            
            while( ks_getuntil( ks, '\n', &kt, 0) >=  0){            
                
                k = kh_put(reg32, h, kt.s, &ret);
                if(ret == 1){
                   kh_key(h, k) = strdup( kt.s );
                   kh_val(h, k) = ncols++;
                }else if(ret == 0){
                   fprintf(stderr, "[WARN]: ID %s repeat, will ignor.\n", kt.s);
                }
            
            }
            ks_destroy(ks);
            gzclose(fp);

        }else{
            fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind + 1]);
            exit(1);
        }

    }

    int location[ncols];
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r");
    if (fp) {
        
        ks = ks_init(fp);
        kstring_t kv  = {0, 0, 0};
        int m = 0;
        if( ks_getuntil(ks, '\n', &kt, 0) ){

            memset(location, 0, sizeof(location[0]) * ncols );
            kstring_t titles = {0, 0, 0};
            if(kt.s[0] == '\t') kputc('#',  &titles);
            kputs(kt.s, &titles);
            fields = ksplit(&titles, '\t', &n);

            for (i = 0; i < n; ++i){

                k = kh_get(reg32, h, titles.s + fields[i]);
                if( k != kh_end(h) ){

                    if(order){
                       location[ kh_val(h, k) ] = i;
                    }else{
                       location[m] = i;
                    }
                    m++;

                    k = kh_put(set, set, titles.s + fields[i], &ret);
                    if(ret == 1){
                        kh_key(set, k) = strdup( titles.s + fields[i] );
                    }else if(ret == 0){
                        fprintf(stderr, "[ERR]: ID %s repeat, will exit.\n", titles.s + fields[i]);
                        exit(-1);
                    }
                }
            }

            if(m == 0){
                fprintf(stderr, "[ERR]: no sample match in titles line.\n");
                return 0;
            }

            for (i = 0; i < m; ++i) ksprintf(&kv, "\t%s", titles.s + fields[ location[i] ]);

            puts(kv.s + 1);
            free(titles.s);
           
        }else{
            fprintf(stderr, "[ERR]: file empty? \n");
            return 0;
        }

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){

            fields = ksplit(&kt, '\t', &n);
            kv.l = 0;
            for (i = 0; i < m; ++i) ksprintf(&kv, "\t%s", kt.s + fields[ location[i] ]);
            puts(kv.s + 1);

        }

        free(kv.s);
        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind]);
        exit(1);
    }

    free(kt.s);

    kh_reg32_destroy(h);
    kh_set_destroy(set);

    return 0;
}