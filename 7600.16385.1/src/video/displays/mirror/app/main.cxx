//
// Generic Windows program template
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <windows.h>

#include <winddi.h>
#include <tchar.h>
#include <winbase.h>
#include <winreg.h>
#include <StrSafe.h>

HINSTANCE appInstance;  // handle to the application instance

LPSTR driverName = "Microsoft Mirror Driver";

LPSTR dispCode[7] = {
   "Change Successful",
   "Must Restart",
   "Bad Flags",
   "Bad Parameters",
   "Failed",
   "Bad Mode",
   "Not Updated"};

LPSTR GetDispCode(INT code)
{
   switch (code) {
   
   case DISP_CHANGE_SUCCESSFUL: return dispCode[0];
   
   case DISP_CHANGE_RESTART: return dispCode[1];
   
   case DISP_CHANGE_BADFLAGS: return dispCode[2];
   
   case DISP_CHANGE_BADPARAM: return dispCode[3];
   
   case DISP_CHANGE_FAILED: return dispCode[4];
   
   case DISP_CHANGE_BADMODE: return dispCode[5];
   
   case DISP_CHANGE_NOTUPDATED: return dispCode[6];
   
   default:
      static char tmp[MAX_PATH];

      StringCbPrintf(&tmp[0], sizeof(tmp), "Unknown code: %08x\n", code);

      return (LPTSTR)&tmp[0];
   
   }
}

//
// Handle window repaint event
//

VOID
DoPaint(
    HWND hwnd
    )

{
    HDC hdc;
    PAINTSTRUCT ps;
    
    hdc = BeginPaint(hwnd, &ps);

    COLORREF red = 0x00FF0000;

    HBRUSH hbr = CreateSolidBrush(red);

    RECT r;
    r.left = ps.rcPaint.left;
    r.top = ps.rcPaint.top;
    r.right = ps.rcPaint.right;
    r.bottom = ps.rcPaint.bottom;

    FillRect(hdc, &r, hbr);

    EndPaint(hwnd, &ps);
}


//
// Window callback procedure
//

LRESULT CALLBACK
MyWindowProc(
    HWND    hwnd,
    UINT    uMsg,
    WPARAM  wParam,
    LPARAM  lParam
    )

{
    switch (uMsg)
    {
    case WM_PAINT:
        DoPaint(hwnd);
        break;

    case WM_DISPLAYCHANGE:
       {
        WORD cxScreen = LOWORD(lParam);
        WORD cyScreen = HIWORD(lParam);
        WPARAM format = wParam;

        UNREFERENCED_PARAMETER(cxScreen);
        UNREFERENCED_PARAMETER(cyScreen);
        UNREFERENCED_PARAMETER(format);
        
        // Add hook to re-initialize the mirror driver's surface

       }
       break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}


//
// Create main application window
//

HWND
CreateMyWindow(
   //********************************************************************
    PCSTR title
    )

#define MYWNDCLASSNAME "Mirror Sample"

{
   //********************************************************************

    //
    // Register window class if necessary
    //

    static BOOL wndclassRegistered = FALSE;

    if (!wndclassRegistered)
    {
        WNDCLASS wndClass =
        {
            0,
            MyWindowProc,
            0,
            0,
            appInstance,
            NULL,
            NULL,
            NULL,
            NULL,
            MYWNDCLASSNAME
        };

        RegisterClass(&wndClass);
        wndclassRegistered = TRUE;
    }

    HWND hwnd;
    INT width = 300, height = 300;

    //********************************************************************

    hwnd = CreateWindow(
                    MYWNDCLASSNAME,
                    title,
                    WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    width,
                    height,
                    NULL,
                    NULL,
                    appInstance,
                    NULL);

    return hwnd;
}


//
// Main program entrypoint
//

VOID _cdecl
main(
    __in               INT     argc,
    __in_ecount(argc)  CHAR  **argv
    )

{
    DWORD dwAttach = 0;
    BOOL  bED   = FALSE;
    BOOL  bTest = FALSE;
    BOOL  bWndObj = FALSE;
    appInstance = GetModuleHandle(NULL);

    if (argc == 2)
    {
        if (strcmp(argv[1],"-e") == 0 ) { bED = TRUE; dwAttach = 1; }
        if (strcmp(argv[1],"-d") == 0 ) { bED = TRUE; dwAttach = 0; }
        if (strcmp(argv[1],"-t") == 0 ) bTest = TRUE;
        if (strcmp(argv[1],"-w") == 0 ) bWndObj = TRUE; 
    }

    //
    // Create the main application window
    //
    //********************************************************************

    DEVMODE devmode;

    FillMemory(&devmode, sizeof(DEVMODE), 0);
    devmode.dmSize = sizeof(DEVMODE);
    devmode.dmDriverExtra = 0;

    // Make sure we have a display on this thread.
    BOOL change = EnumDisplaySettings(NULL,
                                      ENUM_CURRENT_SETTINGS,
                                      &devmode);

    devmode.dmFields = DM_BITSPERPEL |
                       DM_PELSWIDTH | 
                       DM_PELSHEIGHT |
                       DM_POSITION ;

    if (change) 
    {
        // query all display devices in the system until we hit a primary
        // display device. Using it get the width and height of the primary
        // so we can use that for the mirror driver. Also enumerate the
        // display devices installed on this machine untill we hit
        // our favourate mirrored driver, then extract the device name string
        // of the format '\\.\DISPLAY#'
   
        DISPLAY_DEVICE dispDevice;
   
        FillMemory(&dispDevice, sizeof(DISPLAY_DEVICE), 0);
   
        dispDevice.cb = sizeof(DISPLAY_DEVICE);
   
        LPSTR deviceName = NULL;

        devmode.dmDeviceName[0] = '\0';

        INT devNum = 0;
        BOOL result;
        DWORD cxPrimary = 0xFFFFFFFF;
        DWORD cyPrimary = 0xFFFFFFFF;

        // First enumerate for Primary display device:
        while ((result = EnumDisplayDevices(NULL,
                                devNum,
                                &dispDevice,
                                0)) == TRUE)
        {
          printf("DriverName: %s\n", &dispDevice.DeviceString[0]);
          if (dispDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
          {
              // Primary device. Find out its dmPelsWidht and dmPelsHeight.
              EnumDisplaySettings(dispDevice.DeviceName,
                                  ENUM_CURRENT_SETTINGS,
                                  &devmode);

              cxPrimary = devmode.dmPelsWidth;
              cyPrimary = devmode.dmPelsHeight;
              break;
          }
          devNum++;
        }

        // error check
        if (!result)
        {
           printf("No '%s' found.\n", driverName);
           exit(0);
        }

        if (cxPrimary == 0xffffffff || cyPrimary == 0xffffffff)
        {
            printf("cxPrimary or cyPrimary not valid\n");
            exit(0);
        }

        // Enumerate again for the mirror driver:
        devNum = 0;
        while ((result = EnumDisplayDevices(NULL,
                                  devNum,
                                  &dispDevice,
                                  0)) == TRUE)
        {
          if (strcmp(&dispDevice.DeviceString[0], driverName) == 0)
              break;

           devNum++;
        }

        // error check       
        if (!result)
        {
           printf("No '%s' found.\n", driverName);
           exit(0);
        }

        printf("DevNum:%d\nName:%s\nString:%s\nID:%s\nKey:%s\n\n",
               devNum,
               &dispDevice.DeviceName[0],
               &dispDevice.DeviceString[0],
               &dispDevice.DeviceID[0],
               &dispDevice.DeviceKey[0]);

        CHAR deviceNum[MAX_PATH];
        LPSTR deviceSub;

        // Simply extract 'DEVICE#' from registry key.  This will depend
        // on how many mirrored devices your driver has and which ones
        // you intend to use.

        _strupr(&dispDevice.DeviceKey[0]);

        deviceSub = strstr(&dispDevice.DeviceKey[0],
                           "\\DEVICE");

        if (!deviceSub) 
            StringCbCopy(&deviceNum[0], sizeof(deviceNum), "DEVICE0");
        else
            StringCbCopy(&deviceNum[0], sizeof(deviceNum), ++deviceSub);

        // Reset the devmode for mirror driver use:
        FillMemory(&devmode, sizeof(DEVMODE), 0);
        devmode.dmSize = sizeof(DEVMODE);
        devmode.dmDriverExtra = 0;

        devmode.dmFields = DM_BITSPERPEL |
                           DM_PELSWIDTH | 
                           DM_PELSHEIGHT |
                           DM_POSITION;

        StringCbCopy((LPSTR)&devmode.dmDeviceName[0], sizeof(devmode.dmDeviceName), "mirror");
        deviceName = (LPSTR)&dispDevice.DeviceName[0];

        if (bED)
        {
            // Attach and detach information is sent via the dmPelsWidth/Height
            // of the devmode.
            //
            if (dwAttach == 0)
            {
                devmode.dmPelsWidth = 0;
                devmode.dmPelsHeight = 0;
            }
            else
            {
                devmode.dmPelsWidth = cxPrimary;
                devmode.dmPelsHeight = cyPrimary;
            }

            // Update the mirror device's registry data with the devmode. Dont
            // do a mode change. 
            INT code =
            ChangeDisplaySettingsEx(deviceName,
                                    &devmode, 
                                    NULL,
                                    (CDS_UPDATEREGISTRY | CDS_NORESET),
                                    NULL
                                    );
            printf("Update Registry on device mode: %s\n", GetDispCode(code));

            // Now do the real mode change to take mirror driver changes into
            // effect.
            code = ChangeDisplaySettingsEx(NULL,
                                           NULL,
                                           NULL,
                                           0,
                                           NULL);
    
            printf("Raw dynamic mode change on device mode: %s\n", GetDispCode(code));
        }

        // If we need to test
        if (bTest)
        {
            HDC hdc = CreateDC("DISPLAY",
                               deviceName,
                               NULL,
                               NULL);

            // we should be hooked as layered at this point
            HDC hdc1 = CreateCompatibleDC(hdc);
            HBITMAP hbm1 = CreateCompatibleBitmap(hdc, 100, 100);
            SelectObject(hdc1, hbm1);

            // we should be hooked as layered at this point
            HDC hdc2 = CreateCompatibleDC(hdc);
            // call DrvCreateDeviceBitmap
            HBITMAP hbm2 = CreateCompatibleBitmap(hdc, 100, 100);
            SelectObject(hdc2, hbm2);
        
            //call DrvBitBlt
            BitBlt(hdc2, 0, 0, 50, 50, hdc1, 0, 0, SRCCOPY);

            POINT destPlg[3]; 
            destPlg[0].x = 50;  
            destPlg[0].y = 0; 
            destPlg[1].x = 100; 
            destPlg[1].y = 0; 
            destPlg[2].x = 0;    
            destPlg[2].y = 50; 

            //call DrvPlgBlt
            PlgBlt(hdc2, destPlg, hdc1, 0, 0, 50, 50, 0, 0, 0); 

            //call DrvLineTo
            MoveToEx(hdc2, 0, 0, (LPPOINT) NULL); 
            LineTo(hdc2, 60, 60);

            //call DrvStrokePath
            BeginPath(hdc2); 
            MoveToEx(hdc2, 0, 0, (LPPOINT) NULL); 
            LineTo(hdc2, 50, 50);
            EndPath(hdc2); 
            StrokePath(hdc2);

            BitBlt(hdc2, 0, 0, 50, 50, hdc, 0, 0, SRCCOPY);
            //call DrvFillPath
            BeginPath(hdc2); 
            MoveToEx(hdc2, 0, 0, (LPPOINT) NULL); 
            LineTo(hdc2, 50, 50);
            EndPath(hdc2); 
            SetPolyFillMode(hdc2, WINDING); 
            FillPath(hdc2);
            // delete the device context
            DeleteDC(hdc2);
            DeleteDC(hdc);
            //call DrvStrokeAndFillPath
            BeginPath(hdc2); 
            MoveToEx(hdc2, 0, 0, (LPPOINT) NULL); 
            LineTo(hdc2, 50, 50);
            EndPath(hdc2); 
            StrokeAndFillPath(hdc2); 

            //call DrvStretchBltROP
            StretchBlt(hdc2, 0, 0, 50, 50, hdc1, 50, 50, 25, 25, SRCCOPY);

            // delete the device context
            DeleteDC(hdc2);
            DeleteDC(hdc1);
            DeleteDC(hdc);
            printf("Performed bit blit.  Finished. \n");
        }

        if (bWndObj)
        {
            HWND hwnd = CreateMyWindow("Mirror Driver Tracking Window");

            if (hwnd)
            {
                HDC hdc = CreateDC(NULL, deviceName, NULL, NULL);

                if (hdc)
                {
                    ShowWindow (hwnd, SW_SHOW);
                    UpdateWindow(hwnd);

                    int ret = ExtEscape(hdc,
                                        WNDOBJ_SETUP,
                                        4,
                                        (LPCSTR)&hwnd,
                                        0,
                                        (LPSTR)NULL);

                    if (ret <= 0)
                    {
                        printf("ExtEscape: WNDOBJ_SETUP failed\n");
                    }
                    else
                    {
                        MSG msg;

                        while(GetMessage(&msg, NULL, 0, 0))
                        {
                            TranslateMessage(&msg);
                            DispatchMessage(&msg);
                        }
                    }
                }
                else
                {
                    printf("Mirror driver specific DC creation failed\n");
                }
            }
            else
            {
                printf("Hwnd creation failed\n");
            }
        }

        return;
    }
    else
    {
        printf("Can't get display settings.\n");
    }

    return;
}

