
{
    last[NR] = $0 ;
}

/c5000_spi0_writereg/ && last[NR-1] ~ /, 0x60/ {
    print last[NR-1]
    print $0 
}
