#ifndef _TBLIS_CONFIG_H_
#define _TBLIS_CONFIG_H_

#include <stddef.h>

#define TBLIS_LEN_TYPE ptrdiff_t
#define TBLIS_STRIDE_TYPE ptrdiff_t
#define TBLIS_LABEL_TYPE char

#if defined(__GNUC__)
#define TBLIS_RESTRICT __restrict__
#else
#define TBLIS_RESTRICT restrict
#endif

#endif

