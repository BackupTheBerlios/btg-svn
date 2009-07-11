#ifndef GNUTLS_MACRO_H
#define GNUTLS_MACRO_H

#include <bcore/logmacro.h>

#define GNUTLS_NOTICE(_TEXT) { \
  if (BTG_TRANSPORT_DEBUG == 1) \
  { \
     BTG_NOTICE(_TEXT); \
  } \
}

#define printerror(str, ret) { \
if (ret < 0) \
{ \
   GNUTLS_NOTICE("Error while executing " << str << ":" << gnutls_strerror(ret)); \
} \
}

#endif // GNUTLS_MACRO_H
