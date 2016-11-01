
sym2ra()
{
awk -f sym2ra.awk
}

ra2sym()
{
awk -f ra2sym.awk | sort
}

sym_strip()
{
awk '{ print $1 OFS $2 }'
}

lnk2sym()
{
sed 's/;/ ; /' | awk -f lnk2sym.awk | sort
}

