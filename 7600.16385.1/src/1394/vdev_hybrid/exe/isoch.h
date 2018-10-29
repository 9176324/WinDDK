/*++

Copyright (c) 1998  Microsoft Corporation

Module Name:

    isoch.h

Abstract

    1394 isoch wrappers

--*/

INT_PTR CALLBACK
IsochAllocateBandwidthDlgProc(
    HWND        hDlg,
    UINT        uMsg,
    WPARAM      wParam,
    LPARAM      lParam
    );

void
w1394_IsochAllocateBandwidth(
    HWND         hWnd,
    __in PSTR    szDeviceName
    );

INT_PTR CALLBACK
IsochAllocateChannelDlgProc(
    HWND        hDlg,
    UINT        uMsg,
    WPARAM      wParam,
    LPARAM      lParam
    );

void
w1394_IsochAllocateChannel(
    HWND         hWnd,
    __in PSTR    szDeviceName
    );

INT_PTR CALLBACK
IsochAllocateResourcesDlgProc(
    HWND        hDlg,
    UINT        uMsg,
    WPARAM      wParam,
    LPARAM      lParam
    );

void
w1394_IsochAllocateResources(
    HWND         hWnd,
    __in PSTR    szDeviceName
    );

INT_PTR CALLBACK
IsochAttachBuffersDlgProc(
    HWND        hDlg,
    UINT        uMsg,
    WPARAM      wParam,
    LPARAM      lParam
    );

void
w1394_IsochAttachBuffers(
    HWND         hWnd,
    __in PSTR    szDeviceName
    );

INT_PTR CALLBACK
IsochDetachBuffersDlgProc(
    HWND        hDlg,
    UINT        uMsg,
    WPARAM      wParam,
    LPARAM      lParam
    );

void
w1394_IsochDetachBuffers(
    HWND         hWnd,
    __in PSTR    szDeviceName
    );

INT_PTR CALLBACK
IsochFreeBandwidthDlgProc(
    HWND        hDlg,
    UINT        uMsg,
    WPARAM      wParam,
    LPARAM      lParam
    );

void
w1394_IsochFreeBandwidth(
    HWND         hWnd,
    __in PSTR    szDeviceName
    );

INT_PTR CALLBACK
IsochFreeChannelDlgProc(
    HWND        hDlg,
    UINT        uMsg,
    WPARAM      wParam,
    LPARAM      lParam
    );

void
w1394_IsochFreeChannel(
    HWND         hWnd,
    __in PSTR    szDeviceName
    );

INT_PTR CALLBACK
IsochFreeResourcesDlgProc(
    HWND        hDlg,
    UINT        uMsg,
    WPARAM      wParam,
    LPARAM      lParam
    );

void
w1394_IsochFreeResources(
    HWND         hWnd,
    __in PSTR    szDeviceName
    );

INT_PTR CALLBACK
IsochListenDlgProc(
    HWND        hDlg,
    UINT        uMsg,
    WPARAM      wParam,
    LPARAM      lParam
    );

void
w1394_IsochListen(
    HWND         hWnd,
    __in PSTR    szDeviceName
    );

void
w1394_IsochQueryCurrentCycleTime(
    HWND         hWnd,
    __in PSTR    szDeviceName
    );

INT_PTR CALLBACK
IsochQueryResourcesDlgProc(
    HWND        hDlg,
    UINT        uMsg,
    WPARAM      wParam,
    LPARAM      lParam
    );

void
w1394_IsochQueryResources(
    HWND         hWnd,
    __in PSTR    szDeviceName
    );

void
w1394_IsochSetChannelBandwidth(
    HWND         hWnd,
    __in PSTR    szDeviceName
    );

INT_PTR CALLBACK
IsochStopDlgProc(
    HWND        hDlg,
    UINT        uMsg,
    WPARAM      wParam,
    LPARAM      lParam
    );

void
w1394_IsochStop(
    HWND         hWnd,
    __in PSTR    szDeviceName
    );

INT_PTR CALLBACK
IsochTalkDlgProc(
    HWND        hDlg,
    UINT        uMsg,
    WPARAM      wParam,
    LPARAM      lParam
    );

void
w1394_IsochTalk(
                HWND         hWnd,
                __in PSTR    szDeviceName);
/*
INT_PTR CALLBACK
IsochLoopbackDlgProc(
    HWND        hDlg,
    UINT        uMsg,
    WPARAM      wParam,
    LPARAM      lParam
    );

void
w1394_IsochStartLoopback(
    HWND                        hWnd,
    __in PSTR                   szDeviceName,
    PISOCH_LOOPBACK_PARAMS      IsochLoopbackParams
    );

void
w1394_IsochStopLoopback(
    HWND                        hWnd,
    PISOCH_LOOPBACK_PARAMS      IsochLoopbackParams
    );


*/
