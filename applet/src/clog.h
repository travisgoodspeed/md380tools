/*
 *  clog.h
 * 
 */

#ifndef CLOG_H
#define CLOG_H

#ifdef __cplusplus
extern "C" {
#endif


inline int is_clog_enabled()
{
    return 1 ;
}

void clog_printf(const char *fmt, ...);

// radio
#define CLOGR(...) clog_printf(__VA_ARGS__)

// gui
#define CLOGG(...) //clog_printf(__VA_ARGS__)

// boot/setup
#define CLOGB(...) clog_printf(__VA_ARGS__)

// template for others
#define CLOGx(...) clog_printf(__VA_ARGS__)

void clog_dump_dmesg();
void clog_clear();

void clog_redraw();
void clog_draw_poll();

#ifdef __cplusplus
}
#endif

#endif /* CLOG_H */

