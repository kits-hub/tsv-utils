#include "utils.h"
#include "kvec.h"
#include <math.h>

int norm_main(int argc, char *argv[]){

    int rnd = 0;
    int c;
    while ((c = getopt(argc, argv, "r")) >= 0) {
         if (c == 'r') rnd   = 1;

    }

    if ( optind == argc || argc != optind + 2) { 
        fprintf(stderr, "\nUsage: tsv-utils norm  [options] <tsv> <factor: 30000>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "   -r   Round value to Int.\n\n");
        return 1;
    }

    double factor = atof(argv[optind + 1]);

    kstring_t kt    = {0, 0, 0};

    kvec_t(double) sum;
    kv_init(sum);

    kvec_t( char* ) vs;
    kv_init(vs);


    int *fields, n, i;
    gzFile     fp;
    fp = strcmp(argv[ optind ], "-")? gzopen(argv[ optind ], "r") : gzdopen(fileno(stdin), "r");

    if (fp) {
        
        kstream_t *ks;
        ks = ks_init(fp);

        if(ks_getuntil(ks, '\n', &kt, 0) >=  0){
            if(kt.s[0] == '#'){
                
                char *p = strdup(kt.s);
                kv_push(char *, vs, p);

                fields = ksplit(&kt, '\t', &n);
                for (i = 1; i < n; ++i) kv_push(double, sum,  0 );
     
            }else{
                fprintf(stderr, "[ERR]: No headline with '#'!\n");
                exit(1);
            }
        }else{
            fprintf(stderr, "[ERR]: empty file. !\n");
            exit(1);
        }

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
   
            char *p = strdup(kt.s);
            kv_push(char *, vs, p);
            fields = ksplit(&kt, '\t', &n);

            for (i = 1; i < n; ++i) kv_a(double, sum, i - 1 ) +=  atof(kt.s + fields[i]);

        }

        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[ optind ]);
        exit(1);
    }

    printf("%s\n", kv_A(vs, 0)); free(kv_A(vs, 0));
    double val = 1.0;
    for (i = 1; i < kv_size(vs); ++i){
         
         kt.l = 0;
         kputs(kv_A(vs, i), &kt);
         fields = ksplit(&kt, '\t', &n);
         printf("%s", kt.s);

         int j;
         for (j = 1; j < n; ++j){
             
             val = (double)(atof(kt.s + fields[j])  * factor )/ kv_A(sum, j - 1);
             if(rnd) fprintf(stdout, "\t%d", (int)round(val));
             else fprintf(stdout, "\t%lf",  val);
         }
         
         fputc('\n', stdout);
         free( kv_A(vs, i) );

    }

    kv_destroy(sum);
    kv_destroy(vs);
    free(kt.s);
    return 0;

}