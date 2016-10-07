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

#define LOGR(...) syslog_printf(__VA_ARGS__)
#define LOGx(...) syslog_printf(__VA_ARGS__)

void syslog_dump_dmesg();
void syslog_dump_console();
void syslog_clear();

#ifdef __cplusplus
}
#endif

#endif /* SYSLOG_H */

