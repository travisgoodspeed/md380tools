
BEGIN {
    S = " " ;
}

{
    a = strtonum($3);
    addr = sprintf("%08x",a);
    print addr S $1 
}
