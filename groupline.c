#include "utils.h"

int groupline_main(int argc, char *argv[]){
    
    int R = 0;
    int c;
    while ((c = getopt(argc, argv, "r")) >= 0) {       
        if (c == 'r')  R = 1;
    }

    if ( optind == argc || argc != optind + 2 ) {
        
        fprintf(stderr, "\nUsage: tsv-utils groupline [options] <db> <text>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, " -r  replace headline [default: add headline].\n\n");
        return 1;
    
    }

    kstring_t kt    = {0, 0, 0};
    kstring_t title = {0, 0, 0};
    kstring_t aux   = {0, 0, 0};

    int *fields, i, ret, n;

    khash_t(reg) *h;
    h = kh_init(reg);
    khint_t k;

    kstream_t *ks;

    gzFile fp;
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r"); 

    if(fp){
        
        ks           = ks_init(fp);
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){

            if(kt.s[0] == '#' || kt.l == 0) continue;

            fields = ksplit(&kt, '\t', &n);
            k = kh_put(reg, h, kt.s, &ret);
            if(ret){
                kh_key(h, k)   = strdup(kt.s);
                kh_value(h, k) = strdup(kt.s + fields[n - 1]);
            }

        }
        ks_destroy(ks);
        gzclose(fp);
    
    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind]);
        exit(1);
    }
    
    fp = strcmp(argv[optind + 1], "-")? gzopen(argv[optind + 1], "r") : gzdopen(fileno(stdin), "r"); 
    if (fp) {
        
        ks  = ks_init(fp);

        if( ks_getuntil( ks, '\n', &kt, 0) >=  0 ){
             
             kputs(kt.s, &title);
             fields = ksplit(&kt, '\t', &n);
             ksprintf(&aux, "#group");
             for (i = 1; i < n; ++i){
                 
                 k = kh_get(reg, h, kt.s + fields[i]);
                 if(k == kh_end(h)){
                    fprintf(stderr, "[ERR]: can't map %s to a catalog.\n", kt.s + fields[i]);
                    exit(1);
                 }else ksprintf(&aux, "\t%s", kh_val(h, k));

             }

             printf("%s\n", aux.s);
             if(!R){
                if(title.s[0] != '#') fputc('#', stdout);
                printf("%s\n", title.s);
             }

        }else{
            fprintf(stderr, "[ERR]: empty file %s\n", argv[optind + 1]);
            exit(1);
        }

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0 ) puts(kt.s);

        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind + 1]);
        exit(1);
    }

    kh_reg_destroy( h );
    free(kt.s);
    free(title.s);
    free(aux.s);
    return 0;
}