
sym2ra()
{
awk -f ${LIB}/sym2ra.awk
}

ra2sym()
{
awk -f ${LIB}/ra2sym.awk
}

sym2lnk()
{
awk -f ${LIB}/sym2lnk.awk
}

sym_strip()
{
awk '{ print $1 OFS $2 }'
}

lnk2sym()
{
sed 's/;/ ; /' | awk -f ${LIB}/lnk2sym.awk 
}

lnk2sym-unthumb()
{
sed 's/;/ ; /' | awk -f ${LIB}/lnk2sym-unthumb.awk 
}
