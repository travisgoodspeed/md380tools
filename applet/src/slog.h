/*
 *  slog.h
 * 
 */

#ifndef SLOG_H
#define SLOG_H

#ifdef __cplusplus
extern "C" {
#endif


inline int is_slog_enabled()
{
    return 1 ;
}

void slog_printf(const char *fmt, ...);

// radio
#define SLOGR(...) slog_printf(__VA_ARGS__)

// gui
#define SLOGG(...) //slog_printf(__VA_ARGS__)

// boot/setup
#define SLOGB(...) slog_printf(__VA_ARGS__)

// template for others
#define SLOGx(...) slog_printf(__VA_ARGS__)

void slog_dump_dmesg();
void slog_clear();

void slog_redraw();
void slog_draw_poll();

#ifdef __cplusplus
}
#endif

#endif /* SLOG_H */

