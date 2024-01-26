#include "utils.h"

int skip_main(int argc, char *argv[]){
    
    int c;
    char *pat = "#";

    while ((c = getopt(argc, argv, "p:")) >= 0) {
        if (c == 'p')  pat   = optarg;
    }

    if ( optind == argc || argc != optind + 1) {

        fprintf(stderr, "\nUsage: tsv-utils skip [options] <tsv>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "   -p STR  drop lines start with \"STR\", skip first line. [#] \n");
        return 1;
    
    }  

    kstring_t kt = {0, 0, 0};
    kstring_t kz = {0, 0, 0};
    int skip, len, *fields, n;
    
    skip = 0;

    gzFile fp;
    fp = strcmp(argv[ optind ], "-")? gzopen(argv[ optind ], "r") : gzdopen(fileno(stdin), "r");

    if (fp) {
           
        kstream_t *ks;
        ks = ks_init(fp);

        while(ks_getuntil(ks, '\n', &kt, 0) >=  0){
           
            kz.l = 0;
            kputs(kt.s, &kz);
            
            fields = ksplit(&kt, '\t', &n);
            len = (strcmp("#", pat) == 0) ? 1 : fields[1];

            if(strncmp(kt.s, pat, len) == 0){
                if( !skip ){
                    printf("%s\n", kz.s);
                    ++skip;
                }
                continue;
            }
            printf("%s\n", kz.s);
        }

        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[ optind  ]);
        exit(1);
    }

    free(kt.s);
    free(kz.s);
    return 0;
}
