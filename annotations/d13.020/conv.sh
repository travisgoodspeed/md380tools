
SYM=sym.tmp

awk -f ra2sym.awk flash.r | sort >$SYM

awk -f sym2ra.awk $SYM >r2.tmp
awk -f sym2lnk.awk $SYM >lnk.tmp

cat ../../applet/src/symbols_d13.020 | sed 's/;/ ; /' | awk -f lnk2sym.awk | sort >sym_d13.tmp
awk '{ print $1 OFS $2 }' $SYM >sym2.tmp

#diff <( sort lnk.tmp ) <( sort ../../applet/src/symbols_d13.020) 

awk -f ra2sym.awk ../s13.020/flash.r | sort >s13.tmp
