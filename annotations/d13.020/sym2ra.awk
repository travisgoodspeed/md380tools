
BEGIN {
    S = " " ;
}

{
    a = strtonum("0x" $1);
    sym = $2 ;
    addr = sprintf("0x%08x",a);

    print "f" S sym S "@" S addr S 
    if( a < 536870912 ) {
        print "af+" S addr S 4 S sym 
    }
}
