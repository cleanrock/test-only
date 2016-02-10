//
// "$Id$"
//
// Definition of MSWindows Win32/64 Screen interface
//
// Copyright 1998-2016 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     http://www.fltk.org/COPYING.php
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//


#include "../../config_lib.h"
#include "Fl_WinAPI_Screen_Driver.h"
#include <FL/fl_ask.h>

#  if !defined(HMONITOR_DECLARED) && (_WIN32_WINNT < 0x0500)
#    define COMPILE_MULTIMON_STUBS
#    include <multimon.h>
#  endif // !HMONITOR_DECLARED && _WIN32_WINNT < 0x0500


/**
 Creates a driver that manages all screen and display related calls.
 
 This function must be implemented once for every platform. It is called
 when the static members of the class "Fl" are created.
 */
Fl_Screen_Driver *Fl_Screen_Driver::newScreenDriver()
{
  return new Fl_WinAPI_Screen_Driver();
}


// We go the much more difficult route of individually picking some multi-screen
// functions from the USER32.DLL . If these functions are not available, we
// will gracefully fall back to single monitor support.
//
// If we were to insist on the existence of "EnumDisplayMonitors" and
// "GetMonitorInfoA", it would be impossible to use FLTK on Windows 2000
// before SP2 or earlier.

// BOOL EnumDisplayMonitors(HDC, LPCRECT, MONITORENUMPROC, LPARAM)
typedef BOOL(WINAPI* fl_edm_func)(HDC, LPCRECT, MONITORENUMPROC, LPARAM);
// BOOL GetMonitorInfo(HMONITOR, LPMONITORINFO)
typedef BOOL(WINAPI* fl_gmi_func)(HMONITOR, LPMONITORINFO);

static fl_gmi_func fl_gmi = NULL; // used to get a proc pointer for GetMonitorInfoA


BOOL Fl_WinAPI_Screen_Driver::screen_cb(HMONITOR mon, HDC hdc, LPRECT r, LPARAM d) 
{
  Fl_WinAPI_Screen_Driver *drv = (Fl_WinAPI_Screen_Driver*)d;
  return drv->screen_cb(mon, hdc, r);
}


BOOL Fl_WinAPI_Screen_Driver::screen_cb(HMONITOR mon, HDC, LPRECT r) 
{
  if (num_screens >= MAX_SCREENS) return TRUE;

  MONITORINFOEX mi;
  mi.cbSize = sizeof(mi);

  //  GetMonitorInfo(mon, &mi);
  //  (but we use our self-acquired function pointer instead)
  if (fl_gmi(mon, &mi)) {
    screens[num_screens] = mi.rcMonitor;
    // If we also want to record the work area, we would also store mi.rcWork at this point
    work_area[num_screens] = mi.rcWork;
    /*fl_alert("screen %d %d,%d,%d,%d work %d,%d,%d,%d",num_screens,
    screens[num_screens].left,screens[num_screens].right,screens[num_screens].top,screens[num_screens].bottom,
    work_area[num_screens].left,work_area[num_screens].right,work_area[num_screens].top,work_area[num_screens].bottom);
    */
    // find the pixel size
    if (mi.cbSize == sizeof(mi)) {
      HDC screen = CreateDC(mi.szDevice, NULL, NULL, NULL);
      if (screen) {
        dpi[num_screens][0] = (float)GetDeviceCaps(screen, LOGPIXELSX);
        dpi[num_screens][1] = (float)GetDeviceCaps(screen, LOGPIXELSY);
      }
      ReleaseDC(0L, screen);
    }

    num_screens++;
  }
  return TRUE;
}


void Fl_WinAPI_Screen_Driver::init()
{
  // Since not all versions of Windows include multiple monitor support,
  // we do a run-time check for the required functions...
  HMODULE hMod = GetModuleHandle("USER32.DLL");

  if (hMod) {
    // check that EnumDisplayMonitors is available
    fl_edm_func fl_edm = (fl_edm_func)GetProcAddress(hMod, "EnumDisplayMonitors");

    if (fl_edm) {
      // we have EnumDisplayMonitors - do we also have GetMonitorInfoA ?
      fl_gmi = (fl_gmi_func)GetProcAddress(hMod, "GetMonitorInfoA");
      if (fl_gmi) {
        // We have GetMonitorInfoA, enumerate all the screens...
        //      EnumDisplayMonitors(0,0,screen_cb,0);
        //      (but we use our self-acquired function pointer instead)
        //      NOTE: num_screens is incremented in screen_cb so we must first reset it here...
        num_screens = 0;
        fl_edm(0, 0, screen_cb, (LPARAM)this);
        return;
      }
    }
  }

  // If we get here, assume we have 1 monitor...
  num_screens = 1;
  screens[0].top = 0;
  screens[0].left = 0;
  screens[0].right = GetSystemMetrics(SM_CXSCREEN);
  screens[0].bottom = GetSystemMetrics(SM_CYSCREEN);
  work_area[0] = screens[0];
}


void Fl_WinAPI_Screen_Driver::screen_work_area(int &X, int &Y, int &W, int &H, int n)
{
  if (num_screens < 0) init();
  if (n < 0 || n >= num_screens) n = 0;
  X = work_area[n].left;
  Y = work_area[n].top;
  W = work_area[n].right - X;
  H = work_area[n].bottom - Y;
}


void Fl_WinAPI_Screen_Driver::screen_xywh(int &X, int &Y, int &W, int &H, int n)
{
  if (num_screens < 0) init();

  if ((n < 0) || (n >= num_screens))
    n = 0;

  if (num_screens > 0) {
    X = screens[n].left;
    Y = screens[n].top;
    W = screens[n].right - screens[n].left;
    H = screens[n].bottom - screens[n].top;
  } else {
    /* Fallback if something is broken... */
    X = 0;
    Y = 0;
    W = GetSystemMetrics(SM_CXSCREEN);
    H = GetSystemMetrics(SM_CYSCREEN);
  }
}


void Fl_WinAPI_Screen_Driver::screen_dpi(float &h, float &v, int n)
{
  if (num_screens < 0) init();
  h = v = 0.0f;
  if (n >= 0 && n < num_screens) {
    h = float(dpi[n][0]);
    v = float(dpi[n][1]);
  }
}


int Fl_WinAPI_Screen_Driver::x()
{
  RECT r;

  SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);
  return r.left;
}


int Fl_WinAPI_Screen_Driver::y()
{
  RECT r;

  SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);
  return r.top;
}


int Fl_WinAPI_Screen_Driver::h()
{
  RECT r;

  SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);
  return r.bottom - r.top;
}


int Fl_WinAPI_Screen_Driver::w()
{
  RECT r;

  SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);
  return r.right - r.left;
}


void Fl_WinAPI_Screen_Driver::beep(int type)
{
  switch (type) {
    case FL_BEEP_QUESTION :
    case FL_BEEP_PASSWORD :
      MessageBeep(MB_ICONQUESTION);
      break;
    case FL_BEEP_MESSAGE :
      MessageBeep(MB_ICONASTERISK);
      break;
    case FL_BEEP_NOTIFICATION :
      MessageBeep(MB_ICONASTERISK);
      break;
    case FL_BEEP_ERROR :
      MessageBeep(MB_ICONERROR);
      break;
    default :
      MessageBeep(0xFFFFFFFF);
      break;
  }
}



//
// End of "$Id$".
//