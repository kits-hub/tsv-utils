#include "utils.h"

int replace_main(int argc, char *argv[]){

    int c;
    int target = 0;
    int R      = 0;

    while ((c = getopt(argc, argv, "c:r")) >= 0) {
        
        if(c == 'r') R = 1;
        else if (c == 'c') {
            target = atoi(optarg) - 1;
        }

    }

    if ( optind == argc || argc != optind + 2 ) {
        
        fprintf(stderr, "\n");       
        fprintf(stderr, "Usage: tsv-utils replace [options] <db> <text>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -c  INT column for replace.\n");
        fprintf(stderr, "  -r  remove unmapped lines.\n\n");
        return 1;
    
    }

    kstring_t kt    = {0, 0, 0};

    int *fields, i, ret, n;

    khash_t(reg) *h;
    h = kh_init(reg);
    khint_t k;

    kstream_t *ks;

    gzFile fp;
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r"); 
    if(fp){
        
        ks = ks_init(fp);
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
                if(kt.l == 0) continue;
                ks_tokaux_t aux;
                kstrtok(kt.s, "\t", &aux);
                kt.s[aux.p - kt.s] = '\0';
                k = kh_put(reg, h, kt.s, &ret);
                
                if(ret){
                    kh_key(h, k) = strdup(kt.s);
                    kh_val(h, k) = strdup(aux.p + 1);
                }
        }

        ks_destroy(ks);
        gzclose(fp);
    
    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind]);
        exit(1);
    }

    kstring_t aux   = {0, 0, 0};

    fp = strcmp(argv[optind + 1], "-")? gzopen(argv[optind + 1], "r") : gzdopen(fileno(stdin), "r"); 
    if (fp) {
        
        ks  = ks_init(fp);        
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            if(kt.l == 0) continue;
            if(kt.s[0] == '#') {
                puts(kt.s);
                continue;
            }

            aux.l = 0;
            fields = ksplit(&kt, '\t', &n);
  

            k = kh_get(reg, h, kt.s +  fields[ target ]);
            if(R && k == kh_end(h)) continue;

            for (i = 0; i < n; ++i){
                kputc('\t', &aux);
                if( i == target ){
                    (k != kh_end(h)) ? kputs(kh_val(h, k),  &aux) : kputs(kt.s +  fields[ target ],  &aux);
                }else
                    kputs(kt.s +  fields[i],  &aux);
            }
            puts(aux.s + 1);
        
        }
        
        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind + 1]);
        exit(1);
    }

    kh_reg_destroy( h );
    free(kt.s);
    free(aux.s);

    return 0;
}