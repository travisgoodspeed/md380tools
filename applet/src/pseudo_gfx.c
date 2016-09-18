
// r0 = str, r1 = sx, r2 = sy, r3 = x, r4/sp[28] = y, r5/sp[24] = maxlen
gfx_drawtext(wchar_t *str, short sx, short sy, short x, short y, int maxlen)
{
    void p1 ;
    f1(str,&p1,14,y,maxlen,...?);
}

        
// r0 = string in ASCII (char index in table?)
gfx_drawtext8( char *r0 )
{
    if( r0 == 0 ) {
        return ;
    }
    // ...
    if( gfx_info.off44 & 3 > 2 ) {
        gfx_info.ypos = gfx_info.ypos2 ;        
    } else {
        gfx_info.ypos = r5 ;
    }
    if( c == '\n' ) {
        gfx_info.ypos += r4 + 1 ;
    }
        
    //
    gfx_info.ypos += r4 + 1 ;
    //
    gfx_info.xpos += r6 ;
    
    gfx_info.off44 &= ~3 ;
}

// r0 = str, r1 = x, r2 = y, r3 = xlen
void gfx_chars_to_display(wchar_t *str, int x, int y, int xlen)
{
    gfx_drawtext6( str, x, y, 18);
    gfx_clear3( 18 );
}

void gfx_drawtext4(wchar_t *str, int x, int y, int xlen, int ylen?)
{
    gfx_drawtext6( str, x, y, ylen);    
    gfx_clear3( xlen );
}

void gfx_drawtext5( char *r0, int r1, int r2, int r3 )
{
    gfx_drawtext7( *r0, r1, r2 );
    gfx_clear3( r3 );
}

void gfx_drawtext7( char *r0, int r1, int r2 )
{
    gfx_info[34] = r1 ; // xpos
    gfx_info[36] = r2 ; // ypos
    gfx_drawtext8(r0); // sole caller
}

