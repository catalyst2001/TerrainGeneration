#pragma once
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")

#include "resource.h"

#define PADDING_PX 2
#define CONTROLS_WINDOW_CLASS "ControlsWindow"
#define VIEWPORT_WINDOW_CLASS "ViewportWindow"

#define PERCENTOF(x, p) ((INT)(x / 100 * p))