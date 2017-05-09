/*
 *  syslog.h
 * 
 */

#ifndef SYSLOG_H
#define SYSLOG_H

#ifdef __cplusplus
extern "C" {
#endif


inline int is_syslog_enabled()
{
    return 1 ;
}

void syslog_printf(const char *fmt, ...);

// radio
#define LOGR(...) syslog_printf(__VA_ARGS__)

// gui
#define LOGG(...) //syslog_printf(__VA_ARGS__)

// boot/setup
#define LOGB(...) syslog_printf(__VA_ARGS__)

// template for others
#define LOGx(...) syslog_printf(__VA_ARGS__)

void syslog_dump_dmesg();
void syslog_clear();

void syslog_redraw();
void syslog_draw_poll();


#ifdef __cplusplus
}
#endif

#endif /* SYSLOG_H */

