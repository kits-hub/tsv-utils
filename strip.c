#include "utils.h"

int strip_main(int argc, char *argv[]){
    
    if ( optind == argc || argc != optind + 1) {

        fprintf(stderr, "\nUsage: tsv-utils strip <tsv>\n\n");
        return 1;
    
    }  


    kstring_t kt = {0, 0, 0};

    gzFile     fp;
    fp = strcmp(argv[ optind ], "-")? gzopen(argv[ optind ], "r") : gzdopen(fileno(stdin), "r");

    if (fp) {
           
        kstream_t *ks;
        ks = ks_init(fp);

        int i, n;

        while(ks_getuntil(ks, '\n', &kt, 0) >=  0){

            for (i = 0, n = 0; i < kt.l; ++i){
              if(kt.s[i] == ' ')
                ++n;
              else break;
            }
   
            char *kend;
            kend = kt.s + kt.l - 1;
            while (kend > kt.s && isspace(*kend)) kend = kend - 1;

            *(kend + 1) = '\0';

            puts(kt.s + n);

        }
        
        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[ optind  ]);
        exit(1);
    }

    free(kt.s);
    return 0;
}
