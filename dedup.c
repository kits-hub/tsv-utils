#include "utils.h"

#include "kvec.h"
static kvec_t( int ) vt;
static khash_t(set) *h;

void dedup_fileds(char *s);
void dedup_print(char *fn);

int dedup_main(int argc, char *argv[]){
    
    char *field = NULL;
    int c;
    while ((c = getopt(argc, argv, "f:")) >= 0) {
        if (c == 'f') field = optarg;
    }

    if ( optind == argc || argc != optind + 1) {
        fprintf(stderr, "\n\nUsage: tsv-utils dedup [options]\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -f  STR  field to split \n\n");
        return 1;
    }


    kv_init(vt);
    h = kh_init(set);
    dedup_fileds(field);
    dedup_print(argv[optind]);
    
    return 0;
}

void dedup_fileds(char *s){

    if (s == NULL){
    
        fprintf(stderr, "[ERR]: Please specify the fields.\n");
        exit(-1);
    
    }else{
        
        kstring_t kt = {0,0,0};
        kputs(s, &kt);
        int *fields, n, i;
        fields = ksplit(&kt, ',', &n);
        for (i = 0; i < n; ++i) kv_push(int, vt, atoi(kt.s + fields[i]) - 1);
    
    }
}

void dedup_print(char *fn){
    
    gzFile fp;
    fp = strcmp(fn, "-")? gzopen(fn, "r") : gzdopen(fileno(stdin), "r");
    if (fp) {

        kstring_t  kt    = {0, 0, 0};
        kstream_t *ks;
        ks = ks_init(fp);
        
        int *fields, n, i, ret;
        khint_t k;
        kstring_t kv     = {0, 0, 0};
        kstring_t kp     = {0, 0, 0};

        while( ks_getuntil(ks, '\n', &kt, 0) >=  0){
            
            if( kt.s[0] == '#'){
                puts(kt.s);
                continue;
            }

            kp.l = 0;
            kputs(kt.s, &kp);

            fields = ksplit(&kt, '\t',  &n);
            kv.l = 0;
            for (i = 0; i < kv_size(vt); ++i)  ksprintf(&kv, "\t%s" , kt.s + fields[ kv_A(vt, i) ]);

            k = kh_get(set, h, kv.s);
            if(k == kh_end(h)){
               puts(kp.s);
               k =  kh_put(set, h, kv.s,  &ret);
               kh_key(h, k) = strdup(kv.s);
            }

        }

        free(kt.s);
        free(kv.s);
        free(kp.s);

        ks_destroy(ks);
        kh_set_destroy(h);
    
    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", fn);
        exit(-1);
    }
}
