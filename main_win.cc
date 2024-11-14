// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "simple_app.h"

#include <windows.h>

#include "include/cef_command_line.h"
#include "include/cef_sandbox_win.h"

// 샌드박스 지원을 수동으로 활성화하려면 이 줄의 주석을 제거하세요.
// #define CEF_USE_SANDBOX 1

#if defined(CEF_USE_SANDBOX)
#pragma comment(lib, "cef_sandbox.lib")
#endif

// 32비트는 지원하지 않습니다.
#if defined(ARCH_CPU_32_BITS)
#error "Only 64-bit is supported"
#endif

// 모든 프로세스의 진입점 함수.
int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPTSTR lpCmdLine,
                      int nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  int exit_code;

  void *sandbox_info = nullptr;

#if defined(CEF_USE_SANDBOX)
  // 샌드박스 정보 객체의 수명을 관리합니다. 이는 Windows에서 샌드박스 지원을 위해 필요합니다.
  // 자세한 내용은 cef_sandbox_win.h를 참조하세요.
  CefScopedSandboxInfo scoped_sandbox;
  sandbox_info = scoped_sandbox.sandbox_info();
#endif

  // CEF에 명령줄 인수를 제공합니다.
  CefMainArgs main_args(hInstance);

  // CEF 애플리케이션은 동일한 실행 파일을 공유하는 여러 하위 프로세스(render, GPU 등)를 가집니다.
  // 이 함수는 명령줄을 확인하고, 하위 프로세스인 경우 적절한 로직을 실행합니다.
  exit_code = CefExecuteProcess(main_args, nullptr, sandbox_info);
  if (exit_code >= 0)
  {
    return exit_code;
  }

  // 명령줄 인수를 파싱합니다.
  CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
  command_line->InitFromString(::GetCommandLineW());

  // CEF 전역 설정을 지정합니다.
  CefSettings settings;

#if !defined(CEF_USE_SANDBOX)
  settings.no_sandbox = true;
#endif

  // SimpleApp은 브라우저 프로세스에 대한 애플리케이션 수준 콜백을 구현합니다.
  // CEF가 초기화된 후 OnContextInitialized()에서 첫 번째 브라우저 인스턴스를 생성합니다.
  CefRefPtr<SimpleApp> app(new SimpleApp);

  // CEF 브라우저 프로세스를 초기화합니다. 초기화 실패 시 또는 프로세스 단일 실행 재시작 동작 등으로 조기 종료가 필요한 경우 false를 반환할 수 있습니다.
  if (!CefInitialize(main_args, settings, app.get(), sandbox_info))
  {
    return CefGetExitCode();
  }

  // CEF 메시지 루프는 CefQuitMessageLoop()가 호출될 때까지 블로킹됩니다.
  CefRunMessageLoop();

  // CEF를 종료합니다.
  CefShutdown();

  return 0;
}
