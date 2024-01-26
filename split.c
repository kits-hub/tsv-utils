#include "utils.h"

int split_main(int argc, char *argv[]){

    int n = 1000;
    int c;
    while ((c = getopt(argc, argv, "n:")) >= 0) {
        if (c == 'n') n = atoi(optarg);
    }

    if ( optind == argc || argc != optind + 2) {
        fprintf(stderr, "\n\nUsage: tsv-utils split [options] <file> <prefix>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -n  : default: [1000];\n\n");
        return 1;
    }

    gzFile fp;
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r");
    if(fp){
        
        char    *prefix        = NULL;
        int     current_chunk  = 1;
        int     current_index  = 0;
        
        kstring_t chunk_name = {0,0,0};
        FILE *output = NULL;
      
        prefix = argv[optind + 1];

        ksprintf(&chunk_name,"%s_%d", prefix, current_chunk++);  
        if ((output = fopen(chunk_name.s, "w")) == NULL) {
            fprintf(stderr,"[ERR]: can't open file %s\n", chunk_name.s);
            exit(1);
        }

        kstream_t *ks;
        ks          = ks_init(fp);
        kstring_t kt = {0,0,0};
        
        while( ks_getuntil(ks, '\n', &kt, 0) > 0 ){
            
            if(kt.s[0] == 0 || kt.s[0] == '#') continue;

            current_index++;
            if(current_index <= n){
                fprintf(output, "%s\n", kt.s);
                continue;
            }
            
            fclose(output);
            current_index = 1;

            chunk_name.l  = 0;
            ksprintf(&chunk_name,"%s_%d", prefix, current_chunk++);  
            if ((output = fopen(chunk_name.s, "w")) == NULL) {
                fprintf(stderr,"[ERR]: can't open file %s\n", chunk_name.s);
                exit(1);
            }
            fprintf(output, "%s\n", kt.s);

        }

        free(kt.s);
        free(chunk_name.s);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind]);
        exit(1);
    }


    return 0;
}
