// Skeleton.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

//----------------------------------
// Skeleton functions and variables.
//-----------------
char szAppName[];
float fTime=0.f, fDeltaTime=0.f;
//-------
// Stuff.
bool Init();
void Shutdown();
#ifndef NOEXTENSION
bool Splash();
void UserKey(int k);
#endif
// Draw all the scene related stuff.
void Render();
//----------------------------------
// Timer info.
LARGE_INTEGER TimerFreq;	// Timer Frequency.
LARGE_INTEGER TimeStart;	// Time of start.
LARGE_INTEGER TimeCur;		// Current time.

// Good 'ol generic drawing stuff.
LRESULT CALLBACK SkeletonProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

// Usefull debugger thingy.
	const int kMaxDebugMessageLength = 1024;
	void DebugOut(const char *szFormat, ...)
	{
		va_list argList;
		char szBuffer[kMaxDebugMessageLength];

		va_start(argList, szFormat);
		_vsnprintf(szBuffer, kMaxDebugMessageLength, szFormat, argList);
		va_end(argList);

		OutputDebugString(szBuffer);
	}

// Select the pixel format for a given device context
void SetDCPixelFormat(HDC hDC)
{
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// Size of this structure
		1,								// Version of this structure
		PFD_DRAW_TO_WINDOW |			// Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |			// Support OpenGL calls in window
		PFD_DOUBLEBUFFER|				// Double buffered mode
#ifdef NOZBUFFER
		PFD_DEPTH_DONTCARE,
#endif
		PFD_TYPE_RGBA,					// RGBA Color mode
		GetDeviceCaps(hDC, BITSPIXEL),	// Want the display bit depth
		0,0,0,0,0,0,					// Not used to select mode
		0,0,							// Not used to select mode
		0,0,0,0,0,						// Not used to select mode
#ifdef NOZBUFFER
		0,								// Size of depth buffer
#else
		16,								// Size of depth buffer
#endif
		0,								// Not used to select mode
		0,								// Not used to select mode
		PFD_MAIN_PLANE,					// Draw in main plane
		0,								// Not used to select mode
		0,0,0
	};									// Not used to select mode

	// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);
	DebugOut( "PixelFormat Selected: %d\nBPP: %d\n", nPixelFormat, GetDeviceCaps(hDC, BITSPIXEL) );

	// Set the pixel format for the device context
	SetPixelFormat(hDC, nPixelFormat, &pfd);
}

void ChangeSize( GLsizei w, GLsizei h )
{
	// Prevent a divide by Zero
	if( h == 0 )
		h = 1;

	// Set viewport to our dimensions.
	glViewport( 0, 0, w, h);
}

GLsizei g_w, g_h;
#ifndef NOEXTENSION
HWND g_hwnd;
#endif
bool bReady;

// Good ol' creation code.
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
#ifndef NOEXTENSION
	if (!Splash()) return 0;
#endif
	// Say we're not ready to render yet.
	bReady = false;

	// Setup the window class.
	WNDCLASS wc;
	HWND hWnd;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc	= (WNDPROC)SkeletonProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );

	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szAppName;

	// Announce it to the Microsloth OS.
	if( RegisterClass( &wc ) == 0)
	{
		MessageBox( NULL, "Failed to register the idiot window class, right before creating the window.", "Fatal Blow", MB_OK | MB_ICONERROR );
		return FALSE;	// Failed to create window, so just quit right away.
	}

	// Make it go.......
	hWnd = CreateWindow(
			szAppName,
			szAppName,
			WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
			CW_USEDEFAULT, CW_USEDEFAULT,
			640, 480,
			NULL, NULL,
			hInstance,
			NULL );

	if( hWnd == NULL )
	{
		MessageBox( NULL, "Failed to create the effect window.", "Fatal Blow", MB_OK | MB_ICONERROR );
		return FALSE;	// Failed to create window, so just quit right away.
	}
#ifndef NOEXTENSION
	g_hwnd=hWnd;
#endif
	// Make it visible...
	ShowWindow( hWnd, SW_SHOW );
	UpdateWindow( hWnd );

	// Reset the timer stuff.
	QueryPerformanceFrequency(&TimerFreq);
	QueryPerformanceCounter(&TimeStart);


	// Prepare the scene for rendering.
	if( Init() == false )
	{
		MessageBox( hWnd, "Failed to initialize the visual effect.", "Fatal Blow", MB_OK | MB_ICONERROR );
		return FALSE;	// Failed init, so just quit right away.
	}

	// We're now ready.
	bReady = true;

	// Usual running around in circles bit...
	BOOL bGotMsg;
	MSG  msg;
	PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );
    while( WM_QUIT != msg.message  )
    {
		// Use PeekMessage() if the app is active, so we can use idle time to
		// render the scene. Else, use GetMessage() to avoid eating CPU time.
		bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );

		if( bGotMsg )
        {
			// Translate and dispatch the message
			TranslateMessage( &msg );
			DispatchMessage( &msg );
        }
		else
		{
			InvalidateRect( hWnd, NULL, FALSE );
		}
    }

	// Not ready to render anymore.
	bReady = false;

	// Nuke all applicable scene stuff.
	Shutdown();

	return msg.wParam;
}

// Stupid usual callback window proc message spud gun.
LRESULT CALLBACK SkeletonProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Keep drawing stuff around.
	static HGLRC hRC;
	static HDC hDC;

    switch( uMsg )
    {
    case WM_CREATE:
		// Remember our mSloth drawing context.
		hDC = GetDC( hWnd );

		// Select our precious pixel format.
		SetDCPixelFormat( hDC );

		// Yeppers, make something that OpenGL understands.
		hRC = wglCreateContext( hDC );
		wglMakeCurrent( hDC, hRC );
		break;

	case WM_DESTROY:
		wglMakeCurrent( hDC, NULL );
		wglDeleteContext( hRC );
		PostQuitMessage( 0 );
		break;

	case WM_SIZE:
		g_w = LOWORD(lParam);
		g_h = HIWORD(lParam);
		ChangeSize( g_w, g_h );
		break;

	case WM_PAINT:
		if( bReady )
		{
			// Get the current time, and update the time controller.
			QueryPerformanceCounter(&TimeCur);
			float fOldTime = fTime;
			fTime = (float)((double)(TimeCur.QuadPart-TimeStart.QuadPart)/(double)TimerFreq.QuadPart);
			fDeltaTime = fTime - fOldTime;

			// Draw all the scene related stuff.
			Render();

			// Make it all visible at once!
			SwapBuffers( hDC );

			// We actually did something with this rect, so announce it to the mSloth OS.
			ValidateRect( hWnd, NULL );
		}
		break;
#ifndef NOEXTENSION
	case WM_CHAR:
		UserKey(wParam);
		break;
#endif
	default:
		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}

	return 0;
}
