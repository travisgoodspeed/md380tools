BEGIN {
    init = 1 ;
    S = " " ;
}
END {
    print ""
    print "# from link file >>>"
    for( a in symbols ) {
        addr = sprintf("0x%08x",a);
        sym = symbols[a];
        if( a < 536870912 ) {
            # func
            if( a in found_af ) {
            } else {
                print "af+" S addr S 4 S sym
            }                
        } 
          
        if( a in found_f ) {
        } else {
            print "f" S sym S "@" S addr
        }
    }
    print "# from link file <<<"
}

$0 ~ /MARKER/ {
    init = 0 ;
    next ;
}

init == 1 {
    a = strtonum("0x"$1);
    symbols[a] = $2 ;
    next ;
}

$1 == "af+" { 
#print $2
    a = strtonum($2);
    if( a in symbols ) {
        $4 = symbols[a];
        found_af[a] = 1 ;
#print "hit" OFS $4 OFS a       
    }
    print $0 ;
    next ;
}

$1 == "f" {  
    a = strtonum($4);
    if( a in symbols ) {
        $2 = symbols[a];
        found_f[a] = 1 ;
    }
    print $0 ;
    next ;
}

{
    print $0 ;
}
