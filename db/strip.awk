function trim(s)
{
    gsub(/^[ \t]+|[ \t]+$/,"",s);
    return s ;
}

BEGIN {
    T = ","
}
{ 
print trim($1) T trim($2) T trim($3) T trim($5) T trim($6) T trim($4) T trim($7)
}

