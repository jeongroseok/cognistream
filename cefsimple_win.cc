// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <windows.h>

#include "include/cef_command_line.h"
#include "include/cef_sandbox_win.h"
#include "tests/cefsimple/simple_app.h"

// CMake으로 프로젝트를 생성할 때 CEF_USE_SANDBOX 값이 자동으로 정의됩니다.
// 필요한 컴파일러 버전을 사용하는 경우 자동으로 정의됩니다. 샌드박스 사용을 비활성화하려면
// CMake 명령줄에 -DUSE_SANDBOX=OFF를 전달하세요.
// 샌드박스 지원을 수동으로 활성화하려면 이 줄의 주석을 제거하세요.
// #define CEF_USE_SANDBOX 1

#if defined(CEF_USE_SANDBOX)
// cef_sandbox.lib 정적 라이브러리는 모든 VS 버전과 성공적으로 링크되지 않을 수 있습니다.
#pragma comment(lib, "cef_sandbox.lib")
#endif

// 모든 프로세스의 진입점 함수.
int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPTSTR lpCmdLine,
                      int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  int exit_code;

#if defined(ARCH_CPU_32_BITS)
  // 32비트 Windows에서 선호되는 4MiB 스택 크기를 가진 fiber로 메인 스레드를 실행합니다.
  // 이 함수는 실행 파일의 진입점 함수(`main()` 또는 `wWinMain()`)의 최상단에서 호출되어야 합니다.
  // 링커 플래그 `/STACK:0x80000`을 통해 구성된 초기 스택 크기 0.5MiB와 함께 사용됩니다.
  // 이는 Windows 스레드 풀과 같이 스택 크기를 링커 플래그로만 제어할 수 있는 스레드의 메모리를 절약합니다.
  exit_code = CefRunWinMainWithPreferredStackSize(wWinMain, hInstance,
                                                  lpCmdLine, nCmdShow);
  if (exit_code >= 0) {
    // fiber가 완료되었으므로 여기서 반환합니다.
    return exit_code;
  }
#endif

  void* sandbox_info = nullptr;

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
  if (exit_code >= 0) {
    // 하위 프로세스가 완료되었으므로 여기서 반환합니다.
    return exit_code;
  }

  // 이 메서드에서 사용하기 위해 명령줄 인수를 파싱합니다.
  CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
  command_line->InitFromString(::GetCommandLineW());

  // 여기에서 CEF 전역 설정을 지정합니다.
  CefSettings settings;

#if !defined(CEF_USE_SANDBOX)
  settings.no_sandbox = true;
#endif

  // SimpleApp은 브라우저 프로세스에 대한 애플리케이션 수준 콜백을 구현합니다.
  // CEF가 초기화된 후 OnContextInitialized()에서 첫 번째 브라우저 인스턴스를 생성합니다.
  CefRefPtr<SimpleApp> app(new SimpleApp);

  // CEF 브라우저 프로세스를 초기화합니다. 초기화 실패 시 또는 프로세스 단일 실행 재시작 동작 등으로 조기 종료가 필요한 경우 false를 반환할 수 있습니다.
  if (!CefInitialize(main_args, settings, app.get(), sandbox_info)) {
    return CefGetExitCode();
  }

  // CEF 메시지 루프를 실행합니다. CefQuitMessageLoop()가 호출될 때까지 차단됩니다.
  CefRunMessageLoop();

  // CEF를 종료합니다.
  CefShutdown();

  return 0;
}
