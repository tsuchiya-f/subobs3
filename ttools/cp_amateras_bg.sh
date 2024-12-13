#! /usr/bin/bash
echo $#
if [ $# -ne 1 ]; then
  echo "usage: cp_amateras_bg YYYYMMDD"
  exit 1
fi
echo $#

chmod 644 qsun_bg_*.out
cp qsun_bg_high.out ../Debug/qsun_bg_high.out
cp qsun_bg_high.out ../Debug/qsun_bg_high.out.$1
cp qsun_bg_high.out ../Debug/qsun_bg_tab/archive/qsun_bg_high.out.$1
cp qsun_bg_high.out /cygdrive/c/opefiles/Debug/qsun_bg_high.out
cp qsun_bg_high.out /cygdrive/c/opefiles/Debug/qsun_bg_high.out.$1
cp qsun_bg_high.out qsun_bg_high.out.$1
cp qsun_bg_low.out ../Debug/qsun_bg_low.out
cp qsun_bg_low.out ../Debug/qsun_bg_low.out.$1
cp qsun_bg_low.out ../Debug/qsun_bg_tab/archive/qsun_bg_low.out.$1
cp qsun_bg_low.out /cygdrive/c/opefiles/Debug/qsun_bg_low.out
cp qsun_bg_low.out /cygdrive/c/opefiles/Debug/qsun_bg_low.out.$1
cp qsun_bg_low.out qsun_bg_low.out.$1
cp ../Debug/qsun_bg_tab/archive/*.* /cygdrive/c/opefiles/Debug/qsun_bg_tab/archive/
scp qsun_bg_*.out.$1 iprt@130.34.116.246:/db/IPRT-SUN/CALIB/
scp bg_list.history iprt@130.34.116.246:/db/IPRT-SUN/CALIB/
