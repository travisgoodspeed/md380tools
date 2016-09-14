
int mode ;

 * sym q = 0x2001e94b (0x08020bf4);
 * sym r = 0x2001e94c (0x08020cd0);
 * sym s = 0x2001e844 (0x08020cd8);

void f4225()
{
    // change detection.
    if( mode >= 0x80 ) {
        mode &= 0x7F ;
        switch( mode ) {
            case 16 :
            case 17 :
            case 18 :
            case 19 :
                if( q != 2 && q != 4 ) {
                    // ...
                }
                // ...
                break ;
            case 20 :
            case 21 :
            case 22 :
            case 27 :
                if( q != 12 ) {
                   r = 27 ;
                   if( q == 5 ) {
                      // 0x0802062e
                      s = 1000 ;
                   } else if( q != 6 ) {
                      q = 10 ;
                      if( 0x0x080213c0 != 0 ) {
                         // 0x08020656
                         s = 1000 ;
                      }
                   } else {
                      // do_something();
                   }
                }
                break ;                
            case 28 :
                if( q != 12 ) {
                   r = 28 ;
                   q = 1 ;
                   s = 10 ;
                   do_something();
                }
                break ;
            case 29 :
                r = 29 ;
                q = 12 ;
                // someother();
                break ;
            case 30 :
            case 31 :
            case 32 :
            case 33 :
            case 35 :
            case 36 :
            default :                
        }
    }
}


if( *0x20004acc == 0xFF ) {
    Create_MainMenyEntry();
}
