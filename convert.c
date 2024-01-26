#include <ctype.h>
#include "utils.h"

int convert_main(int argc, char *argv[]){
    
    int u = 0;
    int f = 1;

    int c;
    while ((c = getopt(argc, argv, "f:u")) >= 0) {
        
        if (c == 'f')      f    = atoi(optarg) - 1;
        else if (c == 'u') u    = 1;

    }

    if ( optind == argc || argc != optind + 1) {
       
        fprintf(stderr, "Usage: tsv-utils convert  [options]  <tsv>\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -f INT   field to convert, default: [60]\n");
        fprintf(stderr, "  -u       to uppercase, default: [lowercase]\n\n");
        return 1;
    }

    gzFile     fp;
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r");
    if(fp){
        
        kstream_t  *ks;
        ks           = ks_init(fp);
        kstring_t kt = {0, 0, 0};

        char *p;
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            
            if(kt.s[0] == '#'){
                printf("%s\n", kt.s);
                continue;
            }
            
            ks_tokaux_t aux;
            int    k                = 0;

            for (p = kstrtok(kt.s, "\t", &aux), k = 0; p; p = kstrtok(0, 0, &aux), ++k){
                
                if(k == f)
                    for ( ; p < aux.p; ++p){
                        (u) ? (*p = toupper(*p)) :  (*p = tolower(*p));
                    }
            }

            printf("%s\n", kt.s);

        }

        free(kt.s);
        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[2]);
        exit(1);
    }

    return 0;
}