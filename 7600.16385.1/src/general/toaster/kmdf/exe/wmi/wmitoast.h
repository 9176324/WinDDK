#include <windows.h>
#include <comutil.h>
#include <wbemcli.h>
#include <stdio.h>
#include <tchar.h>

#define ARRAY_SIZE(_X_)     (sizeof((_X_))/sizeof((_X_)[0]))


HRESULT
SetInterfaceSecurity(
    __in     IUnknown* InterfaceObj,
    __in_opt PWCHAR UserId,
    __in_opt PWCHAR Password,
    __in_opt PWCHAR DomainName
    );

HRESULT
ExecuteMethodsInClass(
    __in     IWbemServices* WbemServices,
    __in_opt PWCHAR UserId,
    __in_opt PWCHAR Password,
    __in_opt PWCHAR DomainName
    );

HRESULT
GetAndSetValuesInClass(
    __in     IWbemServices* WbemServices,
    __in_opt PWCHAR UserId,
    __in_opt PWCHAR Password,
    __in_opt PWCHAR DomainName
    );
