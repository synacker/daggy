
#ifndef DAGGYCORE_EXPORT_H
#define DAGGYCORE_EXPORT_H

#ifdef DAGGYCORE_STATIC_DEFINE
#  define DAGGYCORE_EXPORT
#  define DAGGYCORE_NO_EXPORT
#else
#  ifndef DAGGYCORE_EXPORT
#    ifdef DaggyCore_EXPORTS
        /* We are building this library */
#      define DAGGYCORE_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define DAGGYCORE_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef DAGGYCORE_NO_EXPORT
#    define DAGGYCORE_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef DAGGYCORE_DEPRECATED
#  define DAGGYCORE_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef DAGGYCORE_DEPRECATED_EXPORT
#  define DAGGYCORE_DEPRECATED_EXPORT DAGGYCORE_EXPORT DAGGYCORE_DEPRECATED
#endif

#ifndef DAGGYCORE_DEPRECATED_NO_EXPORT
#  define DAGGYCORE_DEPRECATED_NO_EXPORT DAGGYCORE_NO_EXPORT DAGGYCORE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef DAGGYCORE_NO_DEPRECATED
#    define DAGGYCORE_NO_DEPRECATED
#  endif
#endif

#endif /* DAGGYCORE_EXPORT_H */
