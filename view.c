#include "utils.h"
#include <math.h>

int view_main(int argc, char *argv[]){
    
    int c;
    int comment = 0;
    int blank   = 0;
    int R       = 0;
    int line    = 0;
    int drop    = 0;

    while ((c = getopt(argc, argv, "cbrld")) >= 0) {
        if (c == 'c')  comment   = 1;
        else if (c == 'b') blank = 1;
        else if (c == 'r') R     = 1;
        else if (c == 'l') line  = 1;
        else if (c == 'd') drop  = 1;

    }

    if ( optind == argc || argc != optind + 1) {

        fprintf(stderr, "\nUsage: tsv-utils view [options] <tsv>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "   -c   Add / Remove comment char '#' to the first line. \n");
        fprintf(stderr, "   -d   Drop lines start with '#', skip first line. \n");
        fprintf(stderr, "   -b   Remove the blank lines.\n");
        fprintf(stderr, "   -r   Round value to Int.\n");
        fprintf(stderr, "   -l   Add line number.\n\n");
        return 1;
    
    }  

    kstring_t kt = {0, 0, 0};
    kstring_t kv = {0, 0, 0};

    gzFile     fp;
    fp = strcmp(argv[ optind ], "-")? gzopen(argv[ optind ], "r") : gzdopen(fileno(stdin), "r");

    int l = 0;

    if (fp) {
           
        kstream_t *ks;
        ks = ks_init(fp);
        int i, *fields, n;

        if(ks_getuntil(ks, '\n', &kt, 0) >=  0 && kt.l > 0){
            
            if(line) printf("%d\t", ++l);
            if(comment)
                ( kt.s[0] != '#') ? printf("#%s\n", kt.s) : puts(kt.s + 1);
            else puts(kt.s);
        
        }

        while(ks_getuntil(ks, '\n', &kt, 0) >=  0){
            
            if(drop  && kt.s[0] == '#') continue;
            if(blank && kt.l == 0) continue;
            
            if(line) printf("%d\t", ++l);
            if(R){
                
                fields = ksplit(&kt, '\t', &n);
                kv.l   = 0;
                kputs(kt.s, &kv);
                for (i = 1; i < n; ++i) ksprintf(&kv, "\t%d", (int)round(atof(kt.s + fields[i])) );
                puts(kv.s);

            }else puts(kt.s);
        
        }
        
        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[ optind  ]);
        exit(1);
    }

    free(kt.s);
    free(kv.s);
    return 0;
}
