#pragma warning(push)   // Save the current warning state
#pragma warning(disable:4100) //unreference parameter in functions
#pragma warning(disable:4101)  //unreference local variables
#pragma warning(disable:4189)  //local variable is initialized but not referenced"
#include <ntddk.h>
#include <ntstatus.h>
#define NTSTRSAFE_NO_UNICODE_STRING_FUNCTIONS 
#include <ntstrsafe.h>
#include "precomp.h"

int
xf_strlen(const char *s)
{
    size_t len = 0;
    return (int)(NT_SUCCESS(RtlStringCbLengthA((const char *)s, (XF_STRSAFE_MAX_CCH*sizeof(CHAR)), &len))?len:0);
}

int 
xf_snprintf(char *buf, int n, const char *fmt, ...)
{
    NTSTATUS _s;
    va_list va;
    va_start(va, fmt);
    _s = RtlStringCbVPrintfA(buf, (size_t) n, fmt, va);
    if (!NT_SUCCESS(_s))
    {
        return 0;
    }
    va_end(va);
    return (xf_strlen(buf));
}

void 
xf_vaprintf(const char *fmt, ...) 
{
    char buf[XGE_TEMP_BUFFER_SIZE] = {0};
    NTSTATUS _s;
    va_list va;

    va_start(va, fmt);
    _s = RtlStringCbVPrintfA(buf, sizeof(buf), fmt, va);
    if (!NT_SUCCESS(_s))
    {
        return;
    }
    va_end(va);
}


#pragma warning(pop)   // Save the current warning state

