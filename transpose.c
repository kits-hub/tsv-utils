#include "utils.h"
#include "kvec.h"

int transpose_main(int argc, char *argv[]){
    
    if ( argc != 2) {
        fprintf(stderr, "\nUsage: tsv-utils transpose <tsv>\n\n");
        return 1;
    }

    kstring_t kt = {0, 0, 0};
    
    kvec_t(kstring_t) vs;
    kv_init(vs);

    int i;
    gzFile     fp;
    fp = strcmp(argv[1], "-")? gzopen(argv[1], "r") : gzdopen(fileno(stdin), "r");

    if (fp) {
           
        kstream_t *ks;
        ks = ks_init(fp);
        int *fields, n, i;
        
        if(ks_getuntil(ks, '\n', &kt, 0) >=  0 && kt.l > 0){
            
            kstring_t ktmp = {0, 0, 0};
            kputs("", &ktmp);
            if(kt.s[0] == '\t') kv_push(kstring_t, vs, ktmp);

            fields = ksplit(&kt, '\t', &n);

            for (i = 0; i < n; ++i){
                kstring_t ktmp = {0, 0, 0};
                kputs(kt.s + fields[i], &ktmp);
                kv_push(kstring_t, vs, ktmp);
            }

        }else{
            fprintf(stderr, "[ERR]: Empty file? %s\n", argv[1]);
            exit(1);
        }

        while(ks_getuntil(ks, '\n', &kt, 0) >=  0){
            if(kt.s[0] == '#') continue;
            fields = ksplit(&kt, '\t', &n);
            for (i = 0; i < n; ++i) ksprintf(&kv_A(vs, i), "\t%s", kt.s + fields[i]);
        
        }
        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[1]);
        exit(1);
    }

    for (i = 0; i < kv_size(vs); ++i){
        puts(kv_A(vs, i).s);
        free(kv_A(vs, i).s);
    }

    kv_destroy(vs);
    free(kt.s);
    return 0;
}
