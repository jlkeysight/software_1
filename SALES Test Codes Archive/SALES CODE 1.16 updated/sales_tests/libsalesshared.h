#ifndef AG3070SALES_H
#define AG3070SALES_H

#ifdef __WIN32__
  #ifdef _LIBSALESSHARED_EXPORTS_
    #define _LIBSALESSHARED_API_ __declspec(dllexport)
  #elif _LIBSALESSHARED_IMPORTS_
    #define _LIBSALESSHARED_API_ __declspec(dllimport)
  #else
    #define _LIBSALESSHARED_API_
  #endif
#else
  #define _LIBSALESSHARED_API_
#endif

#endif
