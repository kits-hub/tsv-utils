#include "utils.h"

void print_headline(char *);
int add_headline_main(int argc, char *argv[]){
    
    if ( argc != 3) {
        fprintf(stderr, "\nUsage: tsv-utils add_headline <header: \"string\"> <text>\n\n");
        return 1;
    }

    gzFile     fp;    
    fp = strcmp(argv[2], "-")? gzopen(argv[2], "r") : gzdopen(fileno(stdin), "r"); 

    kstring_t  kt  = {0, 0, 0};
    kstream_t  *ks;

    if( fp ){
        
        print_headline(argv[1]);

        ks = ks_init(fp);

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0) puts(kt.s);

        ks_destroy(ks);
        gzclose(fp);

    }
    free(kt.s);
    return 0;
}

void print_headline(char * str) {

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
       puts(kt.s);
       free(kt.s);

}