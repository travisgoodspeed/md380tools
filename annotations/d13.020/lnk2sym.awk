
BEGIN {
    S = " " ;
}

$2 == "=" {
    a = strtonum($3);
    if( a < 536870912 ) {
        a = and(a, compl(1)); # unthumb
    }
    addr = sprintf("%08x",a);
    print addr S $1 
}
