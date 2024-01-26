#include "utils.h"

#include "kvec.h"
#include "ksort.h"

#include <math.h>
#include <stdint.h>
#include <float.h>

static kvec_t( double ) vt;
KSORT_INIT_GENERIC(double)

int stats_main (int argc, char *argv[]){
    
    int c, col = 0, skip = 0;
    uint64_t n = 0;
    double min = DBL_MAX, max = DBL_MIN, avg;
    long double sum = 0.;

    while ((c = getopt(argc, argv, "c:s")) >= 0) {
        if (c == 'c') col = atol(optarg) - 1;
        else if (c == 's') skip = 1;
    }

    if ( optind == argc || argc != optind + 1) {
        
        fprintf(stderr, "\nUsage: tsv-utils stats [options] <tsv>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -c INT     column number [1].\n");
        fprintf(stderr, "  -s         skip first line.\n");
        fprintf(stderr, "\n");
        return 1;
    
    }
    
    printf("#number\tsum\tmean\tmin\tmax\tstd.dev\tskewness\t25%%-percentile\tmedian\t75%%\t2.5%%\t97.5%%\t1%%\t99%%\t0.5%%\t99.5%%\n");

    gzFile     fp;
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r");

    kstream_t *ks;
    ks = ks_init(fp);
    kv_init(vt);

    if (fp) {

        kstring_t  kt  = {0, 0, 0};
        ks = ks_init(fp);
        int *fields, num;
        double val;

        if(skip == 1) ks_getuntil(ks, '\n', &kt, 0);
        while( ks_getuntil(ks, '\n', &kt, 0) >=  0 ){
            
            fields = ksplit(&kt, '\t', &num);
            val = atof( kt.s + fields[col] );

            ++n; sum += val;
            min = min < val? min : val;
            max = max > val? max : val;
            kv_push(double, vt, val);
        }
        free(kt.s);
        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind]);
        exit(1);
    }
    
    if (n == 0) {
        fprintf(stderr, "[ERR]: no data are read\n");
        return 1;
    }

    avg = sum / n;
    printf("%llu\t%Lf\t%g\t%g\t%g", (unsigned long long)n, sum, avg, min, max);

    long double sum2 = 0., sum3 = 0.;
    double q[3], CI5[2], CI2[2], CI1[2];
    uint64_t i;
    if (n > 1) {
        double g1, tmp;
        for (i = 0; i < n; ++i) {
            double t = (vt.a[i] - avg) * (vt.a[i] - avg);
            sum2 += t;
            sum3 += t * (vt.a[i] - avg);
        }
        tmp = sqrt(sum2 / n);
        printf("\t%g", sqrt(sum2 / (n - 1)));
        g1 = (sum3 / n) / (tmp * tmp * tmp);
        if (n > 2) printf("\t%g", sqrt((double)n * (n - 1)) / (n - 2) * g1);
        else printf("\tNaN");
    } else printf("\tNaN");

    q[0]   = ks_ksmall(double, vt.n, vt.a, (int)(ceil(n * .25)  + .499));
    q[1]   = ks_ksmall(double, vt.n, vt.a, (int)(ceil(n * .50)  + .499));
    q[2]   = ks_ksmall(double, vt.n, vt.a, (int)(ceil(n * .75)  + .499));
    CI5[0] = ks_ksmall(double, vt.n, vt.a, (int)(ceil(n * .025) + .499));
    CI5[1] = ks_ksmall(double, vt.n, vt.a, (int)(ceil(n * .975) + .499));
    CI2[0] = ks_ksmall(double, vt.n, vt.a, (int)(ceil(n * .01)  + .499));
    CI2[1] = ks_ksmall(double, vt.n, vt.a, (int)(ceil(n * .99)  + .499));
    CI1[0] = ks_ksmall(double, vt.n, vt.a, (int)(ceil(n * .005) + .499));
    CI1[1] = ks_ksmall(double, vt.n, vt.a, (int)(ceil(n * .995) + .499));
    printf("\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g", q[0], q[1], q[2], CI5[0], CI5[1], CI2[0], CI2[1], CI1[0], CI1[1]);
    putchar('\n');

    kv_destroy(vt);
    return 0;
}