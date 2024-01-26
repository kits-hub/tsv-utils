#include "utils.h"

void nline_fmt (char * str, int64_t lines) {

       char *ret, *s = str;
       int  *prep = 0;
       char *pat  = "\\t";

       kstring_t kt = {0, 0, 0};

       while( (ret = kstrstr(s, pat, &prep)) != 0){
            kputsn(s, ret - s, &kt);
            kputc('\t', &kt);
            s = ret + prep[0];
       }

       kputs(s , &kt);
       printf("%s\t%lld\n", kt.s, (long long)lines);
       free(kt.s);

}

int nlines_main(int argc, char *argv[]){
    
    int   ignore    = 0;
    char *auxiliary = NULL;
    
    int c;
    while ((c = getopt(argc, argv, "a:i")) >= 0) {       
        if (c == 'i')  ignore = 1;
        if (c == 'a')  auxiliary = optarg;
    }

    if ( optind == argc || argc != optind + 1) {
        fprintf(stderr, "\nUsage: tsv-utils nlines [options] <tsv>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -a  STR print auxiliary information. ie: sample_id;\n");
        fprintf(stderr, "  -i      flag to ignore lines start with '#';\n\n");
        return 1;
    }

    gzFile     fp;
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r");

    if(fp){
        kstream_t  *ks;
        ks         = ks_init(fp);
 
        int64_t lines = 0;       
        kstring_t  kt = {0, 0, 0};

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            if(ignore == 1 && kt.s[0] == '#') continue;
            lines += 1;
        }

        
        (NULL == auxiliary) ? printf("%lld\n",(long long)lines) :
                              nline_fmt(auxiliary, lines);

        ks_destroy(ks);
        gzclose(fp);
        free(kt.s);
    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind]);
        exit(1);
    }

    return 0;
}
