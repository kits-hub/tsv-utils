#include "utils.h"
#include <math.h>

static int  n_sample;
static int  R = 0;
static char D = ';';

void dv_print (int n, double *dv){

    int i;
    for (i = 0; i < n; ++i){
      (R) ?  printf("\t%d", (int)round( dv[i] ) ) :
             printf("\t%.5lf", dv[i]);
    }

    putchar('\n');
}

void dv_zero (int n, double *dv){
    int i;
    for (i = 0; i < n; ++i) dv[i] = 0;
}

int melt_main(int argc, char *argv[]){
    
    int c;
    while ((c = getopt(argc, argv, "d:r")) >= 0) {
        if (c == 'r') R = 1;
        else if (c == 'd') D = optarg[0];

    }

    if ( optind == argc || argc != optind + 2) {
        fprintf(stderr, "\nUsage: tsv-utils melt [options] <bin-tab> <matrix>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -r  round value to nearest intege;\n");
        fprintf(stderr, "  -d  delimiter for bins, default [:]\n\n");
        return 1;
    }

    khash_t(kreg) *h;
    h = kh_init(kreg);
    
    khint_t  k;

    kstream_t  *ks;
    kstring_t   kt         = {0, 0, 0};
    kstring_t   k_features = {0, 0, 0};
   
    int *fields, ret, n, i;

    gzFile  fp;
    fp = strcmp(argv[optind + 1], "-")? gzopen(argv[ optind + 1], "r") : gzdopen(fileno(stdin), "r");   
    if(fp){
        
        ks = ks_init(fp);
        char *p;

        if(ks_getuntil( ks, '\n', &kt, 0) >=  0){
            if( kt.s[0] == '#'){
                ks_tokaux_t aux;
                p = kstrtok(kt.s, "\t", &aux);
                kt.s[aux.p - p] = '\0';
                printf("#features\t%s\n", aux.p + 1);
                kputs(aux.p + 1, &k_features);
                ksplit(&k_features, '\t',  &n);
                n_sample = n;
            }else{
                fprintf(stderr, "[ERR]: Specify samples with '#'. \n\n");
                exit(-1);
            }
        }

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            if( kt.l == 0 ) continue;

            ks_tokaux_t aux;
            p = kstrtok(kt.s, "\t", &aux);
            kt.s[aux.p - p] = '\0';

            k = kh_put(kreg, h, p, &ret);
            kh_key(h, k) = strdup(p);

            kstring_t k_features = {0, 0, 0};

            kputs(aux.p + 1, &k_features);
            kh_val(h, k) = k_features;

        }

        ks_destroy(ks);
        gzclose(fp);
    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind + 1]);
        exit(1);
    }


    double *dv  = (double *) malloc(n_sample * sizeof(double));
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r");   
    if(fp){
        
        ks = ks_init(fp);
        char *p;
        kstring_t k_elements = {0, 0, 0};
        int *values, n_values, j;
        kstring_t  buffer = {0, 0, 0};

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            if( kt.l == 0 || kt.s[0] == '#') continue;

            ks_tokaux_t aux;
            p = kstrtok(kt.s, "\t", &aux);
            kt.s[aux.p - p] = '\0';

            k_elements.l = 0;
            kputs(aux.p + 1, &k_elements);

            dv_zero(n_sample, dv);
            fields = ksplit(&k_elements, D ,&n);

            for (i = 0; i < n; ++i){

                k = kh_get(kreg, h, k_elements.s + fields[i]);
                if( k == kh_end(h) ) continue;

                buffer.l = 0;
                kputs(kh_val(h, k).s, &buffer);
                values =  ksplit(&buffer, '\t', &n_values);
                for (j = 0; j < n_values; ++j) dv[j] +=  atof(buffer.s + values[j]);

            }

            fputs(p, stdout);
            dv_print(n_sample, dv);

        }

        ks_destroy(ks);
        gzclose(fp);
    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind]);
        exit(1);
    }

    free(dv);

    kh_kreg_destroy(h);
    return 0;

}

