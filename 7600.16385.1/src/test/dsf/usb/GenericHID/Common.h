/*++ BUILD Version: 0000    

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    Common.h

Abstract:
    Define common macros which are use through out the project.
 
Revision History:

--*/
#pragma once

//Macros to have help only one exit point.
#define IfFailHrGoto(EXPR, LABEL) \
                                { hr = (EXPR); if(FAILED(hr)) goto LABEL; }
#define IfFailHrRet(EXPR) \
                        { hr = (EXPR); if(FAILED(hr)) return(hr); }

#define IfFailHrGo(EXPR) IfFailHrGoto(EXPR, Exit)


#define IfFalseHrGoto(EXPR, HR, LABEL) \
                                     { if(!(EXPR)) { hr = (HR); goto LABEL; } }

#define IfFalseHrRet(EXPR, HR) \
                             { if(!(EXPR)) return(HR); }

#define IfFalseHrGo(EXPR, HR) IfFalseHrGoto(EXPR, HR, Exit)

//Release and Add ref macros
#define ADDREF(punk)\
{\
    if ((punk) != NULL)\
    {\
        (punk)->AddRef();\
    }\
}


#define RELEASE(punk)\
{\
    if ((punk) != NULL)\
    {\
        IUnknown *_punkXxx = (punk);\
        (punk) = NULL;\
        _punkXxx->Release();\
    }\
}
