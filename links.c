#include "utils.h"

int links_main(int argc, char *argv[]){
    
    if (argc != 4 ) {
        fprintf(stderr, "\nUsage: tsv-utils links <feature-map> <links> <links-definition>\n\n");
        return 1;
    }

    int *fields, n, i, ret, *elements;

    khash_t(reg) *def;
    def = kh_init(reg);

    khash_t(set) *set;
    set = kh_init(set);

    khint_t k;
    kstring_t  kt = {0, 0, 0};
    kstream_t *ks;
    gzFile     fp;
    fp = strcmp(argv[3], "-")? gzopen(argv[3], "r") : gzdopen(fileno(stdin), "r"); 
    if (fp) {

        ks = ks_init(fp);
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            fields = ksplit(&kt, '\t', &n);
            k = kh_put(reg, def, kt.s, &ret);
            if(ret){
                kh_key(def, k)   = strdup(kt.s);
                kh_value(def, k) = strdup(kt.s + fields[1]);
            }
        }
        ks_destroy(ks);
        gzclose(fp);
    
    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[3]);
        exit(1);
    }

    khash_t(kreg) *links;
    links = kh_init(kreg);

    fp = strcmp(argv[2], "-")? gzopen(argv[2], "r") : gzdopen(fileno(stdin), "r"); 
    if (fp) {
        
        ks = ks_init(fp);
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){

            k   = kh_put( set, set, kt.s, &ret);
            if(ret)           
                kh_key(set, k) = strdup(kt.s);
            else if(ret == 0) continue;
            
            fields = ksplit(&kt, '\t', &n);
            k = kh_put(kreg, links, kt.s, &ret);

            if(ret){
                
                kstring_t ktmp = {0, 0, 0};
                kputs(kt.s +  fields[1], &ktmp);
                kh_key(links, k)  = strdup(kt.s);
                kh_val(links, k)  = ktmp;
            
            }else{
                kputc(',', &kh_val(links, k));
                kputs(kt.s + fields[1], &kh_val(links, k));
            }
        
        }

        ks_destroy(ks);
        gzclose(fp);
    
    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[2]);
        exit(1);
    }

    fp = strcmp(argv[1], "-")? gzopen(argv[1], "r") : gzdopen(fileno(stdin), "r"); 
    if (fp) {
        
        kstring_t  kk = {0, 0, 0};
        ks = ks_init(fp);
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            fields = ksplit(&kt, '\t', &n);
            k = kh_get(kreg, links, kt.s + fields[1]);            
            if(k != kh_end(links)){
                kk.l = 0;
                kputs(kh_val(links, k).s, &kk);
                elements = ksplit(&kk, ',' , &n);
                for (i = 0; i < n; ++i){
                    khint_t  t  = kh_get(reg, def, kk.s + elements[i] );
                    if(t != kh_end(def)){
                       printf("%s\t%s\t%s\n", kt.s, kk.s + elements[i], kh_val(def, t));
                    }else{
                       printf("%s\t%s\t-\n", kt.s, kk.s + elements[i]);
                    }
                }
            }
        }
        ks_destroy(ks);
        gzclose(fp);
        free(kk.s);

    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[1]);
        exit(1);
    }
    
    free(kt.s);
    kh_set_destroy(set);
    kh_reg_destroy(def);
    kh_kreg_destroy(links);
    return 0;

}