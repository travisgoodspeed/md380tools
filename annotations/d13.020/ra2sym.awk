BEGIN {
    S = " " 
}

function cnv( s )
{
    return strtonum(s);
}

$1 == "af+" {  
    a = cnv($2);
    addr[a] = $4 ;
    len[a] = $3 ;
#    printf( "%08x", cnv($2) ); 
#    print S "F" S $4 S $3 
}

$1 == "f" {  
    a = cnv($4);
    if( a in addr ) {
        next ;
    }
    addr[a] = $2 ;
#    printf( "%08x", cnv($4) ); 
#    print S "D" S $2 
}


END {
    for( a in addr ) {
        printf( "%08x", a ); 
        print S addr[a] S len[a] ;
    }
}
