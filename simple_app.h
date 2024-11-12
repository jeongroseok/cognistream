// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_

#include "include/cef_app.h"

// 브라우저 프로세스를 위한 애플리케이션 수준 콜백을 구현
class SimpleApp : public CefApp, public CefBrowserProcessHandler
{
public:
  SimpleApp();

  // CefApp 메소드
  CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override
  {
    return this;
  }

  // CefBrowserProcessHandler 메소드
  void OnContextInitialized() override;
  CefRefPtr<CefClient> GetDefaultClient() override;

private:
  // 기본 참조 카운팅 구현 포함
  IMPLEMENT_REFCOUNTING(SimpleApp);
};

#endif // CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
