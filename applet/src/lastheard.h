/*
 *  lastheard.h
 * 
 */

#ifndef LASTHEARD_H
#define LASTHEARD_H

#ifdef __cplusplus
extern "C" {
#endif


inline int is_lastheard_enabled()
{
    return 1 ;
}

inline int is_clog_enabled()
{
    return 1 ;
}

inline int is_slog_enabled()
{
    return 1 ;
}

void lastheard_putch( char c );  // <- 2017-04-17 .. called without a proper prototype, aka "implicit declaration". Always a bad thing.
void lastheard_printf(const char *fmt, ...);

void slog_putch( char c );  // <- "warning: implicit declaration of function 'slog_putch'" . No, thanks.
void slog_printf(const char *fmt, ...);

void clog_putch( char c );  // <- "warning: implicit declaration of function 'clog_putch'" . Nevermore !
void clog_printf(const char *fmt, ...);

// lastheard
#define LOGLH(...) lastheard_printf(__VA_ARGS__)

void lastheard_dump_dmesg();
void clog_dump_dmesg();
void slog_dump_dmesg();

void lastheard_clear();
void clog_clear();
void slog_clear();

void lastheard_redraw();
void slog_redraw();
void clog_redraw();

void lastheard_draw_poll();
void clog_draw_poll();
void slog_draw_poll();

// radio
#define CLOGR(...) clog_printf(__VA_ARGS__)
#define SLOGR(...) slog_printf(__VA_ARGS__)

// gui
#define CLOGG(...) //clog_printf(__VA_ARGS__)
#define SLOGG(...) //slog_printf(__VA_ARGS__)

// boot/setup
#define CLOGB(...) clog_printf(__VA_ARGS__)
#define SLOGB(...) slog_printf(__VA_ARGS__)

// template for others
#define CLOGx(...) clog_printf(__VA_ARGS__)
#define SLOGx(...) slog_printf(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* LASTHEARD_H */
