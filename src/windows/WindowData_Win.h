#pragma once
#include <MiniFB_enums.h>
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef struct {
  HWND              window;
  WNDCLASS          wc;
  HDC               hdc;
  HGLRC             hGLRC;
  uint32_t          text_id;
  struct mfb_timer* timer;
  bool              mouse_inside;
  } SWindowData_Win;
