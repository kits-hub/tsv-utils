#include "utils.h"

int unpack_main (int argc, char *argv[]){
    
  
    char d=',';
    int  c;
    while ((c = getopt(argc, argv, "d:")) >= 0) {
        
        if (c == 'd') d = optarg[0];
    
    }

    if ( optind == argc || argc != optind + 1) {
        
        fprintf(stderr, "\nUsage: tsv-utils unpack [options]  <bins>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -d char  delimiter for elements, default: [,]\n");
        fprintf(stderr, "           char '0' for split letters.\n");
        fprintf(stderr, "\nExample:\nFeature  1,2,3\n\n");        
        return 1;
    
    }

    int i, *fields, n;
    kstring_t  kt  = {0, 0, 0};

    gzFile fp;
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r"); 

    if(fp){

        kstream_t *ks;
        ks = ks_init(fp);
        kstring_t  kv  = {0, 0, 0};
      
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
           
           if( kt.l == 0 ) continue;          
           fields = ksplit(&kt, '\t' , &n);
           
           if(d == '0'){
              for (i = fields[1]; i < kt.l;  ++i) printf("%s\t%c\n", kt.s, kt.s[i]);
           }else{
               kv.l = 0;
               kputs(kt.s + fields[1], &kv);
               fields = ksplit(&kv, d , &n);
                
               for (i = 0; i < n; ++i){
                     printf("%s\t%s\n", kt.s, kv.s + fields[i]);
               }

           }  
        }

        free(kv.s);
        ks_destroy(ks);
        gzclose(fp);
    }
    
    free(kt.s);
    return 0;
}