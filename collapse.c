#include "utils.h"

int collapse_main(int argc, char *argv[]){
 
    char *field = NULL;
    int  c;
    char  delim = ' ';
    while ((c = getopt(argc, argv, "d:f:")) >= 0) {
        if (c == 'f') field = optarg;
        else if (c == 'd') delim = optarg[0];
    }

    if ( optind == argc || argc != optind + 1) {
        fprintf(stderr, "\nUsage: tsv-utils collapse [options] <tsv>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -f  fields to collapse. Comma separated, ie: 1,2,3.\n");
        fprintf(stderr, "  -d  delim.\n\n");
        return 1;
    }

    if(field == NULL){
        fprintf(stderr, "%s\n", "[ERR]: specifiy fields to collapse.\n");
        exit(-1);
    }
    
    gzFile fp;
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r"); 
    if (fp) {
        
        int *fields, n, m,  i;
        kstream_t   *ks;
        ks = ks_init(fp);
        
        kstring_t kt = {0, 0, 0};
        kputs(field, &kt);
        fields = ksplit(&kt, ',', &m);
        int field_t[m];

        memset(field_t, 0, sizeof(field_t[0]) * m );
        for (i = 0; i < m; ++i) field_t[i] = atoi(kt.s + fields[i]) - 1;

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            fields = ksplit(&kt, '\t', &n);
            for (i = 0; i < m; ++i){
                
                if(field_t[i] >= n - 1){
                    fprintf(stderr, "[ERR]: value %d excess the max collum value: %d\n", field_t[i] + 1, n - 1);
                    exit(-1);
                }
                kt.s[ fields[ field_t[i] + 1] - 1 ] = delim;

            }

            for (i = 0; i < n - 1; ++i)
                if( kt.s[ fields[i + 1] - 1 ] != delim)
                    kt.s[ fields[i + 1] - 1 ] = '\t';

            puts(kt.s);
        }

        free(kt.s);
        ks_destroy(ks);
        gzclose(fp);
    }else{
        fprintf(stderr, "error: %s file streaming error\n", argv[optind + 1]);
        exit(-1);
    }

    return 0;
}


