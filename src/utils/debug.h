#ifndef SIMULAZIONE_TRANSAZIONI_DEBUG_H
#define SIMULAZIONE_TRANSAZIONI_DEBUG_H

#include <stdarg.h>
#include <stdio.h>
/* kudos to https://stackoverflow.com/questions/1644868/define-macro-for-debug-printing-in-c/1644898#1644898?newreg=f3d84f0e4a0846b6ab71f2eac2374d70 */

#ifdef DEBUG
#define TRACE(x) do { if (DEBUG) dbg_printf x; } while (0)

void dbg_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}
#else
#define TRACE(x)
#endif

#endif /* SIMULAZIONE_TRANSAZIONI_DEBUG_H */