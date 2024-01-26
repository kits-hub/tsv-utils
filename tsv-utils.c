/*****************************************************************************
  tsv-utils.c

  tsv-utils command line interface.  

  (c) 2018-2024 - LEI ZHANG
  Logic Informatics Co.,Ltd.
  zhanglei@logicinformatics.com
  
  Licenced under The MIT License.
******************************************************************************/

#include <stdio.h>
#include <string.h>

int agg_main(int argc, char *argv[]);
int join_main(int argc, char *argv[]);
int bins_main(int argc, char *argv[]);
int cut_main(int argc, char *argv[]);

int melt_main(int argc, char *argv[]);
int distribution_main(int argc, char *argv[]);
int trim_main(int argc, char *argv[]);
int ksort_main(int argc, char *argv[]);

int annotation_main(int argc, char *argv[]);
int links_main(int argc, char *argv[]);
int reorder_main(int argc, char *argv[]);
int definition_main(int argc, char *argv[]);
int replace_main(int argc, char *argv[]);
int subset_main(int argc, char *argv[]);
int collapse_main(int argc, char *argv[]);
int add_headline_main(int argc, char *argv[]);
int groupline_main(int argc, char *argv[]);

int transpose_main(int argc, char *argv[]);
int submatrix_main(int argc, char *argv[]);
int matrix2tab_main(int argc, char *argv[]);
int matrix2melt_main(int argc, char *argv[]);
int subcolumn_main(int argc, char *argv[]);

int rank_main(int argc, char *argv[]);
int abundance_main(int argc, char *argv[]);
int norm_main(int argc, char *argv[]);
int stand_main(int argc, char *argv[]);
int placehold_main(int argc, char *argv[]);
int nlines_main(int argc, char *argv[]);
int uniq_main(int argc, char *argv[]);
int stats_main(int argc, char *argv[]);
int view_main(int argc, char *argv[]);
int reshape_main(int argc, char *argv[]);
int unpack_main(int argc, char *argv[]);
int associate_main(int argc, char *argv[]);
int select_main(int argc, char *argv[]);
int nfilter_main(int argc, char *argv[]);
int dedup_main(int argc, char *argv[]);
int pretty_main(int argc, char *argv[]);
int shuffle_main(int argc, char *argv[]);
int strip_main(int argc, char *argv[]);
int split_main(int argc, char *argv[]);
int pad_main(int argc, char *argv[]);
int head_main(int argc, char *argv[]);
int pairwise_main(int argc, char *argv[]);
int convert_main(int argc, char *argv[]);
int demux_main(int argc, char *argv[]);
int skip_main(int argc, char *argv[]);

static int usage(){
    
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage:   tsv-utils <command> <arguments>\n");
    fprintf(stderr, "Version: 0.0.2\n\n");

    fprintf(stderr, "Command:\n");
    fprintf(stderr, "      -- Combination\n");
    fprintf(stderr, "         agg            combinate multi-file.\n");
    fprintf(stderr, "         join           join tables with primery key.\n");
    fprintf(stderr, "\n");

    fprintf(stderr, "      -- Numeric data frame\n");
    fprintf(stderr, "         rank           rank/merge for numeric table.\n");
    fprintf(stderr, "         abundance      calculate relative abundance for numeric table.\n");
    fprintf(stderr, "         norm           normalization with  normalization \n");
    fprintf(stderr, "                        factor for numeric table.\n");
    fprintf(stderr, "         stand          standardization for numeric table.\n");
    fprintf(stderr, "         melt           merge values with bin table file.\n");
    fprintf(stderr, "         distribution   calculate bins feature distribution. \n");
    fprintf(stderr, "         trim           trim rows using cutoff.(sum operation). \n");
    fprintf(stderr, "         select         select rows using string match.\n");
    fprintf(stderr, "         nfilter        filter using value with specied collum. op: >= | <= .\n");
    fprintf(stderr, "         ksort          sort line by numeric.\n");

    fprintf(stderr, "\n");
    fprintf(stderr, "      -- Editing\n");
    fprintf(stderr, "         annotation     annotating specify collum with key/value(s) db.\n");
    fprintf(stderr, "         links          transform annotation with links map and definitions.\n");
    fprintf(stderr, "         associate      associate with links map.\n");
    fprintf(stderr, "         definition     adding definition collum with key/value(s) db.\n");
    fprintf(stderr, "         replace        replace specify collum elements with key/value(s) db.\n");
    fprintf(stderr, "         reorder        reorder rows by specify key in specify collum.\n");
    fprintf(stderr, "         subset         retrieve ids in/not in list file [row].\n");
    fprintf(stderr, "         subcolumn      retrieve ids in/not in list file [collum].\n");
    fprintf(stderr, "         collapse       collapse '\\t' separator to specify delim.\n");
    fprintf(stderr, "         add_headline   add headline.\n");
    fprintf(stderr, "         groupline      add groupline.\n");
    fprintf(stderr, "         placehold      fill empty cell with specify STR.\n");
    fprintf(stderr, "         reshape        reshape and bin using map file.\n");
    fprintf(stderr, "         pairwise       display feature table in pairwise style.\n");

    fprintf(stderr, "\n");

    fprintf(stderr, "      -- Matrix Operation\n");
    fprintf(stderr, "         transpose      matrix transpose.\n");
    fprintf(stderr, "         submatrix      submatrix by id.\n");
    fprintf(stderr, "         matrix2tab     binary format.\n");
    fprintf(stderr, "         matrix2melt    elements in submatrix using metadata.\n");
    fprintf(stderr, "\n");

    fprintf(stderr, "      -- Summary\n");
    fprintf(stderr, "         cut            print selected parts of lines.\n");
    fprintf(stderr, "         bins           uniq/bin/summary.\n");
    fprintf(stderr, "         uniq           unique specify collum and counts.\n");
    fprintf(stderr, "         nlines         calculate lines of file.\n");
    fprintf(stderr, "         stats          calculate stats for selected collum.\n");
    fprintf(stderr, "         unpack         unpack the bins files.\n\n");

    fprintf(stderr, "      -- auxiliary utils.\n");
    fprintf(stderr, "         view           view text file, ignor comments and blank lines.\n");
    fprintf(stderr, "         skip           remove the lines start with pattern but keep the first occurrence.\n");
    fprintf(stderr, "         pretty         pint the tsv with equal width elements.\n");
    fprintf(stderr, "         shuffle        shuffle a string list.\n");
    fprintf(stderr, "         strip          strip leading and tailing whitespace.\n");
    fprintf(stderr, "         split          split file with specified line numer. ignor '#' and blank line .\n");
    fprintf(stderr, "         pad            pad a string to line.\n");
    fprintf(stderr, "         head           print head line in collum manner.\n");
    fprintf(stderr, "         convert        convert specified collum to lowcase or uppercase.\n");
    fprintf(stderr, "         demux          demux file using specified collum elements.\n");

    fprintf(stderr, "\n");
    
    fprintf(stderr, "\nLicenced:\n");
    fprintf(stderr, "(c) 2018-2024 - LEI ZHANG\n");
    fprintf(stderr, "Logic Informatics Co.,Ltd.\n");
    fprintf(stderr, "zhanglei@logicinformatics.com\n");
    fprintf(stderr, "\n");

    return 1;

}

int main(int argc, char *argv[]){

    if (argc < 2) return usage();
    
    if (strcmp(argv[1], "rank") == 0) rank_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "abundance") == 0) abundance_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "norm") == 0) norm_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "stand") == 0) stand_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "agg") == 0) agg_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "join") == 0) join_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "annotation") == 0) annotation_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "links") == 0) links_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "associate") == 0) associate_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "definition") == 0) definition_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "replace") == 0) replace_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "reorder") == 0) reorder_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "melt") == 0) melt_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "reshape") == 0) reshape_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "subset") == 0) subset_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "subcolumn") == 0) subcolumn_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "bins") == 0) bins_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "distribution") == 0) distribution_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "trim") == 0) trim_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "add_headline") == 0) add_headline_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "collapse") == 0) collapse_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "transpose") == 0) transpose_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "submatrix") == 0) submatrix_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "placehold") == 0) placehold_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "groupline") == 0) groupline_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "cut") == 0) cut_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "unpack") == 0) unpack_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "uniq") == 0) uniq_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "nlines") == 0) nlines_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "stats") == 0) stats_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "matrix2tab") == 0) matrix2tab_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "matrix2melt") == 0) matrix2melt_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "view") == 0) view_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "skip") == 0) skip_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "nfilter") == 0) nfilter_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "select") == 0) select_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "dedup") == 0) dedup_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "pretty") == 0) pretty_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "shuffle") == 0) shuffle_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "strip") == 0) strip_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "split") == 0) split_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "pad") == 0) pad_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "ksort") == 0) ksort_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "head") == 0) head_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "convert") == 0) convert_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "pairwise") == 0) pairwise_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "demux") == 0) demux_main(argc - 1, argv + 1);
    else {
        fprintf(stderr, "[main] unrecognized command '%s'. Abort!\n", argv[1]);
        return 1;
    }
    return 0;

}
