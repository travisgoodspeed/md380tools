BEGIN {
    S = " " 
}

function cnv( s )
{
    return strtonum(s);
}

$1 ~ /af\+/ {  
    printf( "%08x", cnv($2) ); 
    print S "F" S $4 S $3 
}



