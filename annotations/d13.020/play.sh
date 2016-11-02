
LIB=$(dirname $0)/../conv

. ${LIB}/convlib.sh

SYM=flash_sym.tmp

ra2sym <flash.r  >$SYM
awk -f sym2lnk.awk $SYM | sort >lnk.tmp

exit


#ra2sym <flash.r | sym_strip >o1.tmp

#lnk2sym <../../applet/src/symbols_d13.020 | sym_strip >o2.tmp

ra2sym <flash.r  >$SYM


awk -f sym2ra.awk $SYM >r2.tmp
awk -f sym2lnk.awk $SYM >lnk.tmp

cat ../../applet/src/symbols_d13.020 | sed 's/;/ ; /' | awk -f lnk2sym.awk | sort >sym_d13.tmp

cat sym_d13.tmp | sym2ra >ra.tmp 

awk '{ print $1 OFS $2 }' $SYM >sym2.tmp

#diff <( sort lnk.tmp ) <( sort ../../applet/src/symbols_d13.020) 

awk -f ra2sym.awk ../s13.020/flash.r | sort >s13.tmp
