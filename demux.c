#include "utils.h"

typedef struct {
    int   setting;
    int   column;
    char *file;
    char *directory;
} opt_t;

static khash_t(handle)  *h;

void demux_usage();
void demux_init(opt_t *opt);
void demux_inspect(opt_t *opt);
void demux_mkfs(opt_t *opt);


void demux( opt_t *opt, khash_t(handle) *h );

int demux_main (int argc, char *argv[]){
   
    opt_t opt;
    demux_init(&opt);

    int c;
    if(argc == 1) demux_usage();

    while ((c = getopt(argc, argv, "f:c:d:h")) >= 0) {
        
        if (c == 'f') {
             opt.file = strdup(optarg);
             opt.setting++;
        }else if (c == 'c') {
             opt.column = atoi(optarg) - 1;
             opt.setting++;
        }else if (c == 'd') {
             opt.directory = strdup(optarg);
             opt.setting++;
        }else if (c == 'h') {
            demux_usage();
        }
    }

    demux_inspect(&opt);
    demux_mkfs(&opt);

    h = kh_init(handle);

    demux(&opt, h);

    kh_handle_destroy(h);

    return 0;
}

void demux_init(opt_t *opt){

    memset(opt, 0, sizeof(opt_t));
    opt->file      = NULL;
    opt->directory = NULL;
    opt->column    = -1;
    opt->setting   = 0;

}

void demux_inspect(opt_t *opt){

    if(opt->file == NULL) {
        fprintf(stderr, "[ERR]: please specify file, -f STR!\n\n"); 
        exit(-1);
    }

    if(opt->directory == NULL){
        fprintf(stderr, "[ERR]: please specify output directory, -d STR!\n\n");
        exit(-1);
    }
    
    if(opt->setting != 3){
        demux_usage();
        exit(-1);
    }
}

void demux_usage(){

    fprintf(stderr, "\nUsage: tsv-utils demux [options]\n\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -f STR  Tsv file;\n");
    fprintf(stderr, "  -c INT  specify column;\n");
    fprintf(stderr, "  -d STR  output filename directory;\n");
    fprintf(stderr, "  -h      display usage;\n");
    fprintf(stderr, "\nExample:\ntsv-utils demux -f tsv.txt -c 1 -d  demux\n\n");

    exit(-1);
}

void demux_mkfs(opt_t *opt){

    kstring_t kv = {0, 0, 0};
    ksprintf(&kv, "mkdir -p %s", opt->directory);
    if(  system(kv.s) != 0 ) exit (-1);
    free(kv.s);
}

void demux( opt_t *opt, khash_t(handle) *h ){

    khint_t k;
    gzFile fp;
    fp   = strcmp(opt->file, "-")? gzopen(opt->file, "r") : gzdopen(fileno(stdin), "r");
    
    if (fp == 0) {
       fprintf(stderr, "[ERR]: open file failed. check: %s\n",  opt->file);
       exit(-1);
    }

    kstream_t *ks   = ks_init(fp);

    kstring_t kt    = {0, 0, 0};
    kstring_t kswp  = {0, 0, 0};
    kstring_t kp    = {0, 0, 0};

    int *fields, n, ret;

    while( ks_getuntil(ks, '\n', &kt, 0) > 0){

        kswp.l = 0;
        kputs(kt.s, &kswp);
        fields = ksplit(&kt, '\t', &n);

        if( opt->column >= n ){
           fprintf(stderr, "[ERR]: specify column [%d] > Total column %d\n",  opt->column + 1, n);
           exit(-1);
        }

        if(kt.l == 0 || kt.s[0] == '#') continue;
        k = kh_get(handle, h, kt.s + fields[ opt-> column]);

        if( k == kh_end(h) ) {
            
            k = kh_put(handle, h, kt.s + fields[ opt->column ], &ret);
            kh_key(h, k) = strdup(kt.s + fields[ opt->column ]);
            
            kp.l  = 0;
            ksprintf(&kp, "%s/%s.txt", opt->directory, kt.s + fields[ opt->column ] );

            handle_t *handle  = (handle_t *) malloc( sizeof(handle_t) );
            handle-> fn = strdup(kp.s);
            FILE *f = fopen(handle->fn, "w");
            if(f == 0){
                printf("[ERR]: FILEHANLE error. too max file opened.\n");
                exit(-1);
            }
            handle-> fh = f;
            kh_val(h, k) = handle;
        
        }

        fprintf(kh_val(h, k)->fh, "%s\n", kswp.s);

    }

    gzclose(fp);
    ks_destroy(ks);

    free(kt.s);
    free(kswp.s);
    free(kp.s);

    
}
