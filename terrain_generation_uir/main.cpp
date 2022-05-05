#include "main.h"
#include "gl_viewport.h"

HINSTANCE g_instance;
INT panel_fixed_width;
INT log_fixed_height;
HWND h_wnd;
HWND h_panel;
HWND h_log;
HFONT h_deffont;

GLVIEWPORT viewport;

void edit_append_text(HWND hEdit, LPCSTR newText)
{
	int TextLen = SendMessageA(hEdit, WM_GETTEXTLENGTH, 0, 0);
	SendMessageA(hEdit, EM_SETSEL, (WPARAM)TextLen, (LPARAM)TextLen);
	SendMessageA(hEdit, EM_REPLACESEL, FALSE, (LPARAM)newText);
}

void edit_insertline(HWND hEdit, LPCSTR p_text, ...)
{
	CHAR buf[1024];
	va_list argptr;
	va_start(argptr, p_text);
	vsprintf_s(buf, sizeof(buf), p_text, argptr);
	va_end(argptr);
	strcat_s(buf, sizeof(buf), "\r\n");
	edit_append_text(hEdit, buf);
}

void resize_userinterface(HWND hWnd)
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	panel_fixed_width = PERCENTOF(GetSystemMetrics(SM_CXSCREEN), 15);
	log_fixed_height = PERCENTOF(GetSystemMetrics(SM_CYSCREEN), 10);

	RECT vrect;
	vrect.left = 1;
	vrect.top = 1;
	vrect.right = rect.right - panel_fixed_width;
	vrect.bottom = rect.bottom - log_fixed_height;
	MoveWindow(viewport.h_viewport, vrect.left, vrect.top, vrect.right, vrect.bottom, FALSE);

	RECT prect;
	prect.left = vrect.right + PADDING_PX;
	prect.top = 1;
	prect.right = rect.right - vrect.right - PADDING_PX - PADDING_PX;
	prect.bottom = rect.bottom - PADDING_PX;
	MoveWindow(h_panel, prect.left, prect.top, prect.right, prect.bottom, TRUE);

	RECT erect;
	erect.left = 1;
	erect.top = vrect.bottom + PADDING_PX;
	erect.right = prect.left - PADDING_PX;
	erect.bottom = rect.bottom - vrect.bottom - PADDING_PX - PADDING_PX;
	MoveWindow(h_log, erect.left, erect.top, erect.right, erect.bottom, TRUE);
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	InitCommonControls();
	h_deffont = (HFONT)GetStockObject(ANSI_VAR_FONT);

	/* main controls window class */
	WNDCLASSEXA wcex;
	memset(&wcex, NULL, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIconA(hInstance, MAKEINTRESOURCEA(IDI_WINDOWSPROJECT1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEA(IDC_WINDOWSPROJECT1);
	wcex.lpszClassName = CONTROLS_WINDOW_CLASS;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	if (!RegisterClassExA(&wcex)) {
		MessageBox(HWND_DESKTOP, _T("Couldn't create main window class"), _T("Critical error"), MB_OK | MB_ICONERROR);
		return 1;
	}

	g_instance = hInstance;
	INT sw = GetSystemMetrics(SM_CXSCREEN);
	INT sh = GetSystemMetrics(SM_CYSCREEN);
	INT width = PERCENTOF(sw, 80);
	INT height = PERCENTOF(sw, 50);
	INT xpos = (sw / 2) - (width / 2);
	INT ypos = (sh / 2) - (height / 2);
	h_wnd = CreateWindowExA(0, wcex.lpszClassName, "Window Application", WS_OVERLAPPEDWINDOW, xpos, ypos, width, height, NULL, NULL, hInstance, NULL);
	if (!h_wnd) {
		MessageBox(HWND_DESKTOP, _T("Couldn't create window"), _T("Critical error"), MB_OK | MB_ICONERROR);
		return 2;
	}

	panel_fixed_width = PERCENTOF(sw, 15);
	log_fixed_height = PERCENTOF(sh, 10);

	RECT rect;
	RECT vrect;
	GetClientRect(h_wnd, &rect);
	vrect.left = 1;
	vrect.top = 1;
	vrect.right = rect.right - panel_fixed_width;
	vrect.bottom = rect.bottom - log_fixed_height;

	// 
	// create panel
	// 
	RECT prect;
	prect.left = vrect.right + PADDING_PX;
	prect.top = 1;
	prect.right = rect.right - vrect.right - PADDING_PX - PADDING_PX;
	prect.bottom = rect.bottom - PADDING_PX;
	h_panel = CreateWindowExA(WS_EX_DLGMODALFRAME, wcex.lpszClassName, "", WS_VISIBLE | WS_CHILD,
		prect.left, prect.top, prect.right, prect.bottom, h_wnd, (HMENU)0, hInstance, NULL);

	// 
	// create log
	// 
	RECT erect;
	erect.left = 1;
	erect.top = vrect.bottom + PADDING_PX;
	erect.right = prect.left - PADDING_PX;
	erect.bottom = rect.bottom - vrect.bottom - PADDING_PX - PADDING_PX;
	h_log = CreateWindowExA(WS_EX_CLIENTEDGE, WC_EDIT, "- - Log started - -\r\n", WS_VISIBLE|WS_CHILD|ES_MULTILINE|ES_AUTOHSCROLL|ES_READONLY,
		erect.left, erect.top, erect.right, erect.bottom, h_wnd, (HMENU)0, hInstance, NULL);
	SendMessageA(h_log, WM_SETFONT, (WPARAM)h_deffont, (LPARAM)TRUE);

	edit_insertline(h_log, "Creating OpenGL viewport...");
	edit_insertline(h_log, "Viewport size ( %d x %d )", vrect.right, vrect.bottom);
	if (gl_viewport_init(&viewport, WS_VISIBLE | WS_CHILD, vrect.left, vrect.top, vrect.right, vrect.bottom, h_wnd, wcex.lpszClassName, 24, 32) != VRES_OK) {
		MessageBoxA(h_wnd, "Failed to create OpenGL viewport!", "Viewport error", MB_OK | MB_ICONERROR);
		DestroyWindow(h_wnd);
		UnregisterClassA(wcex.lpszClassName, hInstance);
		return 3;
	}
	edit_insertline(h_log, "OpenGL version: %s", glGetString(GL_VERSION));
	edit_insertline(h_log, "Vendor: %s", glGetString(GL_VENDOR));
	edit_insertline(h_log, "Renderer: %s", glGetString(GL_RENDERER));

	ShowWindow(h_wnd, nCmdShow);
	UpdateWindow(h_wnd);

	// 
	// установка начальных параметров сцены и окна
	// 
	GetClientRect(viewport.h_viewport, &rect);
	glMatrixMode(GL_PROJECTION); //выбор матрицы проекции
	glLoadIdentity(); //сделать матрицу проекции единичной
	glViewport(0, 0, rect.right, rect.bottom);
	gluPerspective(45.0, rect.right / (double)rect.bottom, -1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW); //выбор матрицы модели вида
	glLoadIdentity(); //сделать матрицу модели вида единичной

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		SwapBuffers(viewport.h_device_context);
    }
	gl_viewport_shutdown(&viewport);
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(g_instance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

	case WM_ERASEBKGND:
		if (hWnd == viewport.h_viewport)
			return 1;

		return 0;

	case WM_SIZE: {
		if (hWnd == h_wnd) {
			resize_userinterface(hWnd);
		}
		else if (hWnd == viewport.h_viewport) {
			GetClientRect(viewport.h_viewport, &rect);
			glMatrixMode(GL_PROJECTION); //выбор матрицы проекции
			glLoadIdentity(); //сделать матрицу проекции единичной
			glViewport(0, 0, rect.right, rect.bottom);
			gluPerspective(45.0, rect.right / (double)rect.bottom, -1.0, 1000.0);
			glMatrixMode(GL_MODELVIEW); //выбор матрицы модели вида
			glLoadIdentity(); //сделать матрицу модели вида единичной
		}
		break;
	}

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
