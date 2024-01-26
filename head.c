#include "utils.h"

int head_main(int argc, char *argv[]){
    
    if ( optind == argc || argc != optind + 1) {
        fprintf(stderr, "\nUsage: tsv-utils head <tsv>\n\n");
        return 1;
    }  

    gzFile     fp;
    fp = strcmp(argv[ optind ], "-")? gzopen(argv[ optind ], "r") : gzdopen(fileno(stdin), "r");

    if (fp) {
        
        kstring_t kt    = {0,0,0};    

        kstream_t *ks;
        ks = ks_init(fp);
        int i = 0;
        ks_tokaux_t aux;
        char *p;

        if(ks_getuntil(ks, '\n', &kt, 0) >=  0 && kt.l > 0){
        
            int k;
            for (p = kstrtok(kt.s, "\t", &aux), k = 0; p; p = kstrtok(0, 0, &aux), ++k){
                kt.s[aux.p - kt.s] = '\0';
                printf("#%d.\t%s\n", ++i,  p);  
            }
        
        }

        free(kt.s);
        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[ optind  ]);
        exit(1);
    }

    return 0;
}
