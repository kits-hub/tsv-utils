#include "utils.h"

int select_main(int argc, char *argv[]){
    
    int c;
    int field  = 0;
    int delet  = 0;

    while ((c = getopt(argc, argv, "f:d")) >= 0) {
        if (c == 'f')  field = atoi(optarg);
        else if (c == 'd')  delet = 1;

    }

    if ( optind == argc || argc != optind + 2) {

        fprintf(stderr, "\nUsage: tsv-utils select <tsv> <string>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "   -f  INT    fileld for compare. default: [1].\n");
        fprintf(stderr, "   -d         delet matched lines.\n\n");
        return 1;
    
    }

    gzFile     fp;
    fp = strcmp(argv[ optind ], "-")? gzopen(argv[ optind ], "r") : gzdopen(fileno(stdin), "r");

    if (fp) {
           
        kstream_t *ks;
        ks           = ks_init(fp);

        kstring_t kt = {0, 0, 0};
        char *p;
        int   l;
        int   k;
        int   status = -1;
        int   q      = strlen(argv[optind + 1]);
        
        int n;
        if( ks_getuntil(ks, '\n', &kt, 0) >=  0 ){
            ksplit(&kt, '\t', &n);
            field  = ( field < 0 ) ? (n + field) : (field - 1);
        } 

        gzrewind(fp);
        ks_rewind(ks);

        while(ks_getuntil(ks, '\n', &kt, 0) >=  0){
            
            if(kt.s[0] == '#'){
                puts(kt.s);
                continue;
            };

            if(kt.l == 0) continue;

            ks_tokaux_t aux;
            for (p = kstrtok(kt.s, "\t", &aux), k = 0; p; p = kstrtok(0, 0, &aux), ++k){
                
                if(k == field){
                   l = aux.p - p;
                   
                   if(q != l)
                      status = 0;
                   else
                      status = ( strncmp(argv[optind + 1], p, l) == 0)  ? 1 : 0;
            
                   break;
                }
            }

            if( status ){
              if(!delet) puts(kt.s);
            }else{
              if(delet) puts(kt.s);
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
