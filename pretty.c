#include "utils.h"
#include "kvec.h"

int pretty_main(int argc, char *argv[]){
    
    if ( argc != 2) {        
        fprintf(stderr, "\nUsage: tsv-utils pretty <tsv> \n\n");
        return 1;
    }

    kstring_t kt    = {0, 0, 0};

    kvec_t(int) width;
    kv_init(width);

    int *fields, n, i;
    gzFile     fp;
    fp = strcmp(argv[1], "-")? gzopen(argv[1], "r") : gzdopen(fileno(stdin), "r");

    if (fp) {
        
        kstream_t *ks;
        ks = ks_init(fp);

        if(ks_getuntil(ks, '\n', &kt, 0) >=  0){
             fields = ksplit(&kt, '\t', &n);          
             for (i = 0; i < n; ++i) kv_push(int, width, strlen(kt.s + fields[i]) );
        }else{
            fprintf(stderr, "[ERR]: empty file. !\n");
            exit(1);
        }

        int len = 0;
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            len = 0;
            fields = ksplit(&kt, '\t', &n);
            for (i = 0; i < n; ++i){
                len = strlen(kt.s + fields[i]);
                if( kv_A(width, i ) < len ) kv_a(int, width, i )  = len;
            }

        }

        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[1]);
        exit(1);
    }

    fp = strcmp(argv[1], "-")? gzopen(argv[1], "r") : gzdopen(fileno(stdin), "r");
    if (fp) {
        
        kstream_t *ks;
        ks = ks_init(fp);

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){

            fields = ksplit(&kt, '\t', &n);
            printf("%-*s", kv_A(width, 0),  kt.s);
            for (i = 1; i < n; ++i) printf("\t%-*s", kv_A(width, i),  kt.s + fields[i]);
            printf("\n");

        }

        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[1]);
        exit(1);
    }

    kv_destroy(width);
    free(kt.s);
    return 0;

}