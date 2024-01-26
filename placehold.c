#include "utils.h"

int placehold_main(int argc, char *argv[]){
    
    char *placehold = "-";
    int c;
    while ((c = getopt(argc, argv, "p:")) >= 0) {   
        if (c == 'p') placehold = optarg;
    }
    
    if ( optind == argc || argc != optind + 1) {
       
        fprintf(stderr, "\n");
        fprintf(stderr, "Usage: tsv-utils placehold [options] <tsv>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -p STR replace emtpy cell with specified STR, default: [-]\n\n");

        return 1;
    }


    gzFile     fp;
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r"); 
    
    if (fp) {      
        kstream_t *ks;
        ks = ks_init(fp);
        kstring_t  kt   = {0, 0, 0};
        kstring_t  line = {0, 0, 0};
        char *p;
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
           int k;
           ks_tokaux_t aux;
           line.l = 0;
           for (p = kstrtok(kt.s, "\t", &aux), k = 0; p; p = kstrtok(0, 0, &aux), ++k){
               kt.s[aux.p - kt.s] = '\0';
               (strcmp(p, "") == 0 ) ?  ksprintf(&line, "\t%s", placehold) : ksprintf(&line, "\t%s", p);

           }

           puts(line.s + 1);

        }

        ks_destroy(ks);
        gzclose(fp);

        free(kt.s);
        free(line.s);

    }else fprintf(stderr, "[ERR]: can't open file '%s'\n", argv[optind]);
    
    return 0;

}