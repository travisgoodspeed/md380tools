
BEGIN {
    S = " " ;
}

{
    a = strtonum($3);
    addr = sprintf("0x%08x",a);
    print addr S $1 
}
