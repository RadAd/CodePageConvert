#pragma once

#include <tchar.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
#define ARG_OPTIONAL(x) = x
#else
#define ARG_OPTIONAL(x)
#endif

    void arginit(int argc, const TCHAR* const argv[], const TCHAR* argdescription ARG_OPTIONAL(NULL));
    const TCHAR* argapp();
    bool argcleanup();
    bool argusage(bool bforce ARG_OPTIONAL(FALSE));

    bool argswitch(const TCHAR* argf, const TCHAR* desc);
    const TCHAR* argvalue(const TCHAR* argf, const TCHAR* def, const TCHAR* descvalue, const TCHAR* desc);
    int argvalueint(const TCHAR* argf, int def, const TCHAR* descvalue, const TCHAR* desc);
    const TCHAR* argnum(int i, const TCHAR* def, const TCHAR* descvalue, const TCHAR* desc);
    void argoptional();

#ifdef __cplusplus
}
#endif
