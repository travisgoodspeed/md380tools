
int mode ;

 * sym q = 0x2001e94b (0x08020bf4);
 * sym r = 0x2001e94c (0x08020cd0);
 * sym s = 0x2001e844 (0x08020cd8);

void f4225()
{
    // change detection.
    if( mode >= 0x80 ) {
        // new mode.
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
    } else {
        // mode is stable
        // do drawing.
    }
}

{
    F_4520();
    0x0800d69c();
    0x08036fbc( 2, *0x2001e510 );
    0x0801e5f4( 50 );
    f4225();
    0x080384d4();
    0x08032536();
    ? = 0x0804fdf4();

            0x08046ab0    9f49         ldr r1, [pc, 636] ; (0x08046d30)                                                                                                        
            0x08046ab2    a048         ldr r0, [pc, 640] ; (0x08046d34)                                                                                                        
            0x08046ab4    0068         ldr r0, [r0, 0]                                                                                                                         
            0x08046ab6    eaf771fb     bl OSMboxPost ;[1]                                                                                                                      

    c5000_spi0_writereg( 0x0E, 68 );
            
    c5000_spi0_writereg( 96, 0 );
}

if( *0x20004acc == 0xFF ) {
    Create_MainMenyEntry();
}
