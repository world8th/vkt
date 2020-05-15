
#ifndef GLBINDING_TEMPLATE_API_H
#define GLBINDING_TEMPLATE_API_H

#include <glbinding/glbinding_export.h>

#ifdef GLBINDING_STATIC_DEFINE
#  define GLBINDING_TEMPLATE_API
#else
#  ifndef GLBINDING_TEMPLATE_API
#    ifdef GLBINDING_EXPORTS
        /* We are building this library */
#      define GLBINDING_TEMPLATE_API
#    else
        /* We are using this library */
#      define GLBINDING_TEMPLATE_API
#    endif
#  endif

#endif

#endif
