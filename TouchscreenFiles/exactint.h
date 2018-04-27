#ifndef EXACTINT_H /* header guard for "exactint.h" */
#define EXACTINT_H /* http://en.wikipedia.org/wiki/Include_guard */

/**
 * @see <a href="http://predef.sourceforge.net/prestd.html">Pre-defined Standard Macros</a>
 * @see <a href="http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1124.pdf">n1124</a>
 */

# if defined(__STDC__)
#  define PREDEF_STANDARD_C_1989
#  if defined(__STDC_VERSION__)
#   define PREDEF_STANDARD_C_1990
#   if (__STDC_VERSION__ >= 199409L)
#    define PREDEF_STANDARD_C_1994
#   endif
#   if (__STDC_VERSION__ >= 199901L)
#    define PREDEF_STANDARD_C_1999
#   endif
#  endif
# endif

# if defined PREDEF_STANDARD_C_1999 /* C99 */

#  include <stdint.h> /* use C99's exact-width integer typedefs */

# else /* tailor these for your platform */

typedef unsigned char uint8_t;
typedef   signed char  int8_t;
typedef unsigned int  uint16_t;
typedef   signed int   int16_t;
typedef unsigned long uint32_t;
typedef   signed long  int32_t;

# endif /* PREDEF_STANDARD_C_1999 */

#endif /* EXACTINT_H */
