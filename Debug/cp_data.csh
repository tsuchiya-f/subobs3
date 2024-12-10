#! /usr/bin/csh
cd /cygdrive/f/opefiles/Debug
scp -l 1000 *$1*.gif iprt@iprt.gp.tohoku.ac.jp:~/data/sun/QL/
scp -l 1000 index2FITS.txt $1_2FITS.txt $1_index2FITS.txt iprt@iprt.gp.tohoku.ac.jp:~/data/sun/LOW/
scp -l 1000 *$1*.pdf iprt@iprt.gp.tohoku.ac.jp:~/data/sun/pdf/
