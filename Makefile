CC=		cc
CFLAGS=		-g -Wall -O2 -Wno-sequence-point -Wno-unused-function
CPPFLAGS=
INCLUDES=
PROG=		tsv-utils
BINDIR=		/usr/local/bin
LIBS=		-lz -lm
OBJS=		kstring.o utils.o fastrand.o rank.o abundance.o stand.o norm.o trim.o \
			annotation.o links.o replace.o associate.o subset.o agg.o subcolumn.o \
			bins.o definition.o collapse.o transpose.o matrix2melt.o matrix2tab.o \
			submatrix.o add_headline.o melt.o distribution.o accumulation.o placehold.o \
			join.o groupline.o ksort.o reorder.o uniq.o nlines.o cut.o stats.o view.o \
			reshape.o unpack.o nfilter.o select.o dedup.o pretty.o shuffle.o strip.o \
			split.o pad.o head.o pairwise.o convert.o demux.o skip.o tsv-utils.o

.SUFFIXES:.c .o
.PHONY:all clean

.c.o:
		$(CC) -c $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $< -o $@

all:$(PROG)

tsv-utils:$(OBJS) tsv-utils.o
		$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

install:all
		install tsv-utils $(BINDIR)

clean:
		rm -fr *.o tsv-utils