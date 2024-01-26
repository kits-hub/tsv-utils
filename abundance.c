#include "utils.h"
#include "kvec.h"

int abundance_main(int argc, char *argv[]){
    
    int d = 1;
    
    int c;
    while ((c = getopt(argc, argv, "k:")) >= 0) {
        if (c == 'k') d = atoi(optarg);
    }

    if ( optind == argc || argc != optind + 1) {       
    
        fprintf(stderr, "\nUsage:  tsv-utils abundance [optind]  <tsv>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -k INT  First |INT| number of fields as main keys. default: [1];\n\n");
        return 1;
    
    }

    kstring_t kt    = {0, 0, 0};
    kstring_t title = {0, 0, 0};
    
    int *fields, n, i;

    kvec_t(double) sum;
    kv_init(sum);

    kvec_t( char* ) vs;
    kv_init(vs);

    gzFile     fp;
    fp = strcmp(argv[ optind ], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r");
    
    if (fp) {
        
        kstream_t *ks;
        ks = ks_init(fp);

        if( ks_getuntil(ks, '\n', &kt, 0) >=  0 ){
            
            if(kt.s[0] == '#'){
                
                kputs(kt.s, &title);
                fields  = ksplit(&kt, '\t', &n);
                n -= d;
                if(n <= 0){
                    fprintf(stderr, "[ERR]: NO enough data fields.\n");
                    exit(-1);
                }
                for (i = d; i < n; ++i) kv_push(double, sum, 0 );

            }else{
                fprintf(stderr, "[ERR]: first line not start with '#' \n %s\n", kt.s);
                exit(-1);
            }
        }

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            kv_push(char *, vs, strdup(kt.s));
            fields = ksplit(&kt, '\t', &n);
            for (i = d; i < n; ++i) kv_a(double, sum, i - 1 ) +=  atof(kt.s + fields[i]);
        
        }
        
        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind]);
        exit(1);
    }

    printf("%s\n", title.s);
    for (i = 0; i < kv_size(vs); ++i){
        kt.l = 0;
        kputs(kv_A(vs, i), &kt);
        fields = ksplit(&kt, '\t', &n);
        int j;
        fputs(kt.s, stdout);
        for (j = 1; j < d; ++j) printf("\t%s", kt.s + fields[j]);
        for (j = d; j < n; ++j) fprintf(stdout, "\t%.4g",  atof(kt.s + fields[j]) * 100 / kv_A(sum, j - 1) );
        fputc('\n', stdout);
    
    }

    kv_destroy(vs);
    kv_destroy(sum);
    free(kt.s);

    return 0;
}