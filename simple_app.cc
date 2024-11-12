// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "simple_app.h"
#include "simple_handler.h"

#include <string>

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"

namespace {

// Views 프레임워크를 사용하는 경우, CefWindow의 델리게이트 구현을 제공
class SimpleWindowDelegate : public CefWindowDelegate {
 public:
  SimpleWindowDelegate(CefRefPtr<CefBrowserView> browser_view,
                       cef_runtime_style_t runtime_style,
                       cef_show_state_t initial_show_state)
      : browser_view_(browser_view),
        runtime_style_(runtime_style),
        initial_show_state_(initial_show_state) {}

  void OnWindowCreated(CefRefPtr<CefWindow> window) override {
    // 브라우저 뷰 추가 및 창 표시
    window->AddChildView(browser_view_);

    if (initial_show_state_ != CEF_SHOW_STATE_HIDDEN) {
      window->Show();
    }

    if (initial_show_state_ != CEF_SHOW_STATE_MINIMIZED &&
        initial_show_state_ != CEF_SHOW_STATE_HIDDEN) {
      // 브라우저 뷰에 키보드 포커스 요청
      browser_view_->RequestFocus();
    }
  }

  void OnWindowDestroyed(CefRefPtr<CefWindow> window) override {
    browser_view_ = nullptr;
  }

  bool CanClose(CefRefPtr<CefWindow> window) override {
    // 브라우저가 닫을 수 있는지 확인
    CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
    if (browser) {
      return browser->GetHost()->TryCloseBrowser();
    }
    return true;
  }

  CefSize GetPreferredSize(CefRefPtr<CefView> view) override {
    return CefSize(800, 600);
  }

  cef_show_state_t GetInitialShowState(CefRefPtr<CefWindow> window) override {
    return initial_show_state_;
  }

  cef_runtime_style_t GetWindowRuntimeStyle() override {
    return runtime_style_;
  }

 private:
  CefRefPtr<CefBrowserView> browser_view_;
  const cef_runtime_style_t runtime_style_;
  const cef_show_state_t initial_show_state_;

  IMPLEMENT_REFCOUNTING(SimpleWindowDelegate);
  DISALLOW_COPY_AND_ASSIGN(SimpleWindowDelegate);
};

// BrowserView 델리게이트 구현
class SimpleBrowserViewDelegate : public CefBrowserViewDelegate {
 public:
  explicit SimpleBrowserViewDelegate(cef_runtime_style_t runtime_style)
      : runtime_style_(runtime_style) {}

  bool OnPopupBrowserViewCreated(CefRefPtr<CefBrowserView> browser_view,
                                 CefRefPtr<CefBrowserView> popup_browser_view,
                                 bool is_devtools) override {
    // 팝업을 위한 새 창 생성
    CefWindow::CreateTopLevelWindow(new SimpleWindowDelegate(
        popup_browser_view, runtime_style_, CEF_SHOW_STATE_NORMAL));

    // 창을 생성했음을 표시
    return true;
  }

  cef_runtime_style_t GetBrowserRuntimeStyle() override {
    return runtime_style_;
  }

 private:
  const cef_runtime_style_t runtime_style_;

  IMPLEMENT_REFCOUNTING(SimpleBrowserViewDelegate);
  DISALLOW_COPY_AND_ASSIGN(SimpleBrowserViewDelegate);
};

}  // namespace

SimpleApp::SimpleApp() = default;

// 컨텍스트 초기화 시 호출
void SimpleApp::OnContextInitialized() {
  CEF_REQUIRE_UI_THREAD();

  CefRefPtr<CefCommandLine> command_line =
      CefCommandLine::GetGlobalCommandLine();

  // Alloy 스타일 사용 여부 확인
  cef_runtime_style_t runtime_style = CEF_RUNTIME_STYLE_DEFAULT;
  bool use_alloy_style = command_line->HasSwitch("use-alloy-style");
  if (use_alloy_style) {
    runtime_style = CEF_RUNTIME_STYLE_ALLOY;
  }

  // SimpleHandler는 브라우저 수준 콜백을 구현
  CefRefPtr<SimpleHandler> handler(new SimpleHandler(use_alloy_style));

  // CEF 브라우저 설정 지정
  CefBrowserSettings browser_settings;

  std::string url;

  // 명령줄에서 "--url=" 값이 제공되었는지 확인
  url = command_line->GetSwitchValue("url");
  if (url.empty()) {
    url = "https://www.google.com";
  }

  // Views는 기본적으로 활성화됨 (`--use-native` 추가 시 비활성화)
  const bool use_views = !command_line->HasSwitch("use-native");

  // Views를 사용하는 경우와 네이티브 플랫폼 프레임워크를 사용하는 경우로 분기
  if (use_views) {
    // BrowserView 생성
    CefRefPtr<CefBrowserView> browser_view = CefBrowserView::CreateBrowserView(
        handler, url, browser_settings, nullptr, nullptr,
        new SimpleBrowserViewDelegate(runtime_style));

    // 초기 표시 상태 설정 (선택 사항)
    cef_show_state_t initial_show_state = CEF_SHOW_STATE_NORMAL;
    const std::string& show_state_value =
        command_line->GetSwitchValue("initial-show-state");
    if (show_state_value == "minimized") {
      initial_show_state = CEF_SHOW_STATE_MINIMIZED;
    } else if (show_state_value == "maximized") {
      initial_show_state = CEF_SHOW_STATE_MAXIMIZED;
    }
#if defined(OS_MAC)
    // MacOS에서만 지원되는 숨김 표시 상태
    else if (show_state_value == "hidden") {
      initial_show_state = CEF_SHOW_STATE_HIDDEN;
    }
#endif

    // 창 생성 (생성 후 자동 표시)
    CefWindow::CreateTopLevelWindow(new SimpleWindowDelegate(
        browser_view, runtime_style, initial_show_state));
  } else {
    // 네이티브 창 생성을 위한 정보
    CefWindowInfo window_info;

#if defined(OS_WIN)
    // Windows에서는 CreateWindowEx에 전달할 플래그 지정
    window_info.SetAsPopup(nullptr, "cefsimple");
#endif

    // 런타임 스타일 설정
    window_info.runtime_style = runtime_style;

    // 첫 번째 브라우저 창 생성
    CefBrowserHost::CreateBrowser(window_info, handler, url, browser_settings,
                                  nullptr, nullptr);
  }
}

// 기본 클라이언트 반환
CefRefPtr<CefClient> SimpleApp::GetDefaultClient() {
  // Chrome 스타일 UI를 통해 새 브라우저 창이 생성될 때 호출
  return SimpleHandler::GetInstance();
}
