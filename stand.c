#include "utils.h"
#include "kvec.h"
#include <math.h>

int stand_main(int argc, char *argv[]){
    
    if ( argc != 2) {        
        fprintf(stderr, "\nUsage: tsv-utils stand <tsv>\n\n");
        return 1;
    }

    kstring_t kt    = {0, 0, 0};
    kstring_t title = {0, 0, 0};

    int *fields, n, i, ret, lines, columns;
    double val = 1.0;
    lines = columns = 0;

    khash_t(reg) *h;
    h = kh_init(reg);
    khint_t k;

    kvec_t(double) mean, stddev;
    kv_init(mean);
    kv_init(stddev);

    kvec_t( char* ) vs;
    kv_init(vs);

    gzFile     fp;
    fp = strcmp(argv[1], "-")? gzopen(argv[1], "r") : gzdopen(fileno(stdin), "r");

    if (fp) {
        
        kstream_t *ks;
        ks = ks_init(fp);

        if(ks_getuntil(ks, '\n', &kt, 0) >=  0){
            if(kt.s[0] == '#'){
                kputs(kt.s, &title);
                fields = ksplit(&kt, '\t', &n);
                for (i = 1; i < n; ++i){
                    kv_push(double, stddev,  0 );
                    kv_push(double, mean,    0 );
                    columns = n - 1;
                }
            }else{
                fprintf(stderr, "[ERR]: No headline with '#'!\n");
                exit(1);
            }
        }else{
            fprintf(stderr, "[ERR]: empty file. !\n");
            exit(1);
        }

        kstring_t kk    = {0, 0, 0};
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            fields = ksplit(&kt, '\t', &n);

            kk.l = 0;
            for (i = 1; i < n; ++i){
                val = log( atof(kt.s + fields[i]) + 1 ) / log(2);
                ksprintf(&kk, "\t%lf", val);
                kv_a(double, mean, i - 1 ) +=  val;
            }

            ++lines;

            k = kh_put(reg, h, kt.s, &ret);
            char *p = strdup(kt.s);
            kv_push(char *, vs, p);
            if(ret){
                kh_key(h, k)   = p;
                kh_val(h, k)   = strdup(kk.s);
            }
        }

        free(kk.s);
        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[1]);
        exit(1);
    }

    for (i = 0; i < columns; ++i) kv_a(double, mean, i ) =  kv_A(mean, i)/lines;
        
    for (k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)) {
            kt.l = 0;
            kputs(kh_val(h, k), &kt);
            fields = ksplit(&kt, '\t', &n);
            for (i = 0; i < n; ++i) kv_a(double, stddev, i) += pow((atof(kt.s + fields[i]) - kv_A(mean, i)), 2);
        }
    }

    for (i = 0; i < columns; ++i) kv_a(double, stddev, i ) =  sqrt(((double)1/lines) *  kv_A(stddev, i));

    printf("%s\n", title.s);
    for (i = 0; i < kv_size(vs); ++i){
        k = kh_get(reg, h, kv_A(vs, i));     
        if(k != kh_end(h)){
           fputs(kh_key(h, k), stdout);
           kt.l = 0;
           kputs(kh_val(h, k), &kt);
           fields = ksplit(&kt, '\t', &n);
           int j;
           for (j = 0; j < n; ++j) fprintf(stdout, "\t%lf",  (double)(atof(kt.s + fields[j]) -  kv_A(mean, j))/ kv_A(stddev, j) );
           fputc('\n', stdout);
        }else{
            fprintf(stderr, "[ERR]: No header match: %s\n", kv_A(vs, i));
            exit(1);
        }
    }

    kh_reg_destroy( h );
    kv_destroy(mean);
    kv_destroy(stddev);
    kv_destroy(vs);
    free(kt.s);
    return 0;
}