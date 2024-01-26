#include "utils.h"

#include "kvec.h"
static kvec_t( int ) vt;
static khash_t(set32 ) *h;
static char delimitor = '\t';

void cut_fileds(char *s);
void cut_subset(char *fn);
void delete_fileds(char *s);
void delete_subset(char *fn);

int cut_main(int argc, char *argv[]){
    
    char *field = NULL;
    int  c;
    int  mode      = 0;

    while ((c = getopt(argc, argv, "s:f:d")) >= 0) {
        if (c == 'f') field = optarg;
        else if (c == 's') delimitor = optarg[0];
        else if (c == 'd') mode = 1;
    }

    if ( optind == argc || argc != optind + 1) {
        fprintf(stderr, "\n\nUsage: tsv-utils cut [options]  <tsv>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -f  STR  field to split\n");
        fprintf(stderr, "  -s  STR  sperator, default: [\\t]\n");
        fprintf(stderr, "  -d       delete mode. \n\n");
        return 1;
    }

    if(mode == 0){
    
        kv_init(vt);
        cut_fileds(field);
        cut_subset(argv[optind]);
    
    }else{
        
        h = kh_init(set32);
        delete_fileds(field);
        delete_subset(argv[optind]);
        
    
    }
    return 0;
}

void cut_fileds(char *s){

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

void cut_subset(char *fn){
    
    gzFile fp;
    fp = strcmp(fn, "-")? gzopen(fn, "r") : gzdopen(fileno(stdin), "r");
    if (fp) {
        
        kstring_t  kt    = {0, 0, 0};
        kstream_t *ks;
        ks = ks_init(fp);
        int *fields, n, i;
        kstring_t kv     = {0, 0, 0};

        while( ks_getuntil(ks, '\n', &kt, 0) >=  0){

            fields = ksplit(&kt, delimitor,  &n);
            kv.l = 0;
            for (i = 0; i < kv_size(vt); ++i)  ksprintf(&kv, "%c%s", delimitor, kt.s + fields[ kv_A(vt, i) ]);
            printf("%s\n", kv.s + 1);
        
        }

        free(kt.s);
        free(kv.s);
        ks_destroy(ks);
        gzclose(fp);
    
    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", fn);
        exit(-1);
    }
}


void delete_fileds(char *s){

    if (s == NULL){
    
        fprintf(stderr, "[ERR]: Please specify the fields.\n");
        exit(-1);
    
    }else{
        
        kstring_t kt = {0,0,0};
        kputs(s, &kt);
        int *fields, n, i, ret;
        fields = ksplit(&kt, ',', &n);
        for (i = 0; i < n; ++i) kh_put(set32, h, atoi(kt.s + fields[i]) - 1, &ret);
    
    }
}


void delete_subset(char *fn){
    
    gzFile fp;
    fp = strcmp(fn, "-")? gzopen(fn, "r") : gzdopen(fileno(stdin), "r");
    if (fp) {
        
        kstring_t  kt    = {0, 0, 0};
        kstream_t *ks;
        ks = ks_init(fp);
        int *fields, n, i;
        khint_t k;
        kstring_t kv     = {0, 0, 0};

        while( ks_getuntil(ks, '\n', &kt, 0) >=  0){
            
            fields = ksplit(&kt, delimitor,  &n);
            kv.l = 0;
            for (i = 0; i < n; ++i){
                k = kh_get(set32, h, i);
                if(k == kh_end(h)){
                     kputc(delimitor, &kv);
                     kputs(kt.s + fields[i], &kv);
                }
            }
            (kv.s[0] == '\t' || kv.s[0] == ',') ? printf("%s\n", kv.s + 1) : printf("%s\n", kv.s);
        
        }

        free(kt.s);
        free(kv.s);
        ks_destroy(ks);
        gzclose(fp);
        kh_destroy(set32, h);
    
    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", fn);
        exit(-1);
    }
}
