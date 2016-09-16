
int mode ; // 0x2001e94d

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
                if( q != 12 ) {
                    if( q != 0 ) {
                        q = 7 ;
                        r = 21 ;
                        Create_MainMenyEntry(...);
                        // ....
                    }
                }
                break ;
            case 22 :
                if( q != 12 ) {
                    if( q != 0 ) {
                        
                    }
                }
                break ;
            case 27 :
                if( q != 12 ) {
                    r = 27 ;
                    if( q == 5 ) {
                      // 0x0802062e
                      s = 1000 ;
                    } else if( q != 6 ) {
                        q = 10 ;
                        if( 0x2001e8ed != 0 ) {
                            // 0x08020656
                            s = 1000 ;
                            *0x20004ab2 = radio_config->off22 * 500 ;
                        } else {
                            // 0x08020676
                            if( *0x2001e574 == 0 ) {
                                *0x2001e574 = 255 ;
                            }
                            *0x2001e844 = *0x2001e574 * 100 ;
                            *0x20004ab2 = radio_config->off22 * 500 ;
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
        if( 0x2046f8a1[21] == 0 ) {
            if( 0x2001e7f8 = 0 ) {
                if( q != 12 ) {
                    // 0x0802084e
                    // ...
                }
            }
        }
        // 0x08020870 
        //...
             
        // 0x080208a6
        switch( q ) {
            case 1 :
                break ; //?
            case 2 :
            case 4 :
                break ; //?
            case 5 :
                break ; //?
            case 6 :
                break ; //?
            case 7 :
                // 0x080208da
                s = s - 1 ;
                if( s == 0 ) {
                    // 0x080208f2
                    s = 200 ;
                    0x080214fe();
                } else {
                    // 0x08020900
                    if( s == 100 ) {
                        0x080215fc();
                    }
                }
                return ;
            case 10 :
                // menu.
                // ...
                // 0x08020a7c
                s = s - 1 ;
                if( s == 0 ) {
                    q = 1 ;
                    s = 5 ;
                    0x0800fc96();
                }
                return ;
            case 13 :
                break ; //?
            default:
                return ;
        }
    }
}

{
    0x08031f1c(...);
    *0x0804758c = 4 ;
    *0x08046d20 = 500 ;
    OSTimeDly(1000):
    OSTimeDly(1000):
    // ...
    keyboard_maybe();
    0x0804fd04();
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

loop2()
{
    F_4225();
    OSTimeDly(10);
    if( *0x2001e892 == 2 ) {
        
        if( *0x2001e8b8 == 5/6/8/9/10/11 ) {
   |`````-> 0x0802db94    dff8980b     ldr.w r0, [pc, 2968] ; 0x0802e730                                                                                                       
   |        0x0802db98    0088         ldrh r0, [r0, 0]                                                                                                                        
   |        0x0802db9a    f4f7d2ff     bl 0x08022b42 ;[8]                                                                                                                      
   |           0x00822b42()                                                                                                                                                    
   |        0x0802db9e    dff8940b     ldr.w r0, [pc, 2964] ; 0x0802e734                                                                                                       
   |        0x0802dba2    dff88c1b     ldr.w r1, [pc, 2956] ; 0x0802e730                                                                                                       
   |        0x0802dba6    0988         ldrh r1, [r1, 0]                                                                                                                        
   |        0x0802dba8    0180         strh r1, [r0, 0]                            
        } else {
  |`------> 0x0802dbac    dff8e003     ldr.w r0, [pc, 992] ; 0x0802df90                                                                                                        
  |         0x0802dbb0    0078         ldrb r0, [r0, 0]                                                                                                                        
  |         0x0802dbb2    0428         cmp r0, 4                                                                                                                               
  ========< 0x0802dbb4    0bd1         bne.n 0x0802dbce ;[9]                  
        }
        
    } else {
        
    }
}

?()
{
    if( *0x20004acc == 0xFF ) {
        Create_MainMenyEntry();
    }
}