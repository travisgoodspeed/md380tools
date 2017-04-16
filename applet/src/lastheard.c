/*
 *  lastheard.c
 * 
 */

#include "lastheard.h"

#include <stdarg.h>
#include "printf.h"
#include "dmesg.h"
#include "console.h"

#define LASTHEARD_SIZE 1500
#define SLOG_SIZE 1500
#define CLOG_SIZE 1500

#define LH_MAXLINES 20
#define SMAXLINES 10
#define CMAXLINES 10

int lh_lines[LH_MAXLINES];
int slog_lines[SMAXLINES];
int clog_lines[CMAXLINES];

int lh_line_poi = 0 ;
int lh_first_char = 1 ;
int slog_line_poi = 0 ;
int slog_first_char = 1 ;
int clog_line_poi = 0 ;
int clog_first_char = 1 ;

char lastheard_buf[LASTHEARD_SIZE];
char slog_buf[SLOG_SIZE];
char clog_buf[CLOG_SIZE];

static int lh_end = 0 ;
static int lh_begin = 0 ;

static int sh_end = 0 ;
static int sh_begin = 0 ;

static int ch_end = 0 ;
static int ch_begin = 0 ;


static int lastheard_redraw_flag = 0 ;
static int slog_redraw_flag = 0 ;
static int clog_redraw_flag = 0 ;

int inline lh_wrap( int idx )
{
    if( idx >= LASTHEARD_SIZE ) {
        idx = 0 ;
    }
    return idx ;
}

int inline slog_wrap( int idx )
{
    if( idx >= SLOG_SIZE ) {
        idx = 0 ;
    }
    return idx ;
}

int inline clog_wrap( int idx )
{
    if( idx >= CLOG_SIZE ) {
        idx = 0 ;
    }
    return idx ;
}


void lastheard_line_begin( int idx, char c )
{
    if( lh_first_char ) {
        lh_lines[lh_line_poi] = idx ;
        lh_line_poi++ ;
        lh_line_poi %= LH_MAXLINES ;
        lh_first_char = 0 ;
    }
    if( c == '\n' ) {
        lh_first_char = 1 ;
    }
}

void lastheard_putch( char c )
{
    if( c == '\n' ) {
    } else if ( c < ' ' || c >= 127 ) {
        c = '.' ;
    }
    lastheard_buf[lh_end] = c ;
    lastheard_line_begin(lh_end,c);
    lh_end = lh_wrap(lh_end+1);
    if( lh_end == lh_begin ) {
        // full.
        lh_begin = lh_wrap(lh_begin+1);
    }
    lastheard_redraw_flag = 1 ;
}

static void lastheard_prch(void* p, char c)
{
    lastheard_putch(c);
}

void lastheard_printf(const char *fmt, ...)
{
    if( !is_lastheard_enabled() ) {
        return ;
    }
    
    va_list lh_va;
    va_start(lh_va, fmt);
    tfp_format(0, lastheard_prch, fmt, lh_va);
    va_end(lh_va);
}

void lastheard_dump_dmesg()
{
    int i = lh_begin ;
    while(1) {
        i = lh_wrap(i);
        if( i == lh_end ) {
            break ;
        }
        md380_putc(0,lastheard_buf[i]);
        i++ ;
    }
}

void lastheard_redraw()
{
    lastheard_redraw_flag = 1 ;
}

void lastheard_draw_poll()
{
    if( !lastheard_redraw_flag ) {
        return ;
    }
    
    int con_height = 10 ;
    
    lastheard_redraw_flag = 0 ;
    
    con_clrscr();
    
    int idx = lh_line_poi ;
    idx += LH_MAXLINES ;
    idx -= con_height ;
    idx %= LH_MAXLINES ;
    
    int pos = lh_lines[idx];
    
    int i = pos ;
    int linecnt = 0 ;
//    int i = begin ;

    while(1) {
        i = lh_wrap(i);
        if( i == lh_end ) {
            break ;
        }
        char c = lastheard_buf[i];
        if( c == '\n' ) {
            linecnt++ ;
            con_nl();
        } else {
            con_printc(c);            
        }
        i++ ;
    }
    for(int i=linecnt;i<con_height;i++) {
        con_nl();        
    }
}

void lastheard_clear()
{
    lh_end = 0 ;
    lh_begin = 0 ;
    lh_line_poi = 0 ;
    for(int i=0;i<LH_MAXLINES;i++) {
        lh_lines[i] = 0 ;
    }
}

void slog_line_begin( int idx, char c )
{
    if( slog_first_char ) {
        slog_lines[slog_line_poi] = idx ;
        slog_line_poi++ ;
        slog_line_poi %= SMAXLINES ;
        slog_first_char = 0 ;
    }
    if( c == '\n' ) {
        slog_first_char = 1 ;
    }
}

void slog_putch( char c )
{
    if( c == '\n' ) {
    } else if ( c < ' ' || c >= 127 ) {
        c = '.' ;
    }
    slog_buf[sh_end] = c ;
    slog_line_begin(sh_end,c);
    sh_end = slog_wrap(sh_end+1);
    if( sh_end == sh_begin ) {
        // full.
        sh_begin = slog_wrap(sh_begin+1);
    }
    slog_redraw_flag = 1 ;
}

static void slog_prch(void* p, char c)
{
    slog_putch(c);
}

void slog_printf(const char *fmt, ...)
{
    if( !is_slog_enabled() ) {
        return ;
    }
    
    va_list va;
    va_start(va, fmt);
    tfp_format(0, slog_prch, fmt, va);
    va_end(va);
}

void slog_dump_dmesg()
{
    int i = sh_begin ;
    while(1) {
        i = slog_wrap(i);
        if( i == sh_end ) {
            break ;
        }
        md380_putc(0,slog_buf[i]);
        i++ ;
    }
}

void slog_redraw()
{
    slog_redraw_flag = 1 ;
}

void slog_draw_poll()
{
    if( !slog_redraw_flag ) {
        return ;
    }
    
    int con_height = 10 ;
    
    slog_redraw_flag = 0 ;
    
    con_clrscr();
    
    int idx = slog_line_poi ;
    idx += SMAXLINES ;
    idx -= con_height ;
    idx %= SMAXLINES ;
    
    int pos = slog_lines[idx];
    
    int i = pos ;
    int linecnt = 0 ;
//    int i = begin ;
    while(1) {
        i = slog_wrap(i);
        if( i == sh_end ) {
            break ;
        }
        char c = slog_buf[i];
        if( c == '\n' ) {
            linecnt++ ;
            con_nl();
        } else {
            con_printc(c);            
        }
        i++ ;
    }
    for(int i=linecnt;i<con_height;i++) {
        con_nl();        
    }
}

void slog_clear()
{
    sh_end = 0 ;
    sh_begin = 0 ;
    slog_line_poi = 0 ;
    for(int i=0;i<SMAXLINES;i++) {
        slog_lines[i] = 0 ;
    }
}

void clog_line_begin( int idx, char c )
{
    if( clog_first_char ) {
        clog_lines[clog_line_poi] = idx ;
        clog_line_poi++ ;
        clog_line_poi %= CMAXLINES ;
        clog_first_char = 0 ;
    }
    if( c == '\n' ) {
        clog_first_char = 1 ;
    }
}

void clog_putch( char c )
{
    if( c == '\n' ) {
    } else if ( c < ' ' || c >= 127 ) {
        c = '.' ;
    }
    clog_buf[ch_end] = c ;
    clog_line_begin(ch_end,c);
    ch_end = clog_wrap(ch_end+1);
    if( ch_end == ch_begin ) {
        // full.
        ch_begin = clog_wrap(ch_begin+1);
    }
    clog_redraw_flag = 1 ;
}

static void clog_prch(void* p, char c)
{
    clog_putch(c);
}

void clog_printf(const char *fmt, ...)
{
    if( !is_clog_enabled() ) {
        return ;
    }
    
    va_list va;
    va_start(va, fmt);
    tfp_format(0, clog_prch, fmt, va);
    va_end(va);
}

void clog_dump_dmesg()
{
    int i = ch_begin ;
    while(1) {
        i = clog_wrap(i);
        if( i == ch_end ) {
            break ;
        }
        md380_putc(0,clog_buf[i]);
        i++ ;
    }
}

void clog_redraw()
{
    clog_redraw_flag = 1 ;
}

void clog_draw_poll()
{
    if( !clog_redraw_flag ) {
        return ;
    }
    
    int con_height = 10 ;
    
    clog_redraw_flag = 0 ;
    
    con_clrscr();
    
    int idx = clog_line_poi ;
    idx += CMAXLINES ;
    idx -= con_height ;
    idx %= CMAXLINES ;
    
    int pos = clog_lines[idx];
    
    int i = pos ;
    int linecnt = 0 ;
//    int i = begin ;
    while(1) {
        i = clog_wrap(i);
        if( i == ch_end ) {
            break ;
        }
        char c = clog_buf[i];
        if( c == '\n' ) {
            linecnt++ ;
            con_nl();
        } else {
            con_printc(c);            
        }
        i++ ;
    }
    for(int i=linecnt;i<con_height;i++) {
        con_nl();        
    }
}

void clog_clear()
{
    ch_end = 0 ;
    ch_begin = 0 ;
    clog_line_poi = 0 ;
    for(int i=0;i<CMAXLINES;i++) {
        clog_lines[i] = 0 ;
    }
}

