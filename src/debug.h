#ifndef DEBUG__GUARD_H
#define DEBUG__GUARD_H

#define DEBUG_LEVEL 1

#define DEBUG(level, args...) DEBUG##level(args)

#if DEBUG_LEVEL >= 3
    #define DEBUG3(args...) printf(args)
#else
    #define DEBUG3(args...)
#endif
#if DEBUG_LEVEL >= 2
    #define DEBUG2(args...) printf(args)
#else
    #define DEBUG2(args...)
#endif
#if DEBUG_LEVEL >= 1
    #define DEBUG1(args...) printf(args)
#else
    #define DEBUG1(args...)
#endif

#endif
