// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "simple_handler.h"

#include <windows.h>
#include <string>

#include "include/cef_browser.h"

// 창 제목 변경을 위한 플랫폼별 구현 (Windows)
void SimpleHandler::PlatformTitleChange(CefRefPtr<CefBrowser> browser,
                                        const CefString &title)
{
  CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
  if (hwnd)
  {
    // 창 제목 설정
    SetWindowText(hwnd, std::wstring(title).c_str());
  }
}
