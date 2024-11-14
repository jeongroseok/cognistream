// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef COGNISTREAM_SIMPLE_HANDLER_H_
#define COGNISTREAM_SIMPLE_HANDLER_H_

#include <list>

#include "include/cef_client.h"

// SimpleHandler 클래스는 CefClient 및 관련 핸들러를 구현합니다.
class SimpleHandler : public CefClient,
                      public CefDisplayHandler,
                      public CefLifeSpanHandler,
                      public CefLoadHandler
{
public:
  explicit SimpleHandler(bool is_alloy_style);
  ~SimpleHandler() override;

  // 이 객체의 전역 인스턴스에 접근하기 위한 정적 메소드
  static SimpleHandler *GetInstance();

  // CefClient 메소드
  CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }
  CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
  CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }

  // CefDisplayHandler 메소드
  void OnTitleChange(CefRefPtr<CefBrowser> browser,
                     const CefString &title) override;

  // CefLifeSpanHandler 메소드
  void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  bool DoClose(CefRefPtr<CefBrowser> browser) override;
  void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

  // CefLoadHandler 메소드
  void OnLoadError(CefRefPtr<CefBrowser> browser,
                   CefRefPtr<CefFrame> frame,
                   ErrorCode errorCode,
                   const CefString &errorText,
                   const CefString &failedUrl) override;
  void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int httpStatusCode) override;

  void ShowMainWindow();

  // 기존의 모든 브라우저 창을 닫도록 요청
  void CloseAllBrowsers(bool force_close);

  bool IsClosing() const { return is_closing_; }

private:
  // 플랫폼별 구현
  void PlatformTitleChange(CefRefPtr<CefBrowser> browser,
                           const CefString &title);
  void PlatformShowWindow(CefRefPtr<CefBrowser> browser);

  // 이 클라이언트가 Alloy 스타일인지 여부
  const bool is_alloy_style_;

  // 기존 브라우저 창 목록. CEF UI 스레드에서만 접근
  typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
  BrowserList browser_list_;

  bool is_closing_ = false;

  // 기본 참조 카운팅 구현 포함
  IMPLEMENT_REFCOUNTING(SimpleHandler);
};

#endif // COGNISTREAM_SIMPLE_HANDLER_H_
