
BEGIN {
    S = " " ;
}

{
    a = strtonum("0x" $1);
    sym = $2 ;
    addr = sprintf("0x%08x",a);

    printf( "%-40s = %10s ;\n", sym, addr ); 
}
