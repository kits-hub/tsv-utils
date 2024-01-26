#include "utils.h"

int nfilter_main(int argc, char *argv[]){
    
    int c;
    int L     = 0;
    int field = 1;

    while ((c = getopt(argc, argv, "lf:")) >= 0) {
        if (c == 'l')  L   = 1;
        else if (c == 'f')  field = atoi(optarg) - 1;
    }

    if ( optind == argc || argc != optind + 2) {

        fprintf(stderr, "\nUsage: tsv-utils nfilter <tsv> <value>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "   -f  INT    fileld for compare. default: [2].\n");
        fprintf(stderr, "   -l  FLAG   print line if: value larger than specified fields.\n\n");
        return 1;
    
    }  

    gzFile     fp;
    fp = strcmp(argv[ optind ], "-")? gzopen(argv[ optind ], "r") : gzdopen(fileno(stdin), "r");

    if (fp) {
           
        kstream_t *ks;
        ks           = ks_init(fp);

        kstring_t kt = {0, 0, 0};
        double value = atof( argv[optind + 1] );
        char *p;
        int   k;

        while(ks_getuntil(ks, '\n', &kt, 0) >=  0){
            
            if(kt.s[0] == '#'){
                puts(kt.s);
                continue;
            };

            if(kt.l == 0) continue;
              
            ks_tokaux_t aux;
            for (p = kstrtok(kt.s, "\t", &aux), k = 0; p; p = kstrtok(0, 0, &aux), ++k){
                
                if(k == field){

                    double q = atof(p);
                    if(L == 1){
                        if(q >= value ) puts(kt.s);
                    }else{
                        if(q <= value ) puts(kt.s);
                    }

                    break;
                }
            }
        
        }
        
        ks_destroy(ks);
        gzclose(fp);
        free(kt.s);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[ optind  ]);
        exit(1);
    }

    return 0;
}
