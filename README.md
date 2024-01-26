###  tsv-utils: tools for tsv files manipulation 

<hr>

#### 1. install

```sh
git clone https://github.com/kits-hub/tsv-utils
cd tsv-utils
make
```

#### 2. interface

current: `version：0.0.2`


```text
Usage:   tsv-utils <command> <arguments>
Version: 0.0.2

Command:
      -- Combination
         agg            combinate multi-file.
         join           join tables with primery key.

      -- Numeric data frame
         rank           rank/merge for numeric table.
         abundance      calculate relative abundance for numeric table.
         norm           normalization with  normalization
                        factor for numeric table.
         stand          standardization for numeric table.
         melt           merge values with bin table file.
         distribution   calculate bins feature distribution.
         trim           trim rows using cutoff.(sum operation).
         select         select rows using string match.
         nfilter        filter using value with specied collum. op: >= | <= .
         ksort          sort line by numeric.

      -- Editing
         annotation     annotating specify collum with key/value(s) db.
         links          transform annotation with links map and definitions.
         associate      associate with links map.
         definition     adding definition collum with key/value(s) db.
         replace        replace specify collum elements with key/value(s) db.
         reorder        reorder rows by specify key in specify collum.
         subset         retrieve ids in/not in list file [row].
         subcolumn      retrieve ids in/not in list file [collum].
         collapse       collapse '\t' separator to specify delim.
         add_headline   add headline.
         groupline      add groupline.
         placehold      fill empty cell with specify STR.
         reshape        reshape and bin using map file.
         pairwise       display feature table in pairwise style.

      -- Matrix Operation
         transpose      matrix transpose.
         submatrix      submatrix by id.
         matrix2tab     binary format.
         matrix2melt    elements in submatrix using metadata.

      -- Summary
         cut            print selected parts of lines.
         bins           uniq/bin/summary.
         uniq           unique specify collum and counts.
         nlines         calculate lines of file.
         stats          calculate stats for selected collum.
         unpack         unpack the bins files.

      -- auxiliary utils.
         view           view text file, ignor comments and blank lines.
         skip           remove the lines start with pattern but keep the first occurrence.
         pretty         pint the tsv with equal width elements.
         shuffle        shuffle a string list.
         strip          strip leading and tailing whitespace.
         split          split file with specified line numer. ignor '#' and blank line .
         pad            pad a string to line.
         head           print head line in collum manner.
         convert        convert specified collum to lowcase or uppercase.
         demux          demux file using specified collum elements.


Licenced:
(c) 2018-2024 - LEI ZHANG
Logic Informatics Co.,Ltd.
zhanglei@logicinformatics.com
```

#### 3. doc

```
https://github.com/atlashub/biostack-suits-docs/tree/master/tsv-utils
```
