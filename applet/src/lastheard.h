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

void lastheard_printf(const char *fmt, ...);

// lastheard
#define LOGLH(...) lastheard_printf(__VA_ARGS__)

void lastheard_dump_dmesg();
void lastheard_clear();

void lastheard_redraw();
void lastheard_draw_poll();

#ifdef __cplusplus
}
#endif

#endif /* LASTHEARD_H */
