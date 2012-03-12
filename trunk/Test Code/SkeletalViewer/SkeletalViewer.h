/************************************************************************
*                                                                       *
*   SkeletalViewer.h -- Declares of CSkeletalViewerApp class            *
*                                                                       *
* Copyright (c) Microsoft Corp. All rights reserved.                    *
*                                                                       *
* This code is licensed under the terms of the                          *
* Microsoft Kinect for Windows SDK (Beta)                               *
* License Agreement: http://kinectforwindows.org/KinectSDK-ToU          *
*                                                                       *
************************************************************************/

#pragma once

#include "resource.h"
#include "MSR_NuiApi.h"
#include "DrawDevice.h"
#include "SocketConnectivity.h"

#define SZ_APPDLG_WINDOW_CLASS              _T("SkeletalViewerAppDlgWndClass")
#define WM_USER_UPDATE_FPS                  WM_USER
#define WM_USER_UPDATE_COMBO                WM_USER+1
// <adeel>
#define POSITION_UPDATE_WAIT_TIME_INTERVAL  2000   // specified in milliseconds
// </adeel>

class CSkeletalViewerApp
{
public:
    CSkeletalViewerApp();
    ~CSkeletalViewerApp();
    HRESULT                 Nui_Init();
    HRESULT                 Nui_Init(int index);
    HRESULT                 Nui_Init(OLECHAR *instanceName);
    void                    Nui_UnInit( );
    void                    Nui_GotDepthAlert( );
    void                    Nui_GotVideoAlert( );
    void                    Nui_GotSkeletonAlert( );
    void                    Nui_Zero();
    void                    Nui_BlankSkeletonScreen( HWND hWnd );
    void                    Nui_DoDoubleBuffer(HWND hWnd,HDC hDC);
    void                    Nui_DrawSkeleton( bool bBlank, NUI_SKELETON_DATA * pSkel, HWND hWnd, int WhichSkeletonColor );
    void                    Nui_DrawSkeletonSegment( NUI_SKELETON_DATA * pSkel, int numJoints, ... );

    RGBQUAD                 Nui_ShortToQuad_Depth( USHORT s );

    static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK        WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static void CALLBACK    Nui_StatusProcThunk(const NuiStatusData *pStatusData);
    void CALLBACK           Nui_StatusProc(const NuiStatusData *pStatusData);

    HWND m_hWnd;

private:
    bool m_fUpdatingUi;
    void UpdateComboBox();

	// <adeel>
	SocketConnectivity         m_socketConnectivity;
    int                        m_skeletonBeingTracked;
    long long int              m_firstSkeletonFoundTime;
	long long int              m_lastPositionUpdateTime;
	float                      m_startPositionX;
	float                      m_startPositionY;
	float                      m_startPositionZ;
	bool                       m_sendPositionUpdates;
    // </adeel>
	CRITICAL_SECTION        m_critSecUi; // Gate UI operations on the background thread.
    static DWORD WINAPI     Nui_ProcessThread(LPVOID pParam);
    DWORD WINAPI            Nui_ProcessThread();
    INuiInstance*           m_pNuiInstance;
    BSTR                    m_instanceId;

    // NUI and draw stuff
    DrawDevice              m_DrawDepth;
    DrawDevice              m_DrawVideo;

    // thread handling
    HANDLE        m_hThNuiProcess;
    HANDLE        m_hEvNuiProcessStop;

    HANDLE        m_hNextDepthFrameEvent;
    HANDLE        m_hNextVideoFrameEvent;
    HANDLE        m_hNextSkeletonEvent;
    HANDLE        m_pDepthStreamHandle;
    HANDLE        m_pVideoStreamHandle;
    HFONT         m_hFontFPS;
    HPEN          m_Pen[6];
    HDC           m_SkeletonDC;
    HBITMAP       m_SkeletonBMP;
    HGDIOBJ       m_SkeletonOldObj;
    int           m_PensTotal;
    POINT         m_Points[NUI_SKELETON_POSITION_COUNT];
    RGBQUAD       m_rgbWk[640*480];
    int           m_LastSkeletonFoundTime;
    bool          m_bScreenBlanked;
    int           m_FramesTotal;
    int           m_LastFPStime;
    int           m_LastFramesTotal;
};

int MessageBoxResource(HWND hwnd,UINT nID,UINT nType);

// <adeel>
extern TCHAR g_szAppTitle[256];
// </adeel>


