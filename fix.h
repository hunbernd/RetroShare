#if !defined(ssize_t) && !defined(__MINGW32__) && !defined(__MINGW64__)
#if defined(_WIN64) 
typedef __int64 ssize_t;
#else
typedef long ssize_t;
#endif
#endif