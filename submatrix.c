#include "utils.h"

int submatrix_main(int argc, char *argv[]){

    int distmx = 0;
    int c;
    while ((c = getopt(argc, argv, "d")) >= 0) {   
        if (c == 'd') distmx = 1;
    }

    if ( optind == argc || argc != optind + 2) {
        fprintf(stderr, "\nUsage:tsv-utils submatrix  [options]  <matrix> <samples>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -d    Export distance matrix format. \n\n");
        return 1;
    }

    khash_t(set) *h;
    h = kh_init(set);

    khash_t(set32) *map;
    map = kh_init(set32);

    kstream_t *ks;
    kstring_t  kt    = {0, 0, 0};
    khint_t k;
    int ret;
    
    gzFile fp;
    fp = strcmp(argv[optind  +  1], "-")? gzopen(argv[optind  +  1], "r") : gzdopen(fileno(stdin), "r");
    if (fp) {

        ks = ks_init(fp);
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){            
            k = kh_put(set, h, kt.s, &ret);
            kh_key(h, k) = strdup(kt.s);
        }
        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind  +  1]);
        exit(1);
    }

    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r");
    if (fp) {
        
        ks = ks_init(fp);
        int i, *fields, n;       
        kstring_t kv  = {0, 0, 0};

        if( ks_getuntil(ks, '\n', &kt, 0) ){

            kstring_t titles = {0, 0, 0};
            if(kt.s[0] == '\t') kputc('#',  &titles);
            kputs(kt.s, &titles);
            fields = ksplit(&titles, '\t', &n);
            for (i = 1; i < n; ++i){

                k = kh_get(set, h, titles.s + fields[i]);
                if(k == kh_end(h)) continue;
                kputc('\t', &kv);
                kputs(titles.s + fields[i], &kv);

                k = kh_put(set32, map, i, &ret);
                kh_key(map, k) = i;
            
            }

            if(kh_size(map) == 0){
                fprintf(stderr, "[ERR]: no sample match in titles line.\n");
                return 0;
            }

            if(!distmx){
               if(kt.s[0] != '\t') printf("%s", titles.s);
            }
            puts(kv.s);
            free(titles.s);

        }else{
            fprintf(stderr, "[ERR]: file empty? \n");
            return 0;
        }

        int line = 0;
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            ++line;
            fields = ksplit(&kt, '\t', &n);
            kv.l = 0;
            k = kh_get(set32, map, line);
            if(k ==kh_end(map)) continue;
           
            kputs(kt.s , &kv);
            for (i = 1; i < n; ++i){
                k = kh_get(set32, map, i);
                if(k != kh_end(map)) ksprintf(&kv, "\t%s", kt.s + fields[i]);
            }          
            puts(kv.s);

        }

        free(kv.s);
        ks_destroy(ks);
        gzclose(fp);
    
    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind]);
        exit(1);
    }

    
    free(kt.s);

    kh_set_destroy(h);
    kh_set32_destroy(map);

    return 0;
}