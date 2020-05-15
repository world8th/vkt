
#ifndef GLBINDING_AUX_API_H
#define GLBINDING_AUX_API_H

#ifdef GLBINDING_AUX_STATIC_DEFINE
#  define GLBINDING_AUX_API
#  define GLBINDING_AUX_NO_EXPORT
#else
#  ifndef GLBINDING_AUX_API
#    ifdef glbinding_aux_EXPORTS
        /* We are building this library */
#      define GLBINDING_AUX_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define GLBINDING_AUX_API __declspec(dllimport)
#    endif
#  endif

#  ifndef GLBINDING_AUX_NO_EXPORT
#    define GLBINDING_AUX_NO_EXPORT 
#  endif
#endif

#ifndef GLBINDING_AUX_DEPRECATED
#  define GLBINDING_AUX_DEPRECATED __declspec(deprecated)
#endif

#ifndef GLBINDING_AUX_DEPRECATED_EXPORT
#  define GLBINDING_AUX_DEPRECATED_EXPORT GLBINDING_AUX_API GLBINDING_AUX_DEPRECATED
#endif

#ifndef GLBINDING_AUX_DEPRECATED_NO_EXPORT
#  define GLBINDING_AUX_DEPRECATED_NO_EXPORT GLBINDING_AUX_NO_EXPORT GLBINDING_AUX_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef GLBINDING_AUX_NO_DEPRECATED
#    define GLBINDING_AUX_NO_DEPRECATED
#  endif
#endif

#endif /* GLBINDING_AUX_API_H */
