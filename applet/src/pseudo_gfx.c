
// r0 = str, r1 = sx, r2 = sy, r3 = x, r4/sp[28] = y, r5/sp[24] = maxlen
gfx_drawtext(wchar_t *str, short sx, short sy, short x, short y, int maxlen)
{
    void p1 ;
    f1(str,&p1,14,y,maxlen,...?);
}

        
// r0 = str, r1 = x, r2 = y, r3 = xlen
void gfx_chars_to_display(wchar_t *str, int x, int y, int xlen)
{
    gfx_drawtext6( str, x, y, 18);
    gfx_clear3( xlen, x, y, 18 );
}

void gfx_drawtext4(wchar_t *str, int x, int y, int xlen, int ylen?)
{
    gfx_drawtext6( str, x, y, ylen);    
    gfx_clear3( xlen, x, y, ylen );
}