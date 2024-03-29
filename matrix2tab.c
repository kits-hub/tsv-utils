#include "utils.h"

int matrix2tab_main(int argc, char *argv[]){
    
    if ( optind == argc || argc > optind + 1) {
        fprintf(stderr, "\nUsage: tsv-utils matrix2tab <matrix>\n\n");
        return 1;
    }

    gzFile fp;
    fp = strcmp(argv[1], "-")? gzopen(argv[1], "r") : gzdopen(fileno(stdin), "r");
    if (fp) {

        kstream_t *ks;
        ks = ks_init(fp);
        int i, *fields, m, n;
        
        kstring_t  kt  = {0, 0, 0};
        kstring_t  kv  = {0, 0, 0};
        int *elem;

        if( ks_getuntil(ks, '\n', &kt, 0) ){
            
            if(kt.s[0] == '\t') kputc('#',  &kv);
            kputs(kt.s, &kv);
            elem = ksplit(&kv, '\t', &m); 
        
        }else{
            fprintf(stderr, "[ERR]: file empty? \n");
            return 0;
        }

        int line = 0;
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){

            fields = ksplit(&kt, '\t', &n);
            for (i = line + 1; i < n; ++i){
                if(strcmp(kt.s, kv.s+elem[ i ]) != 0) printf("%s\t%s\t%s\n", kt.s, kv.s+elem[ i ], kt.s + fields[i]);
            }
            ++line;
        
        }

        if(m != line + 1) printf("[ERR]: not symmetric matrices, or format with errors!\n");

        free(kv.s);
        free(kt.s);
        ks_destroy(ks);
        gzclose(fp);
    
    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[1]);
        exit(1);
    }

    return 0;
}