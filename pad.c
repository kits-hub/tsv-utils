#include "utils.h"
#include <math.h>

int string_fmt (char * str, kstring_t *kt) {

       char *ret, *s = str;
       int  *prep = 0;
       char *pat  = "\\t";
       int   n = 1;

       while( (ret = kstrstr(s, pat, &prep)) != 0){
            
            kputsn(s, ret - s, kt);
            kputc('\t', kt);
            s = ret + prep[0];
            ++n;

       }

       kputs(s, kt);
       return n;
}

int pad_main(int argc, char *argv[]){
    
    char *label = "pad";
    int loc = 1;
    int c;
    while ((c = getopt(argc, argv, "l:r")) >= 0) {
        if (c == 'l')       label   = optarg;
        else if (c == 'r')  loc     = 0;
    }

    if ( optind == argc || argc != optind + 2) {

        fprintf(stderr, "\nUsage: tsv-utils pad [options] <tsv> <string>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "   -l STR label for headline. default [pad]\n");
        fprintf(stderr, "   -r     pad a string at end of line. default [front]\n\n");
        return 1;
    
    }  


    kstring_t kv   = {0, 0, 0};   
    kstring_t kt   = {0, 0, 0};
    kstring_t head = {0, 0, 0};

    int num = string_fmt( argv[optind + 1], &kv);
    
    int i;
    for (i = 0; i < num; ++i){
        kputc('\t',  &head);
        kputs(label, &head);
    }

    gzFile     fp;
    fp = strcmp(argv[ optind ], "-")? gzopen(argv[ optind ], "r") : gzdopen(fileno(stdin), "r");

    if (fp) {
           
        kstream_t *ks;
        ks = ks_init(fp);

        while(ks_getuntil(ks, '\n', &kt, 0) >=  0){
             
             if(kt.l ==  0) continue;

             if(kt.s[0] == '#'){
                
                if(loc){
                    printf("#%s\t%s\n", head.s + 1, kt.s + 1);
                }else{
                    printf("%s%s\n", kt.s, head.s);
                }

            }else{           
            
                if(loc) 
                    printf("%s\t%s\n", kv.s, kt.s);
                else
                    printf("%s\t%s\n", kt.s, kv.s);
            }
        
        }
        
        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[ optind  ]);
        exit(1);
    }

    free(kv.s);
    free(kt.s);
    return 0;
}

