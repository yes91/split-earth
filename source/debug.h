#ifndef _debug_h_
#define _debug_h_

#define DEBUG_PRINT_BUFSIZE 256 

#ifdef DEBUG
#define debug_print(fmt, ...) gba_printf((fmt), __VA_ARGS__)
#else
#define debug_print(fmt, ...)
#endif

char* gba_printf(const char* fmt, ...) __attribute__((format(printf,1,2)));

#endif
