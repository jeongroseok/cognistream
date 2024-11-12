This Wiki page provides an overview of the CEF architecture.

**Note to Editors: Changes made to this Wiki page without prior approval via the [CEF Forum](http://magpcss.org/ceforum/) or [Issue Tracker](https://github.com/chromiumembedded/cef/issues) may be lost or reverted.**

***
[TOC]
***

# Overview

The Chromium Embedded Framework (CEF) is an open source project founded by Marshall Greenblatt in 2008 to develop a Web browser control based on the [Google Chromium](http://www.chromium.org/Home) project. CEF currently supports a range of programming languages and operating systems and can be easily integrated into both new and existing applications. It was designed from the ground up with both performance and ease of use in mind. The base framework includes C and C++ programming interfaces exposed via native libraries that insulate the host application from Chromium and Blink implementation details. It provides close integration between the browser control and the host application, which can optionally control resource loading, navigation, context menus, printing, JavaScript bindings and more, while taking advantage of the same performance and HTML5 technologies available in the Google Chrome Web browser.

See the project [README.md](https://bitbucket.org/chromiumembedded/cef/src/master/README.md) for additional introductory material.

# Dependencies

The CEF project depends on a number of other projects maintained by third parties. The major projects that CEF depends on are:

  * [Chromium](http://www.chromium.org/Home) - Provides general functionality like network stack, threading, message loop, logging and process control needed to create a fully functional Web browser. Implements "platform" code that allows Blink to communicate with V8 and Skia. Many Chromium design documents can be found at http://dev.chromium.org/developers.
  * [Blink](https://www.chromium.org/blink) (formerly WebKit) - The rendering implementation used by Chromium. Provides DOM parsing, layout, event handling, rendering and HTML5 JavaScript APIs. Some HTML5 implementations are split between the Blink and Chromium code bases.
  * [V8](https://developers.google.com/v8/) - JavaScript engine.
  * [Skia](https://skia.org/) - 2D graphics library used for rendering non-accelerated content. More information on how Chromium integrates Skia is available [here](http://www.chromium.org/developers/design-documents/graphics-and-skia).
  * [Angle](https://chromium.googlesource.com/angle/angle/+/master/README.md) - 3D graphics translation layer for Windows that converts GLES calls to DirectX. More information about accelerated compositing is available [here](http://dev.chromium.org/developers/design-documents/gpu-accelerated-compositing-in-chrome).

# Versions

There have been three versions of CEF to date.

  * CEF1 - Single process implementation using the Chromium WebKit API (now discontinued).
  * CEF2 - Multi process implementation built on the Chromium browser (now discontinued).
  * CEF3 - Multi process implementation using either the [Chromium Content API](http://www.chromium.org/developers/content-module/content-api) (called the "Alloy runtime") or the complete Chrome UI (called the "Chrome runtime").

Only the CEF3 version is currently developed and supported. See the "History" and "Modern Architecture" sections below for background and the [BranchesAndBuilding](BranchesAndBuilding.md) Wiki page for current development information.

# History

In 2008, when the CEF project started, Chromium was a monolithic application built on top of Apple's [WebKit](https://webkit.org/) rendering engine. Google maintained an in-tree variant of WebKit (called the [Chromium WebKit API](https://www.youtube.com/watch?v=JFzC_Gx76E8)) that added additional functionality specific to Chromium. The first version of CEF (called CEF1) was implemented on top of this Chromium WebKit API and used the default single-process application model supported by WebKit1 at that time (for more details on CEF1 architecture see the "CEF1" section below).

Use of the Chromium WebKit API came with some known limitations. For example, that API layer did not support the multi-process model that was the basis for most of Google Chrome's [stability and security advantages](https://chromium.googlesource.com/playground/chromium-org-site/+/refs/heads/main/developers/design-documents/multi-process-architecture/index.md). It also lacked many of the UI features that were becoming necessary to support new web platform features (like device selection dialogs, permissions management, etc). We therefore began a new effort, in 2010, to reimplement CEF (called CEF2) on top of the fully-featured but monolithic Chromium browser UI. The lack of internal API boundaries in the Chromium code at that time made it necessary for us to take an "all or nothing" approach. If we could create a CEF browser window based on the complete Chrome UI we would effectively gain access to all of Chrome's capabilities "for free" (for more details on CEF2 architecture see the "CEF2" section below).

Google began acknowledging the validity of the embedded use case for Chromium in 2010 and started work on new internal API boundaries to make embedding easier. The [Chromium Content API](http://www.chromium.org/developers/content-module/content-api), which encapsulated the multi-process application model and provided stub implementations for most web platform features, reached a development stage in late 2011 where it was a viable option for Chromium embedders like CEF. It still didn't provide all of the features that we might like, but it was a "supported" API layer. We therefore abandoned the CEF2 effort in early 2012 and focused our efforts instead on the Content API as the more viable embedding option. The existing CEF1 API, which was strongly dependent on WebKit1 and the single-process model, could not be directly ported to the Content API. A new CEF API (called CEF3), with substantial breaking API changes and multi-process support, was released in early 2013 (for more details on CEF3 architecture see the "Modern Architecture" and "CEF3" sections below).

Also in 2013, Google decided to [fork the WebKit project](https://blog.chromium.org/2013/04/blink-rendering-engine-for-chromium.html). After renaming it to Blink they removed many of the API boundaries that had previously existed between Chromium and WebKit and effectively deleted the Chromium WebKit API. The multi-process architecture, which was optional in WebKit1, was now a requirement with Chromium/Blink. CEF1, which depended on the Chromium WebKit API, no longer had a viable future and CEF3 became the only path forward.

Fast forward another 7 years, to 2020, and the Chromium architecture has again changed substantially. A lot of effort has been spent to define clear architectural layers (sometimes called [components](https://www.chromium.org/developers/design-documents/cookbook/)) in the Chromium/Blink source code. New platforms and new embedders have entered the Chromium ecosystem. And yet, many features desired by CEF users remain stubbornly out of reach in the Chrome UI layer. With this new, more modern, version of Chromium, we decided it was time to revisit our goals from CEF2 and create a new CEF version based on the Chrome UI layer (now called the "Chrome runtime" in CEF3). Continue to the "Modern Architecture" section for details.

# Modern Architecture

The modern Chromium code base is composed of architectural layers, with the primary application layers being "content" (for the [Content API](http://www.chromium.org/developers/content-module/content-api)) and "chrome" (for the Chrome UI layer). The "content" layer includes [WebContents](https://source.chromium.org/chromium/chromium/src/+/master:content/public/browser/web_contents.h) as the primary browser object and provides implementations for most of the generic web platform and application/process management functionality. The "chrome" layer includes [Browser](https://source.chromium.org/chromium/chromium/src/+/master:chrome/browser/ui/browser.h) as the primary browser object and provides implementations for many advanced features (including datalist/autofill, extensions, gamepad, webhid, webmidi, etc.), along with standard Chrome UI toolbars and dialogs for device selection, user permissions, settings, etc.

For historical reasons (outlined above), CEF browser windows have been based on the "content" layer WebContents object since the introduction of CEF3 in 2013. Adding support for additional "chrome" layer features in that architecture was a serious effort because it involved careful understanding and cherry-picking of the related functionality from "chrome" code into the CEF code base. Over time we have added dependencies on the "chrome" layer targets to support some features in CEF (such and extensions, PDF viewer and print preview) and to avoid code duplication. As a result, CEF3 binaries in modern times already contained most/all of the code that would be required to launch a stand-alone “chrome” Browser window, but simply lacked the mechanism to do so.

In 2020 we launched a [new effort](https://github.com/chromiumembedded/cef/issues/2969) to implement the CEF3 API on top of a fully-featured "chrome" Browser window. To be usable in existing CEF3 client applications the "chrome" window needed to support a large subset of CEF3's existing callbacks/intercepts (e.g. loading progress, network requests, etc.), and be inserted into the existing [Views framework](https://www.chromium.org/developers/design-documents/chromeviews/) structure. At the same time, it was necessary to substantially refactor the internal CEF (libcef) code to extract functionality that was common to both the old WebContents-based approach and the new Browser-based approach. As part of this refactoring we renamed the existing "content" layer implementation to the "Alloy runtime" (where Alloy = Chrome + other stuff) and named the new "chrome" layer implementation as the "Chrome runtime". Proceed to the "CEF3" section below for additional feature and implementation details.

# Common API Usage

All versions of CEF expose a simple, easy-to-use API designed to insulate users from the underlying Chromium and Blink code complexity. Common usage is as follows:

1. Initialize CEF by calling CefInitialize().

2. Perform work on the UI message loop by calling CefRunMessageLoop() or CefDoMessageLoopWork().

3. Create a browser window by calling CreateBrowser() or CreateBrowserSync() and passing in a CefClient instance.

4. Shut down CEF by calling CefShutdown() before the application exits.

# C Wrapper API

The libcef shared library exports a C API that isolates the user from the CEF runtime and code base. The libcef\_dll\_wrapper project, which is distributed in source code form as part of the binary release, wraps this exported C API in a C++ API that is then linked into the client application. The code for this C/C++ API translation layer is automatically generated by the [translator](https://bitbucket.org/chromiumembedded/cef/src/master/tools/translator.README.txt?at=master) tool. Direct usage of the C API is described on the [UsingTheCAPI](UsingTheCAPI.md) Wiki page.

# CEF1 (Discontinued)

CEF1 was the initial version of CEF first released in 2008. It was discontinued in 2013 when Google forked the WebKit project and subsequently deleted the Chromium WebKit API. See the "History" section above for additional details.

The single process architecture used by CEF1 integrates Chromium and WebKit directly into the client application. Advantages to the single process architecture included reduced memory usage and closer integration with the client application. For example, having the renderer running in the same process allowed JavaScript bindings to synchronously communicate between client code and renderer code. Disadvantages included [reduced performance](https://github.com/chromiumembedded/cef/issues/304) with certain types of accelerated content, [crashes](https://github.com/chromiumembedded/cef/issues/242) due to plugins like Flash running in the same process, and lack of support for application [security and stability features](https://chromium.googlesource.com/playground/chromium-org-site/+/refs/heads/main/developers/design-documents/multi-process-architecture/index.md) that required a multi-process architecture.

## API Usage

Below is an overview of the main CEF1 interfaces and their uses. Since CEF1 is discontinued we no longer provide detailed usage information.

  * [CefApp](http://magpcss.org/ceforum/apidocs/projects/(default)/CefApp.html) - This interface is passed to CefInitialize() and allows the application to customize global functionality like resource loading and proxy handling.
  * [CefClient](http://magpcss.org/ceforum/apidocs/projects/(default)/CefClient.html) - This interface is passed to CefCreateBrowser() or CefCreateBrowserSync() and acts as the connection between an individual CEF browser instance and the client application. Additional interfaces for request handling, display handling, etc. are exposed via this interface.
  * [CefBrowser](http://magpcss.org/ceforum/apidocs/projects/(default)/CefBrowser.html) - Exposes capabilities provided by the browser. This includes back/forward navigation, source retrieval, request loading, etc. A single CefBrowser may have one or more child [CefFrame](http://magpcss.org/ceforum/apidocs/projects/(default)/CefFrame.html) objects.

## Threading Considerations

CEF1 includes UI, IO and FILE threads. The UI thread creates browser windows and is used for all interaction with WebKit and V8. The IO thread is used for handling schema and network requests. The FILE thread is used for the application cache and other miscellaneous activities.

When using CEF1 you should keep the following threading considerations in mind:

  * Do not perform blocking operations on the UI thread. This can lead to serious performance issues.
  * The UI thread will be the same as the main application thread if CefInitialize() is called with a CefSettings.multi\_threaded\_message\_loop value of false.
  * All WebKit and V8 interation must take place on the UI thread. Consequently, some CEF API functions can only be called on the UI thread. Functions that have this limitation will be documented accordingly in the associated CEF header file.
  * The [CefPostTask](http://magpcss.org/ceforum/apidocs/projects/(default)/(_globals).html#CefPostTask(CefThreadId,CefRefPtr<CefTask>)) method can be used to post tasks asnychronously between the various threads.

## Implementation Details

CEF1 has the following major implementation classes:

  * CefContext - Represents the global CEF context. A single CefContext object is created by CefInitialize() and destroyed by CefShutdown().
  * CefProcess - Used by CefContext to create and manage the CEF threads.
  * BrowserWebKitInit - Manages the global WebKit environment as exposed by the Chromium WebKit API.
  * WebViewHost - Provides the native window "wrapper" implementation for a WebView. This class extends WebWidgetHost which provides functionality shared in common with popup widgets (like select menus) on some platforms.
  * CefBrowserImpl - Implements the CefBrowser interface, creates the WebViewHost and provides the glue code for a single browser instance.
  * BrowserWebViewDelegate - Implements the WebKit interfaces that provide communication between CefBrowserImpl and the underlying WebView.

# CEF2 (Discontinued)

CEF2 was an effort to reimplement the CEF1 API on top of the Chrome browser UI as it existed in 2010. CEF2 was discontinued when Chromium announced support for the Content API. Information about CEF2 can be found in the "History" section above and [here](http://magpcss.org/ceforum/viewtopic.php?f=10&t=122) on the CEF Forum.

# CEF3

CEF3 has been the recommended and supported version of CEF since January 2013. It uses the same multi process architecture as the Chromium Web browser via either the [Chromium Content API](http://www.chromium.org/developers/content-module/content-api) (called the "Alloy runtime") or the complete Chrome UI (called the "Chrome runtime"). This architecture provides a number of advantages over the single process architecture used by CEF1:

  * Support for multi-process run mode (with related [stability and security advantages](https://chromium.googlesource.com/playground/chromium-org-site/+/refs/heads/main/developers/design-documents/multi-process-architecture/index.md)).
  * More code sharing with the Chromium browser.
  * Improved performance and less breakage due to use of the "supported" code path.
  * Faster access to new features.

In most cases CEF3 will have the same performance and stability characteristics as the Chromium Web browser. Both the Alloy runtime and the Chrome runtime support modern web platform features and provide the safety/security expected from an up-to-date Chromium-based browser. The Chrome runtime, which was introduced in 2020, adds implementations for many advanced features (including datalist/autofill, extensions, gamepad, webhid, webmidi, etc.), along with standard Chrome UI toolbars and dialogs for device selection, user permissions, settings, etc. At the time of writing, the Chrome runtime requires use of the [Views framework](https://www.chromium.org/developers/design-documents/chromeviews/) in CEF client applications and does not support off-screen rendering. A current list of open issues specific to the Chrome runtime can be found [here](https://github.com/chromiumembedded/cef/issues?q=is%3Aissue+is%3Aopen+label%3Achrome).

Starting with the M125 release (mid-2024) the Alloy runtime has been [split](https://github.com/chromiumembedded/cef/issues/3681) into separate style and bootstrap components. Both Chrome style and Alloy style browsers/windows can now be created while using the Chrome bootstrap. The Alloy bootstrap has been [removed](https://github.com/chromiumembedded/cef/issues/3685) starting with the M128 release.

## API Usage

See the [GeneralUsage](GeneralUsage.md) Wiki page for detailed API usage instructions.

## Process Considerations

See the [Processes](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md#markdown-header-processes) section of the [GeneralUsage](GeneralUsage.md) Wiki page for details.

## Threading Considerations

See the [Threads](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-threads) section of the [GeneralUsage](GeneralUsage.md) Wiki page for details.

## Implementation Details

For an implementation overview see the "Modern Architecture" section above. 

Shared CEF library/runtime classes use the Cef prefix. Classes that implement the Alloy or Chrome runtime use the Alloy or Chrome prefixes respectively. Classes that extend an existing Chrome-prefixed class add the Cef or Alloy suffix, thereby following the existing naming pattern of Chrome-derived classes.

### Alloy Runtime Classes

Alloy runtime bootstrap was [removed](https://github.com/chromiumembedded/cef/issues/3685) in M128 (mid-2024) and Alloy runtime style is now supported using the Chrome runtime bootstrap. Alloy runtime style is based on the "content" layer and has the following major implementation classes:

  * [AlloyBrowserHostImpl](https://bitbucket.org/chromiumembedded/cef/src/master/libcef/browser/alloy/alloy_browser_host_impl.h) - Implements the CefBrowser and CefBrowserHost interfaces in the browser process. Provides the glue code and implements interfaces for communicating with the RenderViewHost.

### Chrome Runtime Classes

Chrome runtime bootstrap and style is based on the "chrome" layer and has the following major implementation classes:

  * [ChromeMainDelegateCef](https://bitbucket.org/chromiumembedded/cef/src/master/libcef/common/chrome/chrome_main_delegate_cef.h) - Implements the common process bootstrap logic.
  * [ChromeContentClientCef](https://bitbucket.org/chromiumembedded/cef/src/master/libcef/common/chrome/chrome_content_client_cef.h) - Implements the Content API callbacks that are common to all processes.
  * [CefContext](https://bitbucket.org/chromiumembedded/cef/src/master/libcef/browser/context.h) - Represents the global CEF context in the browser process. A single CefContext object is created by CefInitialize() and destroyed by CefShutdown().
  * [ChromeBrowserMainExtraPartsCef](https://bitbucket.org/chromiumembedded/cef/src/master/libcef/browser/chrome/chrome_browser_main_extra_parts_cef.h) - Implements the browser process bootstrap logic.
  * [ChromeContentBrowserClientCef](https://bitbucket.org/chromiumembedded/cef/src/master/libcef/browser/chrome/chrome_content_browser_client_cef.h) - Implements the Content API callbacks for the browser process.
  * [ChromeBrowserHostImpl](https://bitbucket.org/chromiumembedded/cef/src/master/libcef/browser/chrome/chrome_browser_host_impl.h) - Implements the CefBrowser and CefBrowserHost interfaces in the browser process. Provides the glue code and implements interfaces for communicating with the RenderViewHost.
  * [ChromeContentRendererClientCef](https://bitbucket.org/chromiumembedded/cef/src/master/libcef/renderer/chrome/chrome_content_renderer_client_cef.h) - Implements the Content API callbacks for the render process.
  * [CefBrowserImpl](https://bitbucket.org/chromiumembedded/cef/src/master/libcef/renderer/browser_impl.h) - Implements the CefBrowser interface in the render process. Provides the glue code and implements interfaces for communicating with the RenderView.
  * [ChromeBrowserDelegate](https://bitbucket.org/chromiumembedded/cef/src/master/libcef/browser/chrome/chrome_browser_delegate.h) - Interface injected into Chrome's [Browser](https://source.chromium.org/chromium/chromium/src/+/master:chrome/browser/ui/browser.h) object (via [patch files](https://bitbucket.org/chromiumembedded/cef/src/master/patch/README.txt)) to support callbacks into CEF code.
  * [ChromeBrowserFrame](https://bitbucket.org/chromiumembedded/cef/src/ecdfd467f84da9bb6ef57eb4fcb2778fdef5e07a/libcef/browser/chrome/views/chrome_browser_frame.h) - Widget for a Views-hosted Chrome browser.

An overview of the Chrome Browser object model is also available [here](https://bitbucket.org/chromiumembedded/cef/src/ecdfd467f84da9bb6ef57eb4fcb2778fdef5e07a/libcef/browser/chrome/views/chrome_browser_frame.h).

This Wiki page explains how to use AddressSanitizer with CEF-based applications.

**Note to Editors: Changes made to this Wiki page without prior approval via the [CEF Forum](http://magpcss.org/ceforum/) or [Issue Tracker](https://github.com/chromiumembedded/cef/issues) may be lost or reverted.**

***
[TOC]
***

# Overview

[AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer) (ASan) is a fast memory error detector based on compiler instrumentation (LLVM). The below instructions relate to using ASan with CEF-based applications on Linux. ASan is also supported on MacOS and Windows (experimental) but usage with CEF has not been tested on those platforms. For examples of ASan output see [this bug](https://bitbucket.org/chromiumembedded/cef/issue/1457) and [this bug](https://bitbucket.org/chromiumembedded/cef/issue/1458).

# System Setup

Follow the standard [MasterBuildQuickStart](https://bitbucket.org/chromiumembedded/cef/wiki/MasterBuildQuickStart.md#markdown-header-linux-setup) instructions for Linux to configure a local CEF/Chromium build environment (stop after step 7 and continue below).

## Building CEF with ASan

To build Chromium/CEF with ASan enabled follow these steps:

1\. Configure Chromium to build using ASan (see [here](https://chromium.googlesource.com/chromium/src/+/HEAD/docs/asan.md) for complete Chromium-related ASan instructions). For example, to build on Ubuntu 18.04 using locally installed packages instead of the provided sysroot:

```
export GN_DEFINES="is_asan=true dcheck_always_on=true use_sysroot=false use_partition_alloc_as_malloc=false symbol_level=1 is_cfi=false use_thin_lto=false use_vaapi=false"
```

2\. Generate CEF project files.

```
cd /path/to/chromium_git/chromium/src/cef
./cef_create_projects.sh
```

3\. Create a Release build of CEF. ASan must always be used with Release builds.

```
cd /path/to/chromium_git/chromium/src
ninja -C out/Release_GN_x64 cef
```

4\. Run the resulting executable and pipe output to the ASan post-processing script so that stack traces will be symbolized.

```
cd /path/to/chromium_git/chromium/src
./out/Release_GN_x64/cef_unittests 2>&1 | tools/valgrind/asan/asan_symbolize.py
```

# Using CEF and ASan with third-party executables

**NOTE: This section was last updated for 2357 branch (April 2015) and specific details may be out of date.**

ASan on Linux is built as a static library by default. This works fine with applications like cefclient and cef\_unittests that are built at the same time as libcef and consequently specify the “-fsanitize=address” flag. However, in order to use libcef and ASan with pre-built third-party executables (e.g. Java and [JCEF](https://bitbucket.org/chromiumembedded/java-cef)) it’s necessary to build ASan as a shared library (see [here](https://github.com/google/sanitizers/issues/271) for more information). This works as follows (see step 8 below for an example):

A. The ASan library (“libclang\_rt.asan-x86\_64.so” on 64-bit Linux) is specified via LD\_PRELOAD so that it will be loaded into the main executable at runtime.

B. The main executable delay loads “libcef.so” which depends on “libc++.so” and “libclang\_rt.asan-x86\_64.so” provided by the Chromium Clang build.

C. Output is piped to the ASan post-processing script in order to symbolize stack traces.

To build ASan as a shared library the following changes are required to Chromium’s default Clang build and GYP configuration:

1\. Create a custom build of Clang with ASan configured as a shared library.

A. Edit the “tools/clang/scripts/update.sh” script and add "-DCOMPILER\_RT\_BUILD\_SHARED\_ASAN=ON" to all CMake command-lines (see [AddressSanitizerAsDso](https://github.com/google/sanitizers/wiki/AddressSanitizerAsDso) and [AddressSanitizerHowToBuild](https://github.com/google/sanitizers/wiki/AddressSanitizerHowToBuild) for more information).

```
diff --git tools/clang/scripts/update.sh tools/clang/scripts/update.sh
index 10a4645..bd7c5ab 100755
--- tools/clang/scripts/update.sh
+++ tools/clang/scripts/update.sh
@@ -472,6 +472,7 @@ if [[ -n "${bootstrap}" ]]; then
   pushd "${LLVM_BOOTSTRAP_DIR}"
 
   cmake -GNinja \
+      -DCOMPILER_RT_BUILD_SHARED_ASAN=ON \
       -DCMAKE_BUILD_TYPE=Release \
       -DLLVM_ENABLE_ASSERTIONS=ON \
       -DLLVM_TARGETS_TO_BUILD=host \
@@ -567,6 +568,7 @@ add_subdirectory(\${CHROMIUM_TOOLS_SRC} \${CMAKE_CURRENT_BINARY_DIR}/a)
 EOF
 rm -fv CMakeCache.txt
 MACOSX_DEPLOYMENT_TARGET=${deployment_target} cmake -GNinja \
+    -DCOMPILER_RT_BUILD_SHARED_ASAN=ON \
     -DCMAKE_BUILD_TYPE=Release \
     -DLLVM_ENABLE_ASSERTIONS=ON \
     -DLLVM_ENABLE_THREADS=OFF \
@@ -612,6 +614,7 @@ pushd "${COMPILER_RT_BUILD_DIR}"
 
 rm -fv CMakeCache.txt
 MACOSX_DEPLOYMENT_TARGET=${deployment_target} cmake -GNinja \
+    -DCOMPILER_RT_BUILD_SHARED_ASAN=ON \
     -DCMAKE_BUILD_TYPE=Release \
     -DLLVM_ENABLE_ASSERTIONS=ON \
     -DLLVM_ENABLE_THREADS=OFF \
@@ -655,6 +658,7 @@ if [[ -n "${with_android}" ]]; then
   pushd ${LLVM_BUILD_DIR}/android
   rm -fv CMakeCache.txt
   MACOSX_DEPLOYMENT_TARGET=${deployment_target} cmake -GNinja \
+      -DCOMPILER_RT_BUILD_SHARED_ASAN=ON \
       -DCMAKE_BUILD_TYPE=Release \
       -DLLVM_ENABLE_ASSERTIONS=ON \
       -DLLVM_ENABLE_THREADS=OFF \
```

B. Run the “tools/clang/scripts/update.sh” script to create a local build of Clang.

```
cd /path/to/chromium_git/chromium/src
./tools/clang/scripts/update.sh --force-local-build --without-android
```

This will create “libclang\_rt.asan-x86\_64.so” (assuming 64-bit Linux) in the “third\_party/llvm-build/Release+Asserts/lib/clang/3.7.0/lib/linux” directory.

2\. Copy “libclang\_rt.asan-x86\_64.so” to the “out/Release/lib” directory so that binaries built as part of the Chromium build can find it.

3\. Add '-shared-libasan' or modify related exclusions everywhere that ‘-fsanitize=address’ is mentioned for Linux in “build/common.gypi”, “build/sanitizers/sanitizers.gyp”, “sandbox/linux/sandbox\_linux.gypi” and “third\_party/libvpx/libvpx.gyp” (see [AddressSanitizerAsDso](https://github.com/google/sanitizers/wiki/AddressSanitizerAsDso) for details). Also, specify '-mllvm -asan-globals=0' in “base/common.gypi” (see [here](https://github.com/google/sanitizers/issues/82#c18) for details).

```
diff --git build/common.gypi build/common.gypi
index 066b0b4..6756a25 100644
--- build/common.gypi
+++ build/common.gypi
@@ -4292,16 +4292,18 @@
               ['_toolset=="target"', {
                 'cflags': [
                   '-fsanitize=address',
+                  '-shared-libasan',
                   # TODO(earthdok): Re-enable. http://crbug.com/427202
                   #'-fsanitize-blacklist=<(asan_blacklist)',
                 ],
                 'ldflags': [
                   '-fsanitize=address',
+                  '-shared-libasan',
                 ],
               }],
             ],
             'conditions': [
-              ['OS=="mac"', {
+              ['OS=="mac" or OS=="linux"', {
                 'cflags': [
                   '-mllvm -asan-globals=0',  # http://crbug.com/352073
                 ],
@@ -4865,9 +4867,11 @@
               # binaries on x86_64 host is problematic.
               # TODO(eugenis): re-enable.
               '-fsanitize=address',
+              '-shared-libasan',
             ],
             'ldflags!': [
               '-fsanitize=address',
+              '-shared-libasan',
               '-Wl,-z,noexecstack',
               '-Wl,--gc-sections',
               '-Wl,-O1',
diff --git build/sanitizers/sanitizers.gyp build/sanitizers/sanitizers.gyp
index 4126d22..1e3ef49 100644
--- build/sanitizers/sanitizers.gyp
+++ build/sanitizers/sanitizers.gyp
@@ -45,6 +45,7 @@
       'cflags/': [
         ['exclude', '-fsanitize='],
         ['exclude', '-fsanitize-'],
+        ['exclude', '-shared-libasan'],
       ],
       'direct_dependent_settings': {
         'ldflags': [
diff --git sandbox/linux/sandbox_linux.gypi sandbox/linux/sandbox_linux.gypi
index 4305b41..9ca48de 100644
--- sandbox/linux/sandbox_linux.gypi
+++ sandbox/linux/sandbox_linux.gypi
@@ -213,9 +213,11 @@
       # Do not use any sanitizer tools with this binary. http://crbug.com/382766
       'cflags/': [
         ['exclude', '-fsanitize'],
+        ['exclude', '-shared-libasan'],
       ],
       'ldflags/': [
         ['exclude', '-fsanitize'],
+        ['exclude', '-shared-libasan'],
       ],
     },
     { 'target_name': 'sandbox_services',
```

4\. Build CEF with ASan (see the "Building CEF with ASan" section above).

5\. Copy “libcef.so”, “libc++.so” and “libclang\_rt.asan-x86\_64.so” from the “out/Release/lib” directory to the third-party project’s binary directory (e.g. “out/Debug” for JCEF).

6\. Run the third-party executable pre-loading “libclang\_rt.asan-x86\_64.so” and piping output to the ASan post-processing script so that stack traces will be symbolized. For example, using JCEF’s [run.sh script](https://bitbucket.org/chromiumembedded/java-cef/src/master/tools/run.sh):

```
LD_PRELOAD=$LIB_PATH/libclang_rt.asan-x86_64.so java -cp "$CLS_PATH" -Djava.library.path=$LIB_PATH tests.$RUN_TYPE.MainFrame "$@" 2>&1 | /path/to/chromium_git/chromium/src/tools/valgrind/asan/asan_symbolize.py
```

This Wiki page provides information about CEF branches and instructions for downloading, building and packaging source code.

**Note to Editors: Changes made to this Wiki page without prior approval via the [CEF Forum](http://magpcss.org/ceforum/) or [Issue Tracker](https://github.com/chromiumembedded/cef/issues) may be lost or reverted.**

***
[TOC]
***

# Background

The CEF project is an extension of the Chromium project hosted at [chromium.org](https://www.chromium.org). CEF maintains development and release branches that track Chromium branches. CEF source code can be built manually or with automated tools.

# Development

Ongoing development of CEF occurs in the [CEF master branch](https://bitbucket.org/chromiumembedded/cef/src?at=master). This location tracks the current [Chromium master branch](https://chromium.googlesource.com/chromium/src.git) and is not recommended for production use.

Current CEF master branch build requirements are as follows. See the [MasterBuildQuickStart](https://bitbucket.org/chromiumembedded/cef/wiki/MasterBuildQuickStart.md) Wiki page for a development build quick-start guide.

Windows Build Requirements | macOS Build Requirements | Linux Build Requirements |
|:---------------------------|:----------------------------|:-------------------------|
Win 10+ deployment, Win 10+ build system w/ VS2022 17.9.2, Win 10.0.22621.2428 SDK, Ninja | macOS 11.0+ deployment, 14.5+ build system w/ 15.0 base SDK (Xcode 16.0), Ninja, 64-bit only | Ubuntu 20.04+, Debian 10+, Ninja |

The following URLs should be used for downloading development versions of CEF.

  * CEF3 - https://bitbucket.org/chromiumembedded/cef/src?at=master

CEF1 is no longer actively developed or supported. See the [CEF1 Retirement Plan](http://magpcss.org/ceforum/viewtopic.php?f=10&t=10647) for details.

# Release Branches

CEF branches are created to track each Chromium release milesone (MXX) branch. Users developing applications for production environments are encouraged to use release branches for the following reasons:

  * Binary CEF builds are tied to specific Chromium releases.
  * Release versions of CEF/Chromium are better tested and more appropriate for release applications.
  * Within a release branch the CEF API is "frozen" and generally only security/bug fixes are applied.
  * CEF release branches can include patches to Chromium/Blink source if necessary.
  * CEF master development won't interfere with consumer release schedules.

CEF release branches and associated platform build requirements are described below.

## Version Number Format

The CEF version number format was changed in 2019 to include more relevant information and to provide compatibility with the [Semantic Versioning 2.0](https://semver.org/spec/v2.0.0.html) standard.

Newer CEF release version numbers have the format "X.Y.Z+gHHHHHHH+chromium-A.B.C.D" where:

* "X" is the Chromium major version (e.g. 73).
* "Y" is an incremental number that starts at 0 when a release branch is created and changes only when the CEF C/C++ API changes (as determined by updates to the include/cef\_api\_hash.h file).
* "Z" is an incremental number that starts at 0 when a release branch is created and changes on each commit, with reset to 0 when "Y" changes.
* "gHHHHHHH" is the 7-character abbreviation for the Git commit hash. This facilitates lookup of the relevant commit history in Git.
* "A.B.C.D" is the Chromium version (e.g. 73.0.3683.75).

Older CEF release version numbers have the format X.YYYY.A.gHHHHHHH where:

  * "X" is the CEF major version (e.g. 3).
  * "YYYY" is the Chromium branch.
  * "A" is an incremental number representing the number of commits in the current branch. This is roughly equivalent to the SVN revision number but on a per-branch basis and assists people in quickly determining the order of builds in the same branch (for bug reports, etc).
  * "gHHHHHHH" is the 7-character abbreviation for the Git commit hash. This facilitates lookup of the relevant commit history in Git.

Detailed Chromium and CEF version information is available in the include/cef\_version.h header file which is created during the build process or by loading the “about:version” URL in a CEF-derived application.

## Current Release Branches (Supported)

Support for newer branches begins when they enter the Chromium beta channel. Support for older branches ends when they exit the Chromium stable channel. The [Spotify automated builder](https://cef-builds.spotifycdn.com/index.html) provides CEF builds for the current Chromium stable channel and will switch to the next Chromium branch when that branch is promoted to the stable channel. Updating CEF branches is currently a manual process so there will likely be a delay between [Chromium release announcements](http://googlechromereleases.blogspot.com/) and the availability of associated CEF builds. See the [Chromium release calendar](https://chromiumdash.appspot.com/schedule) for estimated Chromium release dates and versions.

| Branch Date | Release Branch | Chromium Version | CEF Version | Windows Build Requirements | macOS Build Requirements | Linux Build Requirements |
|:------------|:---------------|:-----------------|:------------|:---------------------------|:----------------------------|:-------------------------|
| Oct 2024    | [6778](https://bitbucket.org/chromiumembedded/cef/src/6778?at=6778) | 131              | 131        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.9.2, Win 10.0.22621.2428 SDK, Ninja | macOS 11.0+ deployment, 14.5+ build system w/ 15.0 base SDK (Xcode 16.0), Ninja, 64-bit only | Ubuntu 20.04+, Debian 10+, Ninja |
| Sep 2024    | [6723](https://bitbucket.org/chromiumembedded/cef/src/6723?at=6723) | 130              | 130        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.9.2, Win 10.0.22621.2428 SDK, Ninja | macOS 11.0+ deployment, 13.5+ build system w/ 14.0 base SDK (Xcode 15.0), Ninja, 64-bit only | Ubuntu 20.04+, Debian 10+, Ninja |

## Legacy Release Branches (Unsupported)

Legacy CEF builds are available from the [Spotify automated builder](https://cef-builds.spotifycdn.com/index.html) back to 2704 branch. Building legacy branches is not supported. If you choose to build a legacy branch you will need to solve any build errors on your own.

  * Newer legacy branches (within the past year) can often be built using the same tooling as current branches. You will need to (a) manually download depot_tools, (b) sync depot_tools to a revision that existed at the time of the branch, and (c) set the `DEPOT_TOOLS_UPDATE=0` environment variable to keep it from updating automatically.
  * Older legacy branches can potentially be built by downloading a CEF source archive at the desired branch from [here](https://bitbucket.org/chromiumembedded/cef/downloads?tab=branches) and a Chromium source archive at the associated/required version from [here](https://gsdview.appspot.com/chromium-browser-official/), and then combining them to create the required directory structure.
  * See the Build Notes section at the end of this page for historical build details.

| Branch Date | Release Branch | Chromium Version | CEF Version | Windows Build Requirements | macOS Build Requirements | Linux Build Requirements |
|:------------|:---------------|:-----------------|:------------|:---------------------------|:----------------------------|:-------------------------|
| Aug 2024    | [6668](https://bitbucket.org/chromiumembedded/cef/src/6668?at=6668) | 129              | 129        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.9.2, Win 10.0.22621.2428 SDK, Ninja | macOS 11.0+ deployment, 13.5+ build system w/ 14.0 base SDK (Xcode 15.0), Ninja, 64-bit only | Ubuntu 20.04+, Debian 10+, Ninja |
| Jul 2024    | [6613](https://bitbucket.org/chromiumembedded/cef/src/6613?at=6613) | 128              | 128        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.9.2, Win 10.0.22621 SDK, Ninja | macOS 10.15+ deployment, 13.5+ build system w/ 14.0 base SDK (Xcode 15.0), Ninja, 64-bit only | Ubuntu 20.04+, Debian 10+, Ninja |
| Jun 2024    | [6533](https://bitbucket.org/chromiumembedded/cef/src/6533?at=6533) | 127              | 127        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.9.2, Win 10.0.22621 SDK, Ninja | macOS 10.15+ deployment, 13.5+ build system w/ 14.0 base SDK (Xcode 15.0), Ninja, 64-bit only | Ubuntu 20.04+, Debian 10+, Ninja |
| May 2024    | [6478](https://bitbucket.org/chromiumembedded/cef/src/6478?at=6478) | 126              | 126        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.9.2, Win 10.0.22621 SDK, Ninja | macOS 10.15+ deployment, 13.5+ build system w/ 14.0 base SDK (Xcode 15.0), Ninja, 64-bit only | Ubuntu 20.04+, Debian 10+, Ninja |
| Apr 2024    | [6422](https://bitbucket.org/chromiumembedded/cef/src/6422?at=6422) | 125              | 125        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.5+, Win 10.0.22621 SDK, Ninja | macOS 10.15+ deployment, 13.5+ build system w/ 14.0 base SDK (Xcode 15.0), Ninja, 64-bit only | Ubuntu 20.04+, Debian 10+, Ninja |
| Mar 2024    | [6367](https://bitbucket.org/chromiumembedded/cef/src/6367?at=6367) | 124              | 124        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.5+, Win 10.0.22621 SDK, Ninja | macOS 10.15+ deployment, 13.5+ build system w/ 14.0 base SDK (Xcode 15.0), Ninja, 64-bit only | Ubuntu 20.04+, Debian 10+, Ninja |
| Feb 2024    | [6312](https://bitbucket.org/chromiumembedded/cef/src/6312?at=6312) | 123              | 123        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.5+, Win 10.0.22621 SDK, Ninja | macOS 10.15+ deployment, 13.5+ build system w/ 14.0 base SDK (Xcode 15.0), Ninja, 64-bit only | Ubuntu 20.04+, Debian 10+, Ninja |
| Jan 2024    | [6261](https://bitbucket.org/chromiumembedded/cef/src/6261?at=6261) | 122              | 122        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.5+, Win 10.0.22621 SDK, Ninja | macOS 10.15+ deployment, 13.5+ build system w/ 14.0 base SDK (Xcode 15.0), Ninja, 64-bit only | Ubuntu 20.04+, Debian 10+, Ninja |
| Dec 2023    | [6167](https://bitbucket.org/chromiumembedded/cef/src/6167?at=6167) | 121              | 121        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.5+, Win 10.0.22621 SDK, Ninja | macOS 10.15+ deployment, 13.5+ build system w/ 14.0 base SDK (Xcode 15.0), Ninja, 64-bit only | Ubuntu 20.04+, Debian 10+, Ninja |
| Nov 2023    | [6099](https://bitbucket.org/chromiumembedded/cef/src/6099?at=6099) | 120              | 120        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.5+, Win 10.0.22621 SDK, Ninja | macOS 10.15+ deployment, 13.5+ build system w/ 14.0 base SDK (Xcode 15.0), Ninja, 64-bit only | Ubuntu 20.04+, Debian 10+, Ninja |
| Oct 2023    | [6045](https://bitbucket.org/chromiumembedded/cef/src/6045?at=6045) | 119              | 119        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.5+, Win 10.0.22621 SDK, Ninja | macOS 10.15+ deployment, 13.5+ build system w/ 14.0 base SDK (Xcode 15.0), Ninja, 64-bit only | Ubuntu 20.04+, Debian 10+, Ninja |
| Sep 2023    | [5993](https://bitbucket.org/chromiumembedded/cef/src/5993?at=5993) | 118              | 118        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.5+, Win 10.0.22621 SDK, Ninja | macOS 10.15+ deployment, 13.0+ build system w/ 13.3 base SDK (Xcode 14.3), Ninja, 64-bit only | Ubuntu 20.04+, Debian 10+, Ninja |
| Aug 2023    | [5938](https://bitbucket.org/chromiumembedded/cef/src/5938?at=5938) | 117              | 117        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.5+, Win 10.0.22621 SDK, Ninja | macOS 10.15+ deployment, 13.0+ build system w/ 13.3 base SDK (Xcode 14.3), Ninja, 64-bit only | Ubuntu 20.04+, Debian 10+, Ninja |
| Jun 2023    | [5845](https://bitbucket.org/chromiumembedded/cef/src/5845?at=5845) | 116              | 116        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.5+, Win 10.0.22621 SDK, Ninja | macOS 10.13+ deployment, 13.0+ build system w/ 13.3 base SDK (Xcode 14.3), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| May 2023    | [5790](https://bitbucket.org/chromiumembedded/cef/src/5790?at=5790) | 115              | 115        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.5+, Win 10.0.22621 SDK, Ninja | macOS 10.13+ deployment, 13.0+ build system w/ 13.3 base SDK (Xcode 14.3), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Apr 2023    | [5735](https://bitbucket.org/chromiumembedded/cef/src/5735?at=5735) | 114              | 114        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.5+, Win 10.0.22621 SDK, Ninja | macOS 10.13+ deployment, 12.5+ build system w/ 13.0 base SDK (Xcode 14.0-14.3), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Mar 2023    | [5672](https://bitbucket.org/chromiumembedded/cef/src/5672?at=5672) | 113              | 113        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.5+, Win 10.0.22621 SDK, Ninja | macOS 10.13+ deployment, 12.5+ build system w/ 13.0 base SDK (Xcode 14.0), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Feb 2023    | [5615](https://bitbucket.org/chromiumembedded/cef/src/5615?at=5615) | 112              | 112        | Win 10+ deployment, Win 10+ build system w/ VS2022 17.5+, Win 10.0.22621 SDK, Ninja | macOS 10.13+ deployment, 12.5+ build system w/ 13.0 base SDK (Xcode 14.0), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Jan 2023    | [5563](https://bitbucket.org/chromiumembedded/cef/src/5563?at=5563) | 111              | 111        | Win 10+ deployment, Win 10+ build system w/ VS2019 16.11.13+, Win 10.0.20348 SDK, Ninja | macOS 10.13+ deployment, 12.5+ build system w/ 13.0 base SDK (Xcode 14.0), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Dec 2022    | [5481](https://bitbucket.org/chromiumembedded/cef/src/5481?at=5481) | 110              | 110        | Win 10+ deployment, Win 10+ build system w/ VS2019 16.11.13+, Win 10.0.20348 SDK, Ninja | macOS 10.13+ deployment, 12.5+ build system w/ 13.0 base SDK (Xcode 14.0), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Nov 2022    | [5414](https://bitbucket.org/chromiumembedded/cef/src/5414?at=5414) | 109              | 109        | Win 7+ deployment, Win 10+ build system w/ VS2019 16.11.13+, Win 10.0.20348 SDK, Ninja | macOS 10.13+ deployment, 12.5+ build system w/ 13.0 base SDK (Xcode 14.0), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Oct 2022    | [5359](https://bitbucket.org/chromiumembedded/cef/src/5359?at=5359) | 108              | 108        | Win 7+ deployment, Win 10+ build system w/ VS2019 16.11.13+, Win 10.0.20348 SDK, Ninja | macOS 10.13+ deployment, 12+ build system w/ 12.3 base SDK (Xcode 13.3), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Sep 2022    | [5304](https://bitbucket.org/chromiumembedded/cef/src/5304?at=5304) | 107              | 107        | Win 7+ deployment, Win 10+ build system w/ VS2019 16.11.13+, Win 10.0.20348 SDK, Ninja | macOS 10.13+ deployment, 12+ build system w/ 12.3 base SDK (Xcode 13.3), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Aug 2022    | [5249](https://bitbucket.org/chromiumembedded/cef/src/5249?at=5249) | 106              | 106        | Win 7+ deployment, Win 10+ build system w/ VS2019 16.11.13+, Win 10.0.20348 SDK, Ninja | macOS 10.13+ deployment, 12+ build system w/ 12.3 base SDK (Xcode 13.3), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Jul 2022    | [5195](https://bitbucket.org/chromiumembedded/cef/src/5195?at=5195) | 105              | 105        | Win 7+ deployment, Win 10+ build system w/ VS2019 16.11.13+, Win 10.0.20348 SDK, Ninja | macOS 10.13+ deployment, 12+ build system w/ 12.3 base SDK (Xcode 13.3), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Jun 2022    | [5112](https://bitbucket.org/chromiumembedded/cef/src/5112?at=5112) | 104              | 104        | Win 7+ deployment, Win 10+ build system w/ VS2019 16.11.13+, Win 10.0.20348 SDK, Ninja | macOS 10.13+ deployment, 12+ build system w/ 12.3 base SDK (Xcode 13.3), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| May 2022    | [5060](https://bitbucket.org/chromiumembedded/cef/src/5060?at=5060) | 103              | 103        | Win 7+ deployment, Win 10+ build system w/ VS2019 16.11.13+, Win 10.0.20348 SDK, Ninja | macOS 10.11+ deployment, 12+ build system w/ 12.3 base SDK (Xcode 13.3), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Apr 2022    | [5005](https://bitbucket.org/chromiumembedded/cef/src/5005?at=5005) | 102              | 102        | Win 7+ deployment, Win 10+ build system w/ VS2019 16.11.13+, Win 10.0.20348 SDK, Ninja | macOS 10.11+ deployment, 12+ build system w/ 12.3 base SDK (Xcode 13.3), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Mar 2022    | [4951](https://bitbucket.org/chromiumembedded/cef/src/4951?at=4951) | 101              | 101        | Win 7+ deployment, Win 10+ build system w/ VS2019 16.8.4+, Win 10.0.19041 SDK, Ninja | macOS 10.11+ deployment, 11.3+ build system w/ 12.1 base SDK (Xcode 13.2.1), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Feb 2022    | [4896](https://bitbucket.org/chromiumembedded/cef/src/4896?at=4896) | 100              | 100        | Win 7+ deployment, Win 10+ build system w/ VS2019 16.8.4+, Win 10.0.19041 SDK, Ninja | macOS 10.11+ deployment, 11.3+ build system w/ 12.1 base SDK (Xcode 13.2.1), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Jan 2022    | [4844](https://bitbucket.org/chromiumembedded/cef/src/4844?at=4844) | 99               | 99         | Win 7+ deployment, Win 10+ build system w/ VS2019 16.8.4+, Win 10.0.19041 SDK, Ninja | macOS 10.11+ deployment, 10.15.4+ build system w/ 11.0 base SDK (Xcode 12.2-13.0), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Dec 2021    | [4758](https://bitbucket.org/chromiumembedded/cef/src/4758?at=4758) | 98               | 98         | Win 7+ deployment, Win 10+ build system w/ VS2019 16.8.4+, Win 10.0.19041 SDK, Ninja | macOS 10.11+ deployment, 10.15.4+ build system w/ 11.0 base SDK (Xcode 12.2-13.0), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Nov 2021    | [4692](https://bitbucket.org/chromiumembedded/cef/src/4692?at=4692) | 97               | 97         | Win 7+ deployment, Win 10+ build system w/ VS2019 16.8.4+, Win 10.0.19041 SDK, Ninja | macOS 10.11+ deployment, 10.15.4+ build system w/ 11.0 base SDK (Xcode 12.2-13.0), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Oct 2021    | [4664](https://bitbucket.org/chromiumembedded/cef/src/4664?at=4664) | 96               | 96         | Win 7+ deployment, Win 10+ build system w/ VS2019 16.8.4+, Win 10.0.19041 SDK, Ninja | macOS 10.11+ deployment, 10.15.4+ build system w/ 11.0 base SDK (Xcode 12.2-13.0), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Sep 2021    | [4638](https://bitbucket.org/chromiumembedded/cef/src/4638?at=4638) | 95               | 95         | Win 7+ deployment, Win 10+ build system w/ VS2019 16.8.4+, Win 10.0.19041 SDK, Ninja | macOS 10.11+ deployment, 10.15.4+ build system w/ 11.0 base SDK (Xcode 12.2-12.5), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Aug 2021    | [4606](https://bitbucket.org/chromiumembedded/cef/src/4606?at=4606) | 94               | 94         | Win 7+ deployment, Win 10+ build system w/ VS2019 16.8.4+, Win 10.0.19041 SDK, Ninja | macOS 10.11+ deployment, 10.15.4+ build system w/ 11.0 base SDK (Xcode 12.2-12.5), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Jul 2021    | [4577](https://bitbucket.org/chromiumembedded/cef/src/4577?at=4577) | 93               | 93         | Win 7+ deployment, Win 10+ build system w/ VS2019 16.8.4+, Win 10.0.19041 SDK, Ninja | macOS 10.11+ deployment, 10.15.4+ build system w/ 11.0 base SDK (Xcode 12.2-12.5), Ninja, 64-bit only | Ubuntu 18.04+, Debian 10+, Ninja |
| Jun 2021    | [4515](https://bitbucket.org/chromiumembedded/cef/src/4515?at=4515) | 92               | 92         | Win 7+ deployment, Win 10+ build system w/ VS2019 16.8.4+, Win 10.0.19041 SDK, Ninja | macOS 10.11+ deployment, 10.15.4+ build system w/ 11.0 base SDK (Xcode 12.2-12.5), Ninja, 64-bit only | Ubuntu 16.04+, Debian Sid+, Ninja |
| Apr 2021    | [4472](https://bitbucket.org/chromiumembedded/cef/src/4472?at=4472) | 91               | 91         | Win 7+ deployment, Win 10+ build system w/ VS2019 16.8.4+, Win 10.0.19041 SDK, Ninja | macOS 10.11+ deployment, 10.15.4+ build system w/ 11.0 base SDK (Xcode 12.2-12.5), Ninja, 64-bit only | Ubuntu 16.04+, Debian Sid+, Ninja |
| Mar 2021    | [4430](https://bitbucket.org/chromiumembedded/cef/src/4430?at=4430) | 90               | 90         | Win 7+ deployment, Win 10+ build system w/ VS2019 16.8.4+, Win 10.0.19041 SDK, Ninja | macOS 10.11+ deployment, 10.15.4+ build system w/ 11.0 base SDK (Xcode 12.2), Ninja, 64-bit only | Ubuntu 16.04+, Debian Sid+, Ninja |
| Feb 2021    | [4389](https://bitbucket.org/chromiumembedded/cef/src/4389?at=4389) | 89               | 89         | Win 7+ deployment, Win 10+ build system w/ VS2017 15.7.1+, Win 10.0.19041 SDK, Ninja | macOS 10.11+ deployment, 10.15.4+ build system w/ 11.0 base SDK (Xcode 12.2), Ninja, 64-bit only | Ubuntu 16.04+, Debian Sid+, Ninja |
| Dec 2020    | [4324](https://bitbucket.org/chromiumembedded/cef/src/4324?at=4324) | 88               | 88         | Win 7+ deployment, Win 10+ build system w/ VS2017 15.7.1+, Win 10.0.19041 SDK, Ninja | macOS 10.11+ deployment, 10.15.4+ build system w/ 11.0 base SDK (Xcode 12.2), Ninja, 64-bit only | Ubuntu 16.04+, Debian Sid+, Ninja |
| Oct 2020    | [4280](https://bitbucket.org/chromiumembedded/cef/src/4280?at=4280) | 87               | 87         | Win 7+ deployment, Win 10+ build system w/ VS2017 15.7.1+, Win 10.0.19041 SDK, Ninja | macOS 10.10+ deployment, 10.14.4+ build system w/ 10.15.1 base SDK (Xcode 11.2), Ninja, 64-bit only | Ubuntu 16.04+, Debian Sid+, Ninja |
| Sep 2020    | [4240](https://bitbucket.org/chromiumembedded/cef/src/4240?at=4240) | 86               | 86         | Win 7+, VS2017 15.7.1+, Win 10.0.19041 SDK, Ninja | macOS 10.10-10.15, 10.10+ deployment target, 10.14.4+ build system w/ 10.15.1 base SDK (Xcode 11.2), Ninja, 64-bit only | Ubuntu 16.04+, Debian Sid+, Ninja |
| Jul 2020    | [4183](https://bitbucket.org/chromiumembedded/cef/src/4183?at=4183) | 85               | 85         | Win 7+, VS2017 15.7.1+, Win 10.0.19041 SDK, Ninja | macOS 10.10-10.15, 10.10+ deployment target, 10.14.4+ build system w/ 10.15.1 base SDK (Xcode 11.2), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Jun 2020    | [4147](https://bitbucket.org/chromiumembedded/cef/src/4147?at=4147) | 84               | 84         | Win 7+, VS2017 15.7.1+, Win 10.0.18362 SDK, Ninja | macOS 10.10-10.15, 10.10+ deployment target, 10.14.4+ build system w/ 10.15.1 base SDK (Xcode 11.2), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Apr 2020    | [4103](https://bitbucket.org/chromiumembedded/cef/src/4103?at=4103) | 83               | 83         | Win 7+, VS2017 15.7.1+, Win 10.0.18362 SDK, Ninja | macOS 10.10-10.15, 10.10+ deployment target, 10.14.4+ build system w/ 10.15.1 base SDK (Xcode 11.2), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Mar 2020    | [4044](https://bitbucket.org/chromiumembedded/cef/src/4044?at=4044) | 81               | 81         | Win 7+, VS2017 15.7.1+, Win 10.0.18362 SDK, Ninja | macOS 10.10-10.14, 10.10+ deployment target, 10.13.2+ build system w/ 10.13+ base SDK (Xcode 9.3.1+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Feb 2020    | [3987](https://bitbucket.org/chromiumembedded/cef/src/3987?at=3987) | 80               | 80         | Win 7+, VS2017 15.7.1+, Win 10.0.18362 SDK, Ninja | macOS 10.10-10.14, 10.10+ deployment target, 10.13.2+ build system w/ 10.13+ base SDK (Xcode 9.3.1+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Nov 2019    | [3945](https://bitbucket.org/chromiumembedded/cef/src/3945?at=3945) | 79               | 79         | Win 7+, VS2017 15.7.1+, Win 10.0.18362 SDK, Ninja | macOS 10.10-10.14, 10.10+ deployment target, 10.13.2+ build system w/ 10.13+ base SDK (Xcode 9.3.1+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Oct 2019    | [3904](https://bitbucket.org/chromiumembedded/cef/src/3904?at=3904) | 78               | 78         | Win 7+, VS2017 15.7.1+, Win 10.0.18362 SDK, Ninja | macOS 10.10-10.14, 10.10+ deployment target, 10.13.2+ build system w/ 10.13+ base SDK (Xcode 9.3.1+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Sep 2019    | [3865](https://bitbucket.org/chromiumembedded/cef/src/3865?at=3865) | 77               | 77         | Win 7+, VS2017 15.7.1+, Win 10.0.18362 SDK, Ninja | macOS 10.10-10.14, 10.10+ deployment target, 10.13.2+ build system w/ 10.13+ base SDK (Xcode 9.3.1+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Jul 2019    | [3809](https://bitbucket.org/chromiumembedded/cef/src/3809?at=3809) | 76               | 76         | Win 7+, VS2017 15.7.1+, Win 10.0.17763 SDK, Ninja | macOS 10.10-10.14, 10.10+ deployment target, 10.13.2+ build system w/ 10.13+ base SDK (Xcode 9.3.1+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Jun 2019    | [3770](https://bitbucket.org/chromiumembedded/cef/src/3770?at=3770) | 75               | 75         | Win 7+, VS2017 15.7.1+, Win 10.0.17763 SDK, Ninja | macOS 10.10-10.14, 10.10+ deployment target, 10.13.2+ build system w/ 10.13+ base SDK (Xcode 9.3.1+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Mar 2019    | [3729](https://bitbucket.org/chromiumembedded/cef/src/3729?at=3729) | 74               | 74, 3      | Win 7+, VS2017 15.7.1+, Win 10.0.17763 SDK, Ninja | macOS 10.10-10.14, 10.10+ deployment target, 10.13.2+ build system w/ 10.13+ base SDK (Xcode 9.3.1+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Feb 2019    | [3683](https://bitbucket.org/chromiumembedded/cef/src/3683?at=3683) | 73               | 73, 3      | Win 7+, VS2017 15.7.1+, Win 10.0.17134.0 or 10.0.17763 SDK, Ninja | macOS 10.10-10.14, 10.10+ deployment target, 10.12+ build system w/ 10.12+ base SDK (Xcode 8.3.2+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Dec 2018    | [3626](https://bitbucket.org/chromiumembedded/cef/src/3626?at=3626) | 72               | 3          | Win 7+, VS2017 15.7.1+, Win 10.0.17134.0 or 10.0.17763 SDK, Ninja | macOS 10.10-10.14, 10.10+ deployment target, 10.12+ build system w/ 10.12+ base SDK (Xcode 8.3.2+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Oct 2018    | [3578](https://bitbucket.org/chromiumembedded/cef/src/3578?at=3578) | 71               | 3          | Win 7+, VS2017 15.7.1+, Win10.0.17134.0 SDK, Ninja | macOS 10.10-10.14, 10.10+ deployment target, 10.12+ build system w/ 10.12+ base SDK (Xcode 8.3.2+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Sep 2018    | [3538](https://bitbucket.org/chromiumembedded/cef/src/3538?at=3538) | 70               | 3          | Win 7+, VS2017 15.7.1+, Win10.0.17134.0 SDK, Ninja | macOS 10.10-10.13, 10.10+ deployment target, 10.12+ build system w/ 10.12+ base SDK (Xcode 8.3.2+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Jul 2018    | [3497](https://bitbucket.org/chromiumembedded/cef/src/3497?at=3497) | 69               | 3          | Win 7+, VS2017 15.7.1+, Win10.0.17134.0 SDK, Ninja | macOS 10.10-10.13, 10.10+ deployment target, 10.12+ build system w/ 10.12+ base SDK (Xcode 8.3.2+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Jun 2018    | [3440](https://bitbucket.org/chromiumembedded/cef/src/3440?at=3440) | 68               | 3          | Win 7+, VS2017 15.7.1+, Win10.0.17134.0 SDK, Ninja | macOS 10.10-10.13, 10.10+ deployment target, 10.12+ build system w/ 10.12+ base SDK (Xcode 8.3.2+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Apr 2018    | [3396](https://bitbucket.org/chromiumembedded/cef/src/3396?at=3396) | 67               | 3          | Win 7+, VS2017 15.3.2+, Win10.0.15063.468 SDK, Ninja | macOS 10.10-10.13, 10.10+ deployment target, 10.12+ build system w/ 10.12+ base SDK (Xcode 8.3.2+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Mar 2018    | [3359](https://bitbucket.org/chromiumembedded/cef/src/3359?at=3359) | 66               | 3          | Win 7+, VS2017 15.3.2+, Win10.0.15063.468 SDK, Ninja | macOS 10.10-10.13, 10.10+ deployment target, 10.12+ build system w/ 10.12+ base SDK (Xcode 8.3.2+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Jan 2018    | [3325](https://bitbucket.org/chromiumembedded/cef/src/3325?at=3325) | 65               | 3          | Win 7+, VS2017 15.3.2+, Win10.0.15063.468 SDK, Ninja | macOS 10.10-10.13, 10.10+ deployment target, 10.12+ build system w/ 10.12+ base SDK (Xcode 8.3.2+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Dec 2017    | [3282](https://bitbucket.org/chromiumembedded/cef/src/3282?at=3282) | 64               | 3          | Win 7+, VS2017 15.3.2+, Win10.0.15063.468 SDK, Ninja | macOS 10.10-10.13, 10.10+ deployment target, 10.12+ build system w/ 10.12+ base SDK (Xcode 8.3.2+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Oct 2017    | [3239](https://bitbucket.org/chromiumembedded/cef/src/3239?at=3239) | 63               | 3          | Win 7+, VS2017 15.3.2+, Win10.0.15063.468 SDK, Ninja | macOS 10.10-10.13, 10.10+ deployment target, 10.12+ build system w/ 10.12+ base SDK (Xcode 8.3.2+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Sep 2017    | [3202](https://bitbucket.org/chromiumembedded/cef/src/3202?at=3202) | 62               | 3          | Win 7+, VS2015u3, Win10.0.14393 SDK, Ninja | macOS 10.10-10.12, 10.10+ deployment target, 10.12+ build system w/ 10.12+ base SDK (Xcode 8.3.2+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Jul 2017    | [3163](https://bitbucket.org/chromiumembedded/cef/src/3163?at=3163) | 61               | 3          | Win 7+, VS2015u3, Win10.0.14393 SDK, Ninja | macOS 10.9-10.12, 10.9+ deployment target, 10.9+ build system w/ 10.10+ base SDK (Xcode 8.3+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Jun 2017    | [3112](https://bitbucket.org/chromiumembedded/cef/src/3112?at=3112) | 60               | 3          | Win 7+, VS2015u3, Win10.0.14393 SDK, Ninja | macOS 10.9-10.12, 10.9+ deployment target, 10.9+ build system w/ 10.10+ base SDK (Xcode 8.3+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Apr 2017    | [3071](https://bitbucket.org/chromiumembedded/cef/src/3071?at=3071) | 59               | 3          | Win 7+, VS2015u3, Win10.0.14393 SDK, Ninja | macOS 10.9-10.12, 10.9+ deployment target, 10.9+ build system w/ 10.10+ base SDK (Xcode 8.3+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Jessie+, Ninja |
| Mar 2017    | [3029](https://bitbucket.org/chromiumembedded/cef/src/3029?at=3029) | 58               | 3          | Win 7+, VS2015u3, Win10.0.14393 SDK, Ninja | macOS 10.9-10.12, 10.9+ deployment target, 10.9+ build system w/ 10.10+ base SDK (Xcode 8.3+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Wheezy+, Ninja |
| Jan 2017    | [2987](https://bitbucket.org/chromiumembedded/cef/src/2987?at=2987) | 57               | 3          | Win 7+, VS2015u3, Win10.0.14393 SDK, Ninja | macOS 10.9-10.12, 10.9+ deployment target, 10.9+ build system w/ 10.10+ base SDK (Xcode 7.3.1+), 64-bit only | Ubuntu 14.04+, Debian Wheezy+, Ninja |
| Nov 2016    | [2924](https://bitbucket.org/chromiumembedded/cef/src/2924?at=2924) | 56               | 3          | Win 7+, VS2015u3, Win10.0.10586 SDK, Ninja | macOS 10.9-10.12, 10.9+ deployment target, 10.9+ build system w/ 10.10+ base SDK (Xcode 7.3.1+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Wheezy+, Ninja |
| Oct 2016    | [2883](https://bitbucket.org/chromiumembedded/cef/src/2883?at=2883) | 55               | 3          | Win 7+, VS2015u3, Win10.0.10586 SDK, Ninja | macOS 10.9-10.12, 10.7+ deployment target, 10.9+ build system w/ 10.10+ base SDK (Xcode 7.3.1+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Wheezy+, Ninja |
| Aug 2016    | [2840](https://bitbucket.org/chromiumembedded/cef/src/2840?at=2840) | 54               | 3          | Win 7+, VS2015u2 or VS2015u3, Win10.0.10586 SDK, Ninja | macOS 10.9-10.12, 10.7+ deployment target, 10.9+ build system w/ 10.10+ base SDK (Xcode 7.3.1+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Wheezy+, Ninja |
| Jul 2016    | [2785](https://bitbucket.org/chromiumembedded/cef/src/2785?at=2785) | 53               | 3          | Win 7+, VS2015u2 or VS2015u3, Win10.0.10586 SDK, Ninja | macOS 10.9-10.11, 10.7+ deployment target, 10.9+ build system w/ 10.10+ base SDK (Xcode 7.3.1+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Wheezy+, Ninja |
| May 2016    | [2743](https://bitbucket.org/chromiumembedded/cef/src/2743?at=2743) | 52               | 3           | Win 7+, VS2015u2 or VS2015u3, Win10.0.10586 SDK, Ninja | macOS 10.9-10.11, 10.7+ deployment target, 10.9+ build system w/ 10.10+ base SDK (Xcode 7.1.1+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Wheezy+, Ninja |
| Apr 2016    | [2704](https://bitbucket.org/chromiumembedded/cef/src/2704?at=2704) | 51               | 3           | Win 7+, VS2015u2, Win10.0.10586 SDK, Ninja | macOS 10.9-10.11, 10.7+ deployment target, 10.9+ build system w/ 10.10+ base SDK (Xcode 7.1.1+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Wheezy+, Ninja |
| Jan 2016    | [2623](https://bitbucket.org/chromiumembedded/cef/src/2623?at=2623) | 49               | 3           | WinXP+, VS2013u4 or VS2015u1 (experimental), Win10 SDK, Ninja | macOS 10.6-10.11, 10.6+ deployment target, 10.7+ build system w/ 10.10+ base SDK (Xcode 7.1.1+), Ninja, 64-bit only | Ubuntu 14.04+, Debian Wheezy+, Ninja |
| Oct 2015    | [2526](https://bitbucket.org/chromiumembedded/cef/src/2526?at=2526) | 47               | 3           | WinXP+, VS2013u4 or VS2015u1 (experimental), Win8.1 SDK, Ninja | macOS 10.6-10.11, 10.6+ deployment target, 10.10 base SDK, Xcode 6.1, Ninja, 64-bit only | Ubuntu 12.04+, Debian Wheezy+, Ninja |
| Jul 2015    | [2454](https://bitbucket.org/chromiumembedded/cef/src/2454?at=2454) | 45               | 3           | WinXP+, VS2013u4, Win8.1 SDK, Ninja | macOS 10.6-10.10, 10.6+ deployment target, 10.9 base SDK, Xcode 6.1, Ninja, 64-bit only | Ubuntu 12.04+, Debian Wheezy+, Ninja |
| Apr 2015    | [2357](https://bitbucket.org/chromiumembedded/cef/src/2357?at=2357) | 43               | 3           | WinXP+, VS2013u4, Win8.1 SDK, Ninja | macOS 10.6-10.10, 10.6+ deployment target, 10.9 base SDK, Xcode 6.1, Ninja, 64-bit only | Ubuntu 12.04+, Debian Wheezy+, Ninja |
| Jan 2015    | [2272](https://bitbucket.org/chromiumembedded/cef/src/2272?at=2272) | 41               | 3           | WinXP+, VS2013u4, Win8.1 SDK, Ninja | macOS 10.6-10.10, 10.6+ deployment target, 10.9 base SDK, Xcode 6.1, Ninja, 64-bit only | Ubuntu 12.04+, Debian Wheezy+, Ninja |
| Oct 2014    | [2171](https://bitbucket.org/chromiumembedded/cef/src/2171?at=2171) | 39               | 3           | WinXP+, VS2013u4, Win8.1 SDK, Ninja | macOS 10.6-10.9, 10.6+ SDK, Xcode 5.1.1, Ninja | Ubuntu 12.04+, Debian Wheezy+, Ninja |
| Aug 2014    | [2062](https://bitbucket.org/chromiumembedded/cef/src/2062?at=2062) | 37               | 3           | WinXP+, VS2013, Win8 SDK, Ninja | macOS 10.6-10.9, 10.6+ SDK, Xcode 5.1.1, Ninja | Ubuntu 12.04+, Debian Wheezy+, Ninja |
| Apr 2014    | [1916](https://bitbucket.org/chromiumembedded/cef/src/1916?at=1916) | 35               | 3           | WinXP+, VS2013, Win8 SDK, Ninja | macOS 10.6-10.9, 10.6+ SDK, Xcode 5.1.1, Ninja | Ubuntu 12.04+, Debian Wheezy+, Ninja |
| Jan 2014    | [1750](https://bitbucket.org/chromiumembedded/cef/src/1750?at=1750) | 33               | 3           | WinXP+, VS2010-2013, Win8 SDK, Ninja | macOS 10.6-10.9, 10.6+ SDK, Xcode 5.1.1, Ninja | Ubuntu 12.04+, Debian Wheezy+, Ninja |
| Oct 2013    | [1650](https://bitbucket.org/chromiumembedded/cef/src/1650?at=1650) | 31               | 3           | WinXP+, VS2010-2012, Win8 SDK, Ninja (optional) | macOS 10.6-10.9, 10.6+ SDK, Xcode 5.1.1, Ninja | Ubuntu 12.04+, Debian Wheezy+, Ninja |
| Jul 2013    | [1547](https://bitbucket.org/chromiumembedded/cef/src/1547?at=1547) | 29               | 3           | WinXP+, VS2010-2012, Win8 SDK, Ninja (optional) | macOS 10.6-10.8, 10.6+ SDK, Xcode 3.2.6-4.x, Ninja (optional) | Ubuntu 12.04+, Debian Squeeze+, Ninja |
| Apr 2013    | [1453](https://bitbucket.org/chromiumembedded/cef/src/1453?at=1453) | 27               | 1, 3        | WinXP+, VS2010, Win8 SDK, Ninja (optional) | macOS 10.6-10.8, 10.6+ SDK, Xcode 3.2.6-4.x, Ninja (optional) | Ubuntu 12.04+, Debian Squeeze+, Ninja (optional) |
| Jan 2013    | [1364](https://bitbucket.org/chromiumembedded/cef/src/1364?at=1364) | 25               | 1, 3        | WinXP+, VS2010, Win8 SDK, Ninja (optional) | macOS 10.6-10.8, Xcode 3.2.6-4.x, Ninja (optional) | Ubuntu 12.04+, Debian Squeeze+, Ninja (optional) |
| Oct 2012    | [1271](https://bitbucket.org/chromiumembedded/cef/src/1271?at=1271) | 23               | 1, 3        | WinXP+, VS2010, Win7 SDK   | macOS 10.6-10.8, 10.6+ SDK, Xcode 3.2.6-4.x | Ubuntu 12.04+, Debian Squeeze+ |
| Aug 2012    | [1180](https://bitbucket.org/chromiumembedded/cef/src/1180?at=1180) | 21               | 1, 3        | WinXP+, VS2010, Win7 SDK   | macOS 10.6-10.7, 10.5+ SDK, Xcode 3.2.6-4.x | Ubuntu 12.04+, Debian Squeeze+ |
| Apr 2012    | [1084](https://bitbucket.org/chromiumembedded/cef/src/1084?at=1084) | 19               | 1           | WinXP+, VS2008, Win7 SDK   | macOS 10.6-10.7, 10.5+ SDK, Xcode 3.2.6-4.x | Ubuntu 10.04+, Debian Squeeze+ |
| Feb 2012    | [1025](https://bitbucket.org/chromiumembedded/cef/src/1025?at=1025) | 18               | 1           | WinXP+, VS2008, Win7 SDK   | macOS 10.6-10.7, 10.5+ SDK, Xcode 3.2.6-4.x | Ubuntu 10.04+, Debian Squeeze+ |
| Dec 2011    | [963](https://bitbucket.org/chromiumembedded/cef/src/963?at=963) | 17               | 1           | WinXP+, VS2008, Win7 SDK   | macOS 10.6-10.7, 10.5+ SDK, Xcode 3.2.6 | Ubuntu 10.04+, Debian Squeeze+ |

The following URL should be used for downloading release versions of CEF where YYYY is the release branch number.

  * https://bitbucket.org/chromiumembedded/cef/src/YYYY?at=YYYY

Note that 1025 and older branches contain only CEF1 source code and that 1547 and newer branches contain only CEF3 source code.

# Building from Source

Building from source code is currently supported on Windows, macOS and Linux platforms. Use of the Automated Method described below is recommended. Building the current CEF/Chromium master branch for local development is described on the [MasterBuildQuickStart](https://bitbucket.org/chromiumembedded/cef/wiki/MasterBuildQuickStart.md) Wiki page. Building the current CEF/Chromium stable branch automatically for production use is described on the  [AutomatedBuildSetup](https://bitbucket.org/chromiumembedded/cef/wiki/AutomatedBuildSetup.md#markdown-header-linux-configuration) Wiki page. For other branches see the build requirements listed in the “Release Branches” section above and the “Build Notes” section below.

## Automated Method

CEF provides tools for automatically downloading, building and packaging Chromium and CEF source code. These tools are the recommended way of building CEF locally and can also be integrated with automated build systems as described on the [AutomatedBuildSetup](https://bitbucket.org/chromiumembedded/cef/wiki/AutomatedBuildSetup.md) Wiki page. See the [MasterBuildQuickStart](https://bitbucket.org/chromiumembedded/cef/wiki/MasterBuildQuickStart.md) Wiki page for an example of the recommended workflow for local development builds.

These steps apply to the Git workflow only. The Git workflow is recommended for all users and supports CEF3 master and newer CEF3 release branches (1750+).

1\. Download the [automate-git.py script](https://bitbucket.org/chromiumembedded/cef/raw/master/tools/automate/automate-git.py). Use the most recent master version of this script even when building release branches.

2\. On Linux: Chromium requires that certain packages be installed. You can install them by running the [install-build-deps.sh script](https://bitbucket.org/chromiumembedded/cef/wiki/MasterBuildQuickStart.md#markdown-header-linux-setup) or by [explicitly running](https://bitbucket.org/chromiumembedded/cef/wiki/AutomatedBuildSetup.md#markdown-header-linux-configuration) the necessary installation commands.

3\. Run the automate-git.py script at whatever interval is appropriate (for each CEF commit, once per day, once per week, etc).

To build master:

```
python /path/to/automate/automate-git.py --download-dir=/path/to/download
```

To build a release branch:

```
python /path/to/automate/automate-git.py --download-dir=/path/to/download --branch=2785
```

By default the script will download depot\_tools, Chromium and CEF source code, run Debug and Release builds of CEF, and create a binary distribution package containing the build artifacts in the “/path/to/download/chromium/src/cef/binary\_distrib” directory. Future runs of the script will perform the minimum work necessary (unless otherwise configured using command-line flags). For example, if there are no pending CEF or Chromium updates the script will do nothing.

If you run the script and CEF or Chromium updates are pending the “/path/to/download/chromium/src/cef” directory will be removed and replaced with a clean copy from “/path/to/download/cef\_(branch)” (specify the `--no-update` command-line flag to disable updates). Make sure to back up any changes that you made in the “/path/to/download/chromium/src/cef” directory before re-running the script.

The same download directory can be used for building multiple CEF branches (just specify a different `--branch` command-line value). The existing “/path/to/download/chromium/src/out” directory will be moved to “/path/to/download/out\_(previousbranch)” so that the build output from the previous branch is not lost. When you switch back to a previous branch the out directory will be restored to its original location.

The script will create a 32-bit build on Windows by default. To create a 64-bit build on Windows, macOS or Linux specify the `--x64-build` command-line flag. 32-bit builds on macOS are [no longer supported](https://groups.google.com/a/chromium.org/d/msg/chromium-dev/sdsDCkq_zwo/yep65H8Eg3sJ) starting with 2272 branch so this flag is now required when building 2272+ on that platform.

If you receive Git errors when moving an existing checkout from one branch to another you can force a clean Chromium Git checkout (specify the  `--force-clean` command-line flag) and optionally a clean download of Chromium dependencies (specify the `--force-clean-deps` command-line flag). Any build output that currently exists in the “src/out” directory will be deleted. Re-downloading the Chromium dependencies can take approximately 30 minutes with a reasonably fast internet connection.

Add the `--help` command-line switch to output a complete list of supported command-line options.

## Manual Downloading

See the [MasterBuildQuickStart](https://bitbucket.org/chromiumembedded/cef/wiki/MasterBuildQuickStart.md) Wiki page for an example of the recommended developer workflow.

## Manual Building

See the [MasterBuildQuickStart](https://bitbucket.org/chromiumembedded/cef/wiki/MasterBuildQuickStart.md) Wiki page for an example of the recommended developer workflow.

## Manual Packaging

After building both Debug and Release configurations you can use the make\_distrib tool (.bat on Windows, .sh on macOS and Linux) to create a binary distribution.

```
cd /path/to/chromium/src/cef/tools
./make_distrib.sh --ninja-build
```

If the process succeeds a binary distribution package will be created in the /path/to/chromium/src/cef/binary\_distrib directory.

See the [make\_distrib.py](https://bitbucket.org/chromiumembedded/cef/src/master/tools/make_distrib.py) script for additional usage options.

The resulting binary distribution can then be built using CMake and platform toolchains. See the README.txt file included with the binary distribution for more information.

## Build Notes

This section summarizes build-related requirements and options.

  * Building on most platforms will require at least 8GB of system memory.
  * [Ninja](https://ninja-build.org/) must be used when building newer CEF/Chromium branches.
  * [Clang](https://clang.llvm.org/) is used by default for compiling/linking Chromium/CEF on macOS in all branches, Linux starting in 2063 branch, and Windows starting in 3282 branch.
  * [GYP](https://gyp.gsrc.io/docs/UserDocumentation.md) is supported by 2785 branch and older. [GN](http://www.chromium.org/developers/gn-build-configuration) is supported by 2785 branch and newer, and required starting with 2840 branch. Set `CEF_USE_GN=1` to build 2785 branch with GN instead of GYP.
  * To perform a 64-bit build on Windows (any branch) or macOS (branch 2171 or older) set `GYP_DEFINES=target_arch=x64` (GYP only) or build the `out/[Debug|Release]_GN_x64` target (GN only). To perform a 32-bit Linux build on a 64-bit Linux system see instructions on the [AutomatedBuildSetup](https://bitbucket.org/chromiumembedded/cef/wiki/AutomatedBuildSetup.md#markdown-header-linux-configuration) Wiki page.
  * To perform an “official” build set `GYP_DEFINES=buildtype=Official` (GYP only) or `GN_DEFINES=is_official_build=true` (GN only). This will disable debugging code and enable additional link-time optimizations in Release builds. See instructions on the [AutomatedBuildSetup](https://bitbucket.org/chromiumembedded/cef/wiki/AutomatedBuildSetup.md) Wiki page for additional official build recommendations.
  * Windows -
    * If multiple versions of Visual Studio are installed on your system you can set the GYP\_MSVS\_VERSION environment variable to create project files for that version. For example, set the value to "2015" for VS2015 or "2017" for VS2017. Check the Chromium documentation for the correct value when using other Visual Studio versions.
    * If you wish to use Visual Studio for debugging and compiling in combination with a Ninja build you can set `GYP_GENERATORS=ninja,msvs-ninja` (GYP only) or `GN_ARGUMENTS=--ide=vs2017 --sln=cef --filters=//cef/*` (GN only) to generate both Ninja and VS project files. Visual Studio is supported only for debugging and compiling individual source files -- it will not build whole targets successfully. You must use Ninja when building CEF/Chromium targets.
    * For best local developer (non-official debug) build-time performance:
        * When using VS2015 set `GN_DEFINES=is_win_fastlink=true` for improved compile and link time (branches <= 3202).
        * When using VS2017 or VS2019 set `GN_DEFINES=use_jumbo_build=true` for improved compile and link time (branches <= 4044).
        * Component builds are supported by 3202 branch and newer and significantly reduce link time. Add `is_component_build=true` to GN_DEFINES in combination with the above VS-version-specific values. Component builds cannot be used to create a CEF binary distribution. See [issue #1617](https://github.com/chromiumembedded/cef/issues/1617#issuecomment-1464996498) for details.
  * macOS -
    * The combination of deployment target and base SDK version will determine the platforms supported by the resulting binaries. For proper functioning you must use the versions specified under build requirements for each branch.
    * 32-bit builds are no longer supported with 2272 branch and newer. See [here](https://groups.google.com/a/chromium.org/d/msg/chromium-dev/sdsDCkq_zwo/yep65H8Eg3sJ) for the Chromium announcement.
  * Linux -
    * CEF is developed and tested on the Ubuntu version specified in the "Release Branches" section. It should be possible to build and run CEF on other compatible Linux distributions but this is untested.
    * The libgtkglext1-dev package is required in branch 1547 and newer to support the off-screen rendering example in cefclient. This is only a requirement for cefclient and not a requirement for other applications using CEF.

This Wiki page describes sandbox usage and requirements for CEF.

**Note to Editors: Changes made to this Wiki page without prior approval via the [CEF Forum](http://magpcss.org/ceforum/) or [Issue Tracker](https://github.com/chromiumembedded/cef/issues) may be lost or reverted.**

***
[TOC]
***

# Overview

The Chromium/CEF [multi-process model](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md#markdown-header-processes) uses separate processes for the renderer, network stack, GPU, etc. This model allows Chromium to restrict access to system resources based on the sub-process type. For example, a renderer sub-process that executes JavaScript code does not need direct access to the file system or network stack. Allowing unnecessary access can be a security concern if a sub-process is compromised while executing content (e.g. JavaScript code) from a hostile website.

The [Chromium sandbox](https://chromium.googlesource.com/chromium/src/+/master/docs/design/sandbox.md) is used to apply per-process access restrictions based on the process type. With the sandbox enabled a compromised sub-process will often be terminated before it can do any harm. CEF supports the Chromium sandbox on each platform as described below.

# Usage

Sandbox capabilities are OS-specific and different requirements apply to each platform.

## Windows

To enable the sandbox on Windows the following requirements must be met:

1. Use the same executable for the browser process and all sub-processes. The `CefSettings.browser_subprocess_path` setting cannot be used in combination with the sandbox.
2. Link the executable with the `cef_sandbox` static library.
3. Call the `cef_sandbox_info_create()` function from within the executable (not from a separate DLL) and pass the resulting pointer into both the` CefExecutProcess()` and `CefInitialize()` functions via the `windows_sandbox_info` parameter.


See [cef_sandbox_win.h](https://bitbucket.org/chromiumembedded/cef/raw/master/include/cef_sandbox_win.h) for additional details. See [cefsimple_win.cc](https://bitbucket.org/chromiumembedded/cef/raw/master/tests/cefsimple/cefsimple_win.cc) for an example implementation.

## macOS

To enable the sandbox on macOS the following requirements must be met:

1. Link the helper process executable with the `cef_sandbox` static library.
2. Call the `cef_sandbox_initialize()` function at the beginning of the helper executable `main()` function and before loading the CEF framework library.

See [cef_sandbox_mac.h](https://bitbucket.org/chromiumembedded/cef/raw/master/include/cef_sandbox_mac.h) and [cef_library_loader.h](https://bitbucket.org/chromiumembedded/cef/raw/master/include/wrapper/cef_library_loader.h) for additional details. See [cefsimple_mac.mm](https://bitbucket.org/chromiumembedded/cef/raw/master/tests/cefsimple/cefsimple_mac.mm) and [process_helper_mac.cc](https://bitbucket.org/chromiumembedded/cef/raw/master/tests/cefsimple/process_helper_mac.cc) for an example implementation.

The [V2 sandbox](https://chromium.googlesource.com/chromium/src/+/master/sandbox/mac/seatbelt_sandbox_design.md) was enabled starting with 3538 branch. See [this announcement](https://groups.google.com/d/msg/cef-announce/Fith0A3kWtw/6ds_mJVMCQAJ) for a description of the related changes.

## Linux

Different approaches are used based on system capabilities. See [this document](https://chromium.googlesource.com/chromium/src/+/refs/tags/57.0.2956.2/docs/linux_sandboxing.md) for details.

# Building cef_sandbox (Windows/macOS)

Linking the `cef_sandbox` static library is required, as described above, for using the sandbox on Windows and macOS. This library must be built with special configuration settings in order to link successfully with third-party applications. For example, Chromium's custom bundled libc++ version must be disabled to avoid linker conflicts when using the default platform runtime library.

The `cef_sandbox` library build is usually performed by the `automate_git.py` tool as part of the official binary distribution build (see the [AutomatedBuildSetup](https://bitbucket.org/chromiumembedded/cef/wiki/AutomatedBuildSetup.md) Wiki page). The special sandbox-specific configuration comes from the `GetConfigArgsSandbox` function in the [gn_args.py](https://bitbucket.org/chromiumembedded/cef/raw/master/tools/gn_args.py) tool and the sandbox build is performed by `automate_git.py` in a separate `out\[Config]_sandbox` output directory. A final post-processing action by the [make_distrib.py](https://bitbucket.org/chromiumembedded/cef/raw/master/tools/make_distrib.py) tool combines multiple lib files into a single `cef_sandbox.lib` (Windows) or `cef_sandbox.a` (macOS) file which is included with the binary distribution.

The `cef_sandbox` library can also be built locally for development and testing purposes as described below. This example creates a build that is compatible with the official 32-bit Windows distribution available for download from [Spotify](https://cef-builds.spotifycdn.com/index.html). To perform this build you must first set up a local Chromium/CEF checkout as described on the [MasterBuildQuickStart](https://bitbucket.org/chromiumembedded/cef/wiki/MasterBuildQuickStart.md) Wiki page and check out the correct branch/version by passing the appropriate `--branch=XXXX` or `--checkout=HHHHHHHH` command-line flags to `automate_git.py`. You can then build the `cef_sandbox` library as follows:

```
# Discover what `args.gn` contents are required:
cd c:\path\to\chromium\src\cef
set GN_DEFINES=is_official_build=true
python3 tools\gn_args.py

# Create the `chromium\src\out\Debug_GN_x64_sandbox\args.gn` file with the specified contents.

# Build the cef_sandbox target in the new output directory:
cd c:\path\to\chromium\src
set DEPOT_TOOLS_WIN_TOOLCHAIN=0
gn gen out\Debug_GN_x64_sandbox
ninja -C out\Debug_GN_x64_sandbox cef_sandbox

# Create a binary distribution for the cef_sandbox build:
cd c:\path\to\chromium\src\cef\tools
make_distrib.bat --allow-partial --sandbox --ninja-build --x64-build --no-archive --no-symbols --no-docs
```

The resulting `chromium\src\cef\binary_distrib\cef_binary_*_windows64_sandbox\Debug\cef_sandbox.lib` file can then be tested in combination with an existing official binary distribution at the same version.

If a different platform or configuration is desired you can find the appropriate `GN_DEFINES` on the [AutomatedBuildSetup](https://bitbucket.org/chromiumembedded/cef/wiki/AutomatedBuildSetup.md) Wiki page.

This Wiki page describes how to build CEF on Debian 7 systems.

**Note to Editors: Changes made to this Wiki page without prior approval via the [CEF Forum](http://magpcss.org/ceforum/) or [Issue Tracker](https://github.com/chromiumembedded/cef/issues) may be lost or reverted.**

***

**Note: Developing Chromium/CEF on Debian 7 is not recommended. Chromium stopped supporting Debian 7 in [March 2017](https://bugs.chromium.org/p/chromium/issues/detail?id=701894). These instructions should be considered valid only for branch 3029 and older.**

CEF/Chromium is primarily developed on Ubuntu 14 LTS systems as described on the [BranchesAndBuilding](https://code.google.com/p/chromiumembedded/wiki/BranchesAndBuilding) Wiki page. To build CEF/Chromium on a Debian 7 system additional manual steps are required.

The below instructions are based on the CEF 2526 branch built on a Debian 7.8.0 64-bit Linux system. To build using this system:

1\. Install [Debian 7 64-bit](https://www.debian.org/distrib/). This can be done using dedicated hardware or a [VMware](http://www.vmware.com/products/player), [Parallels](http://www.parallels.com/eu/products/desktop/download/) or [VirtualBox](https://www.virtualbox.org/wiki/Downloads) virtual machine. At least 6GB of RAM and 40GB of disk space are required to successfully build Chromium/CEF and create a CEF binary distribution.

2\. Download the install-build-deps.sh script.

```
# Download the install-build-deps.sh script at the correct revision (initially base64 encoded).
wget -O install-build-deps.txt https://chromium.googlesource.com/chromium/src/+/fedbbb0aa762826ba2fc85a2b934dc9660639aa7/build/install-build-deps.sh?format=TEXT
# Decode the script.
python -m base64 -d install-build-deps.txt > install-build-deps.sh
# Mark the script as executable.
chmod 755 install-build-deps.sh
```

3\. Edit the install-build-deps.sh script so that it will run on Debian 7. This includes removing Ubuntu-specific logic and packages that are not available on Debian 7. For example:

```
--- install-build-deps.sh	2015-03-11 16:42:28.546982015 -0400
+++ install-build-deps.sh	2015-03-11 16:43:40.930979602 -0400
@@ -72,12 +72,6 @@
 lsb_release=$(lsb_release --codename --short)
 ubuntu_codenames="(precise|quantal|raring|saucy|trusty)"
 if [ 0 -eq "${do_unsupported-0}" ] && [ 0 -eq "${do_quick_check-0}" ] ; then
-  if [[ ! $lsb_release =~ $ubuntu_codenames ]]; then
-    echo "ERROR: Only Ubuntu 12.04 (precise) through 14.04 (trusty) are"\
-        "currently supported" >&2
-    exit 1
-  fi
-
   if ! uname -m | egrep -q "i686|x86_64"; then
     echo "Only x86 architectures are currently supported" >&2
     exit
@@ -91,12 +85,11 @@
 fi
 
 # Packages needed for chromeos only
-chromeos_dev_list="libbluetooth-dev libxkbcommon-dev"
+chromeos_dev_list=""
 
 # Packages needed for development
 dev_list="apache2.2-bin bison cdbs curl dpkg-dev elfutils devscripts fakeroot
-          flex fonts-thai-tlwg g++ git-core git-svn gperf language-pack-da
-          language-pack-fr language-pack-he language-pack-zh-hant
+          flex fonts-thai-tlwg g++ git-core git-svn gperf
           libapache2-mod-php5 libasound2-dev libbrlapi-dev libav-tools
           libbz2-dev libcairo2-dev libcap-dev libcups2-dev libcurl4-gnutls-dev
           libdrm-dev libelf-dev libexif-dev libgconf2-dev libgl1-mesa-dev
@@ -170,11 +163,6 @@
 nacl_list="${nacl_list} libgl1-mesa-glx${mesa_variant}:i386"
 
 # Some package names have changed over time
-if package_exists ttf-mscorefonts-installer; then
-  dev_list="${dev_list} ttf-mscorefonts-installer"
-else
-  dev_list="${dev_list} msttcorefonts"
-fi
 if package_exists libnspr4-dbg; then
   dbg_list="${dbg_list} libnspr4-dbg libnss3-dbg"
   lib_list="${lib_list} libnspr4 libnss3"
```

4\. Install the necessary packages (run as root).

```
# Run the script excluding unnecessary components.
./install-build-deps.sh --no-lib32 --no-arm --no-chromeos-fonts --no-nacl

# Install additional packages that will be required later on.
# cmake is required when building llvm/clang.
# libgtkglext1-dev is required when building the cefclient sample application.
apt-get install cmake libgtkglext1-dev 
```

5\. Download Chromium and CEF source code at the correct branch and without building (see the [BranchesAndBuilding](BranchesAndBuilding.md) Wiki page for complete CEF build instructions).

```
python automate-git.py --download-dir=/path/to/chromium_git --depot-tools-dir=/path/to/depot_tools --no-distrib --no-build --branch=2526
```

6\. Add depot\_tools to the PATH.

```
export PATH=/path/to/depot_tools:$PATH
```

7\. Build cmake version 2.8.12.2 or newer (required by newer llvm/clang builds).

```
# Download/build in whichever directory you prefer.
cd ~

wget http://www.cmake.org/files/v2.8/cmake-2.8.12.2.tar.gz
tar xzf cmake-2.8.12.2.tar.gz
cd cmake-2.8.12.2
./configure
./make

# Run the following command as root.
./make install
```

Verify that the cmake version is now correct by running `cmake --version`.

8\. Build llvm and clang locally. Debian 7 ships with an [older version of GLIBC](https://code.google.com/p/chromiumembedded/issues/detail?id=1575) than Ubuntu 14 so the binaries distributed with the Chromium checkout cannot be used. See comments in the update.sh script for additional llvm/clang build information.

```
cd /path/to/chromium_git/chromium/src
./tools/clang/scripts/update.sh --force-local-build --without-android --gcc-toolchain '/usr'
```

If you get build errors while compiling llvm/clang for 2526 branch or newer see [this thread](http://www.magpcss.org/ceforum/viewtopic.php?f=6&t=13330) for possible solutions.

9\. Build binutils locally for 2526 branch or newer. The binutils binaries included with these newer branches have the same GLIBC issue as llvm/clang.

A\. Edit the “src/third_party/binutils/build-one.sh” script and change all instances of “/build/output” to “$PWD/build/output”. This script is intended to build inside a chroot environment but we want to build directly on the Debian system instead.

B\. Download and build binutils (based on the contents of “src/third_party/binutils/build-all.sh”):

```
cd /path/to/chromium_git/chromium/src/third_party/binutils

# Download binutils at the version specified in build-all.sh.
wget http://ftp.gnu.org/gnu/binutils/binutils-2.25.tar.bz2
tar xjf binutils-2.25.tar.bz2

cd binutils-2.25

# Apply the Chromium patches specified in build-all.sh.
patch -p1 < ../unlock-thin.patch
patch -p1 < ../plugin-dso-fix.patch

cd ..

# Build binutils (don’t forget to apply the build-one.sh changes from A first!).
./build-one.sh binutils-2.25

# Copy the resulting files to the expected location (backing up the old files first).
mv Linux_x64/Release Linux_x64/Release_old
cp -a binutils-2.25/build/output/x86_64-unknown-linux-gnu Linux_x64/Release
mkdir Linux_x64/Release/include/
cp -a binutils-2.25/build/output/include/plugin-api.h Linux_x64/Release/include/
```

Verify that the resulting executables are correct by running `Linux_x64/Release/bin/objcopy`.

10\. Build Chromium/CEF and create the CEF binary distribution.

```
python automate-git.py --download-dir=/path/to/chromium_git --depot-tools-dir=/path/to/depot_tools --no-update --force-build --force-distrib --branch=2526
```

The Chromium Embedded Framework (CEF) is a simple framework for embedding Chromium-based browsers in other applications. Visit the [Project Overview page](https://bitbucket.org/chromiumembedded/cef/overview) for more information.

**Note to Editors: Changes made to this Wiki page without prior approval via the [CEF Forum](http://magpcss.org/ceforum/) or [Issue Tracker](https://github.com/chromiumembedded/cef/issues) may be lost or reverted.**

***

# Wiki Pages

* [Tutorial](Tutorial.md) - Tutorial explaining how to create a simple application using CEF3.
* [MasterBuildQuickStart](MasterBuildQuickStart.md) - Quick-start guide for building the CEF/Chromium master (development) branch.
* [BranchesAndBuilding](BranchesAndBuilding.md) - Information about CEF branches and instructions for downloading, building and packaging source code.
* [ContributingWithGit](ContributingWithGit.md) - How to contribute code changes to CEF using Git.
* [GeneralUsage](GeneralUsage.md) - Overview and general usage of CEF3.
* [JavaScriptIntegration](JavaScriptIntegration.md) - How to use V8 JavaScript integration in client applications.
* [Architecture](Architecture.md) - Overview of the CEF architecture.
* [UsingChromeDriver](UsingChromeDriver.md) - How to use ChromeDriver and Selenium to automate CEF-based applications.
* [UsingAddressSanitizer](UsingAddressSanitizer.md) - How to use AddressSanitizer with CEF-based applications.
* [UsingTheCAPI](UsingTheCAPI.md) - How to use the C API in your client application.
* [LinkingDifferentRunTimeLibraries](LinkingDifferentRunTimeLibraries.md) - How to link CEF against a different run-time library.
* [BuildingOnDebian7](BuildingOnDebian7.md) - How to build CEF on Debian 7 systems (these instructions are deprecated).
* [ChromiumUpdate](ChromiumUpdate.md) - How to update CEF to use the newest Chromium revision.
* [AutomatedBuildSetup](AutomatedBuildSetup.md) - How to set up an automated build system for CEF.
* [CrashReporting](CrashReporting.md) - How to handle crash reporting from CEF-based applications.
* [SandboxSetup](SandboxSetup.md) - Sandbox usage and requirements for CEF.

The complete list of CEF Wiki pages is available [here](https://bitbucket.org/chromiumembedded/cef/wiki/browse/).

# External Links

* Support Forum - http://www.magpcss.org/ceforum/
* C++ API Docs - [Stable release docs](https://cef-builds.spotifycdn.com/docs/stable.html) / [Beta release docs](https://cef-builds.spotifycdn.com/docs/beta.html)
* Downloads - https://cef-builds.spotifycdn.com/index.html
* Donations - http://www.magpcss.org/ceforum/donate.php

This Wiki page explains how to use V8 JavaScript integration in client applications.

**Note to Editors: Changes made to this Wiki page without prior approval via the [CEF Forum](http://magpcss.org/ceforum/) or [Issue Tracker](https://github.com/chromiumembedded/cef/issues) may be lost or reverted.**

***
[TOC]
***

# Introduction

Chromium and CEF use the [V8 JavaScript Engine](http://code.google.com/p/v8/) for their internal JavaScript (JS) implementation. Each frame in a browser has its own JS context that provides scope and security for JS code executing in that frame (see the "Working with Contexts" section for more information). CEF exposes a large number of JS features for integration in client applications.

With CEF3 Blink (WebKit) and JS execution run in a separate renderer process. The main thread in a renderer process is identified as TID\_RENDERER and all V8 execution must take place on this thread. Callbacks related to JS execution are exposed via the CefRenderProcessHandler interface. This interface is retrieved via CefApp::GetRenderProcessHandler() when a new renderer process is initialized.

JS APIs that communicate between the browser and renderer processes should be designed using asynchronous callbacks. See the "Asynchronous JavaScript Bindings" section of the [GeneralUsage](GeneralUsage.md) wiki page for more information.

# ExecuteJavaScript

The simplest way to execute JS from a client application is using the CefFrame::ExecuteJavaScript() function. This function is available in both the browser process and the renderer process and can safely be used from outside of a JS context.

```
CefRefPtr<CefBrowser> browser = ...;
CefRefPtr<CefFrame> frame = browser->GetMainFrame();
frame->ExecuteJavaScript("alert('ExecuteJavaScript works!');",
    frame->GetURL(), 0);
```

The above example will cause `alert('ExecuteJavaScript works!');` to be executed in the browser's main frame.

The ExecuteJavaScript() function can be used to interact with functions and variables in the frame's JS context. In order to return values from JS to the client application consider using Window Binding or Extensions.


# Window Binding

Window binding allows the client application to attach values to a frame's `window` object. Window bindings are implemented using the CefRenderProcessHandler::OnContextCreated() method.

```
void MyRenderProcessHandler::OnContextCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
  // Retrieve the context's window object.
  CefRefPtr<CefV8Value> object = context->GetGlobal();

  // Create a new V8 string value. See the "Basic JS Types" section below.
  CefRefPtr<CefV8Value> str = CefV8Value::CreateString("My Value!");

  // Add the string to the window object as "window.myval". See the "JS Objects" section below.
  object->SetValue("myval", str, V8_PROPERTY_ATTRIBUTE_NONE);
}
```

JavaScript in the frame can then interact with the window bindings.

```
<script language="JavaScript">
alert(window.myval); // Shows an alert box with "My Value!"
</script>
```

Window bindings are reloaded each time a frame is reloaded giving the client application an opportunity to change the bindings if necessary. For example, different frames may be given access to different features in the client application by modifying the values that are bound to the window object for that frame.


# Extensions

Extensions are like window bindings except they are loaded into the context for every frame and cannot be modified once loaded. The DOM does not exist when an extension is loaded and attempts to access the DOM during extension loading will result in a crash. Extensions are registered using the CefRegisterExtension() function which should be called from the CefRenderProcessHandler::OnWebKitInitialized() method.

```
void MyRenderProcessHandler::OnWebKitInitialized() {
  // Define the extension contents.
  std::string extensionCode =
    "var test;"
    "if (!test)"
    "  test = {};"
    "(function() {"
    "  test.myval = 'My Value!';"
    "})();";

  // Register the extension.
  CefRegisterExtension("v8/test", extensionCode, NULL);
}
```

The string represented by `extensionCode` can be any valid JS code. JS in the frame can then interact with the extension code.

```
<script language="JavaScript">
alert(test.myval); // Shows an alert box with "My Value!"
</script>
```

# Basic JS Types

CEF supports the creation of basic JS data types including undefined, null, bool, int, double, date and string. These types are created using the CefV8Value::Create\*() static methods. For example, to create a new JS string value use the CreateString() method.

```
CefRefPtr<CefV8Value> str = CefV8Value::CreateString("My Value!");
```

Basic value types can be created at any time and are not initially associated with a particular context (see the "Working with Contexts" section for more information).

To test the value type use the Is\*() methods.

```
CefRefPtr<CefV8Value> val = ...;
if (val.IsString()) {
  // The value is a string.
}
```

To retrieve the value use the Get\*Value() methods.

```
CefString strVal = val.GetStringValue();
```

# JS Arrays

Arrays are created using the CefV8Value::CreateArray() static method which accepts a length argument. Arrays can only be created and used from within a context (see the "Working with Contexts" section for more information).

```
// Create an array that can contain two values.
CefRefPtr<CefV8Value> arr = CefV8Value::CreateArray(2);
```

Values are assigned to an array using the SetValue() method variant that takes an index as the first argument.

```
// Add two values to the array.
arr->SetValue(0, CefV8Value::CreateString("My First String!"));
arr->SetValue(1, CefV8Value::CreateString("My Second String!"));
```

To test if a CefV8Value is an array use the IsArray() method. To get the length of an array use the GetArrayLength() method. To get a value from an array use the GetValue() variant that takes an index as the first argument.

# JS Objects

Objects are created using the CefV8Value::CreateObject() static method that takes an optional CefV8Accessor argument. Objects can only be created and used from within a context (see the "Working with Contexts" section for more information).

```
CefRefPtr<CefV8Value> obj = CefV8Value::CreateObject(NULL);
```

Values are assigned to an object using the SetValue() method variant that takes a key string as the first argument.

```
obj->SetValue("myval", CefV8Value::CreateString("My String!"));
```

## Objects with Accessors

Objects can optionally have an associated CefV8Accessor that provides a native implementation for getting and setting values.

```
CefRefPtr<CefV8Accessor> accessor = …;
CefRefPtr<CefV8Value> obj = CefV8Value::CreateObject(accessor);
```

An implementation of the CefV8Accessor interface that must be provided by the client application.

```
class MyV8Accessor : public CefV8Accessor {
public:
  MyV8Accessor() {}

  virtual bool Get(const CefString& name,
                   const CefRefPtr<CefV8Value> object,
                   CefRefPtr<CefV8Value>& retval,
                   CefString& exception) OVERRIDE {
    if (name == "myval") {
      // Return the value.
      retval = CefV8Value::CreateString(myval_);
      return true;
    }

    // Value does not exist.
    return false;
  }

  virtual bool Set(const CefString& name,
                   const CefRefPtr<CefV8Value> object,
                   const CefRefPtr<CefV8Value> value,
                   CefString& exception) OVERRIDE {
    if (name == "myval") {
      if (value->IsString()) {
        // Store the value.
        myval_ = value->GetStringValue();
      } else {
        // Throw an exception.
        exception = "Invalid value type";
      }
      return true;
    }

    // Value does not exist.
    return false;
  }

  // Variable used for storing the value.
  CefString myval_;

  // Provide the reference counting implementation for this class.
  IMPLEMENT_REFCOUNTING(MyV8Accessor);
};
```

In order for a value to be passed to the accessor it must be set using the SetValue() method variant that accepts AccessControl and PropertyAttribute arguments.

```
obj->SetValue("myval", V8_ACCESS_CONTROL_DEFAULT, 
    V8_PROPERTY_ATTRIBUTE_NONE);
```

# JS Functions

CEF supports the creation of JS functions with native implementations. Functions are created using the CefV8Value::CreateFunction() static method that accepts name and CefV8Handler arguments. Functions can only be created and used from within a context (see the "Working with Contexts" section for more information).

```
CefRefPtr<CefV8Handler> handler = …;
CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("myfunc", handler);
```

An implementation of the CefV8Handler interface that must be provided by the client application.

```
class MyV8Handler : public CefV8Handler {
public:
  MyV8Handler() {}

  virtual bool Execute(const CefString& name,
                       CefRefPtr<CefV8Value> object,
                       const CefV8ValueList& arguments,
                       CefRefPtr<CefV8Value>& retval,
                       CefString& exception) OVERRIDE {
    if (name == "myfunc") {
      // Return my string value.
      retval = CefV8Value::CreateString("My Value!");
      return true;
    }

    // Function does not exist.
    return false;
  }

  // Provide the reference counting implementation for this class.
  IMPLEMENT_REFCOUNTING(MyV8Handler);
};
```

## Functions and Window Binding

Functions can be used to create complex window bindings.

```
void MyRenderProcessHandler::OnContextCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
  // Retrieve the context's window object.
  CefRefPtr<CefV8Value> object = context->GetGlobal();

  // Create an instance of my CefV8Handler object.
  CefRefPtr<CefV8Handler> handler = new MyV8Handler();

  // Create the "myfunc" function.
  CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("myfunc", handler);

  // Add the "myfunc" function to the "window" object.
  object->SetValue("myfunc", func, V8_PROPERTY_ATTRIBUTE_NONE);
}
```

```
<script language="JavaScript">
alert(window.myfunc()); // Shows an alert box with "My Value!"
</script>
```

## Functions and Extensions

Functions can be used to create complex extensions. Note the use of the "native function" forward declaration which is required when using extensions.

```
void MyRenderProcessHandler::OnWebKitInitialized() {
  // Define the extension contents.
  std::string extensionCode =
    "var test;"
    "if (!test)"
    "  test = {};"
    "(function() {"
    "  test.myfunc = function() {"
    "    native function myfunc();"
    "    return myfunc();"
    "  };"
    "})();";

  // Create an instance of my CefV8Handler object.
  CefRefPtr<CefV8Handler> handler = new MyV8Handler();

  // Register the extension.
  CefRegisterExtension("v8/test", extensionCode, handler);
}
```

```
<script language="JavaScript">
alert(test.myfunc()); // Shows an alert box with "My Value!"
</script>
```


# Working with Contexts

Each frame in a browser window has its own V8 context. The context defines the scope for all variables, objects and functions defined in that frame. V8 will be inside a context if the current code location has a CefV8Handler, CefV8Accessor or OnContextCreated()/OnContextReleased() callback higher in the call stack.

The OnContextCreated() and OnContextReleased() methods define the complete life span for the V8 context associated with a frame. You should be careful to follow the below rules when using these methods:

1. Do not hold onto or use a V8 context reference past the call to OnContextReleased() for that context.

2. The lifespan of all V8 objects is unspecified (up to the GC). Be careful when maintaining references directly from V8 objects to your own internal implementation objects. In many cases it may be better to use a proxy object that your application associates with the V8 context and which can be "disconnected" (allowing your internal implementation object to be freed) when OnContextReleased() is called for the context.

If V8 is not currently inside a context, or if you need to retrieve and store a reference to a context, you can use one of two available CefV8Context static methods. GetCurrentContext() returns the context for the frame that is currently executing JS. GetEnteredContext() returns the context for the frame where JS execution began. For example, if a function in frame1 calls a function in frame2 then the current context will be frame2 and the entered context will be frame1.

Arrays, objects and functions may only be created, modified and, in the case of functions, executed, if V8 is inside a context. If V8 is not inside a context then the application needs to enter a context by calling Enter() and exit the context by calling Exit(). The Enter() and Exit() methods should only be used:

1. When creating a V8 object, function or array outside of an existing context. For example, when creating a JS object in response to a native menu callback.

2. When creating a V8 object, function or array in a context other than the current context. For example, if a call originating from frame1 needs to modify the context of frame2.

# Executing Functions

Native code can execute JS functions by using the ExecuteFunction() and ExecuteFunctionWithContext() methods. The ExecuteFunction() method should only be used if V8 is already inside a context as described in the "Working with Contexts" section. The ExecuteFunctionWithContext() method allows the application to specify the context that will be entered for execution.

## Using JS Callbacks

When registering a JS function callback with native code the application should store a reference to both the current context and the JS function in the native code. This could be implemented as follows.

1\. Create a "register" function in OnJSBinding().

```
void MyRenderProcessHandler::OnContextCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
  // Retrieve the context's window object.
  CefRefPtr<CefV8Value> object = context->GetGlobal();

  CefRefPtr<CefV8Handler> handler = new MyV8Handler(this);
  object->SetValue("register",
                   CefV8Value::CreateFunction("register", handler),
                   V8_PROPERTY_ATTRIBUTE_NONE);
}
```

2\. In the MyV8Handler::Execute() implementation for the "register" function keep a reference to both the context and the function.

```
bool MyV8Handler::Execute(const CefString& name,
                          CefRefPtr<CefV8Value> object,
                          const CefV8ValueList& arguments,
                          CefRefPtr<CefV8Value>& retval,
                          CefString& exception) {
  if (name == "register") {
    if (arguments.size() == 1 && arguments[0]->IsFunction()) {
      callback_func_ = arguments[0];
      callback_context_ = CefV8Context::GetCurrentContext();
      return true;
    }
  }

  return false;
}
```

3\. Register the JS callback via JavaScript.

```
<script language="JavaScript">
function myFunc() {
  // do something in JS.
}
window.register(myFunc);
</script>
```

4\. Execute the JS callback at some later time.

```
CefV8ValueList args;
CefRefPtr<CefV8Value> retval;
CefRefPtr<CefV8Exception> exception;
if (callback_func_->ExecuteFunctionWithContext(callback_context_, NULL, args, retval, exception, false)) {
  if (exception.get()) {
    // Execution threw an exception.
  } else {
    // Execution succeeded.
  }
}
```

See the [Asynchronous JavaScript Bindings](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-asynchronous-javascript-bindings) section of the GeneralUsage wiki page for more information on using callbacks.

## Rethrowing Exceptions

If CefV8Value::SetRethrowExceptions(true) is called before CefV8Value::ExecuteFunction\*() then any exceptions generated by V8 during function execution will be immediately rethrown. If an exception is rethrown any native code needs to immediately return. Exceptions should only be rethrown if there is a JS call higher in the call stack. For example, consider the following call stacks where "JS" is a JS function and "EF" is a native ExecuteFunction call:

Stack 1: JS1 -> EF1 -> JS2 -> EF2

Stack 2: Native Menu -> EF1 -> JS2 -> EF2

With stack 1 rethrow should be true for both EF1 and EF2. With stack 2 rethrow should false for EF1 and true for EF2.

This can be implemented by having two varieties of call sites for EF in the native code:

1. Only called from a V8 handler. This covers EF 1 and EF2 in stack 1 and EF2 in stack 2. Rethrow is always true.

2. Only called natively. This covers EF1 in stack 2. Rethrow is always false.

Be very careful when rethrowing exceptions. Incorrect usage (for example, calling ExecuteFunction() immediately after exception has been rethrown) may cause your application to crash or malfunction in hard to debug ways.

This Wiki page explains how to link CEF against a different run-time library.

**Note to Editors: Changes made to this Wiki page without prior approval via the [CEF Forum](http://magpcss.org/ceforum/) or [Issue Tracker](https://github.com/chromiumembedded/cef/issues) may be lost or reverted.**

***
# Introduction

Visual Studio supports multiple run-time libraries. The different libraries are identified by flags such as `/MD`, `/MT` and `/LD`. By default, CEF uses the `/MT` flag that is also used by the Chromium project. Different applications, however, can sometimes require different run-time libraries.

# Details

There are two ways currently to link CEF with your application.

## Static Linking (without a CEF DLL)

If you wish to link CEF with your application statically then all Chromium and CEF projects must use the same run-time flag as your application.  If your application already uses the `/MT` flag then you can build CEF statically as follows:

  1. Set up the environment required to build Chromium and CEF as described on the project page and build it.
  1. Link your application against the resulting libcef\_static.lib file.

If, however, your application does not use the `/MT` flag then you're effectively out of luck. Important parts of Chromium will not compile with a flag other than `/MT`.

## Dynamic Linking (with a CEF DLL)

If you prefer to link CEF with your application dynamically then the process is faster. CEF provides a binary distribution on the downloads page that contains everything you need to link CEF with your application including source code for the libcef\_dll\_wrapper project. If your application does not use the `/MT` flag then you will need to rebuild the libcef\_dll\_wrapper project with the same flags as your application. Unlike with static linking, you will not need to rebuild all of CEF or Chromium in order to do this.

  1. Download a CEF binary release from the project downloads page.
  1. Run CMake with the additional `-DCEF_RUNTIME_LIBRARY_FLAG=/MD` command-line flag (`/MD` can be replaced with other flags as appropriate).
  1. Open cef.sln in Visual Studio.
  1. Right click on the libcef\_dll\_wrapper project and choose the "Project Only -> Build Only libcef\_dll\_wrapper" option.

Sandbox support (linking `cef_sandbox.lib`) is only possible when your application is built with the `/MT` flag. To disable sandbox usage run CMake with the additional `-DUSE_SANDBOX=Off` command-line flag.

# Additional Information

More information on Microsoft run-time flags can be found here:
http://msdn.microsoft.com/en-us/library/2kzt1wy3.aspx

This Wiki page explains how to use ChromeDriver and Selenium to automate CEF-based applications.

**Note to Editors: Changes made to this Wiki page without prior approval via the [CEF Forum](http://magpcss.org/ceforum/) or [Issue Tracker](https://github.com/chromiumembedded/cef/issues) may be lost or reverted.**

***

# Overview

ChromeDriver and Selenium are tools for automated testing of Chromium-based applications. The tests themselves can be written in a number of languages including Java, JavaScript and Python. ChromeDriver communicates with the Chromium-based application using the DevTools remote debugging protocol (configured via the `--remote-debugging-port=XXXX` command-line flag passed to the application). Detailed ChromeDriver/Selenium usage information is available here:

  * https://sites.google.com/a/chromium.org/chromedriver/getting-started
  * https://code.google.com/p/selenium/wiki/GettingStarted

# Java Tutorial

This tutorial demonstrates how to control the cefclient sample application using the Java programming language on Windows. Usage on other platforms and with other CEF-based applications is substantially similar.

1\. Install the Java JDK and add its bin directory to your system PATH variable.

2\. Create a directory that will contain all files. This tutorial uses `c:\temp`.

3\. Download ChromeDriver from https://sites.google.com/a/chromium.org/chromedriver/downloads and extract (e.g. `chromedriver_win32.zip` provides `chomedriver.exe`). This tutorial was tested with version 2.14.

4\. Download `selenium-server-standalone-x.y.z.jar` from http://selenium-release.storage.googleapis.com/index.html. This tutorial was tested with version 2.44.0.

5\. Download a CEF binary distribution client from [Downloads](http://magpcss.net/cef_downloads/) and extract (e.g. `cef_binary_3.2171.1979_windows32_client.7z`).

6\. Create `Example.java`:

```
import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import org.openqa.selenium.chrome.ChromeDriver;
import org.openqa.selenium.chrome.ChromeOptions;

public class Example  {
  public static void main(String[] args) {
    // Path to the ChromeDriver executable.
    System.setProperty("webdriver.chrome.driver", "c:/temp/chromedriver.exe");

    ChromeOptions options = new ChromeOptions();
    // Path to the CEF executable.
    options.setBinary("c:/temp/cef_binary_3.2171.1979_windows32_client/Release/cefclient.exe");
    // Port to communicate on. Required starting with ChromeDriver v2.41.
    options.addArguments("remote-debugging-port=12345");

    WebDriver driver = new ChromeDriver(options);
    driver.get("http://www.google.com/xhtml");
    sleep(3000);  // Let the user actually see something!
    WebElement searchBox = driver.findElement(By.name("q"));
    searchBox.sendKeys("ChromeDriver");
    searchBox.submit();
    sleep(3000);  // Let the user actually see something!
    driver.quit();
  }

  static void sleep(int time) {
    try { Thread.sleep(time); } catch (InterruptedException e) {}
  }
}
```

Your directory structure should now look similar to this:
```
c:\temp\
  cef_binary_3.2171.1979_windows32_client\
    Release\
      cefclient.exe  (and other files)
  chromedriver.exe
  Example.java
  selenium-server-standalone-2.44.0.jar
```

7\. Compile `Example.java` to generate `Example.class`:

```
> cd c:\temp
> javac -cp ".;*" Example.java
```

8\. Run the example:

```
> cd c:\temp
> java -cp ".;*" Example
```

When the example is run ChromeDriver will:

  * Output to the console:
```
Starting ChromeDriver 2.14.313457 (3d645c400edf2e2c500566c9aa096063e707c9cf) on port 28110
Only local connections are allowed.
```
  * Launch `cefclient.exe`.
  * Submit a Google search for "ChromeDriver".
  * Close `cefclient.exe`.

This Wiki page provides a quick-start guide for creating a Debug build of CEF/Chromium using the current master (development) branch.

**Note to Editors: Changes made to this Wiki page without prior approval via the [CEF Forum](http://magpcss.org/ceforum/) or [Issue Tracker](https://github.com/chromiumembedded/cef/issues) may be lost or reverted.**

***
[TOC]
***

# Overview

This page provides a quick-start guide for setting up a minimal development environment and building the master branch of Chromium/CEF for development purposes. For a comprehensive discussion of the available tools and configurations visit the [BranchesAndBuilding](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md) Wiki page.

This guide is NOT intended for:

- Those seeking a prebuilt binary distribution for use in third-party apps. Go [here](https://cef-builds.spotifycdn.com/index.html) instead.
- Those seeking to build the binary distribution in a completely automated manner. Go [here](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md#markdown-header-automated-method) instead.

Development systems can be configured using dedicated hardware or a [VMware](http://www.vmware.com/products/player), [Parallels](http://www.parallels.com/eu/products/desktop/download/) or [VirtualBox](https://www.virtualbox.org/wiki/Downloads) virtual machine.

The below steps can often be used to develop the most recent release branch of CEF/Chromium in addition to the master branch. Chromium build requirements change over time so review the build requirements listed on the [BranchesAndBuilding](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md#markdown-header-release-branches) Wiki page before attempting to build a release branch. Then just add `--branch=XXXX` to the automate-git.py command-line where "XXXX" is the branch number you wish to build.

# File Structure

The same file structure will be used on all platforms. "~" can be any path that does not include spaces or special characters. We'll be building this directory structure for each platform in the following sections.

```
~/code/
  automate/
    automate-git.py   <-- CEF build script
  chromium_git/
    cef/              <-- CEF source checkout
    chromium/
      src/            <-- Chromium source checkout
    update.[bat|sh]   <-- Bootstrap script for automate-git.py
  depot_tools/        <-- Chromium build tools
```

With this file structure you can develop multiple CEF/Chromium branches side-by-side. For example, repeat the below instructions using "chromium_git1" as the directory name instead of "chromium_git".

# Windows Setup

**What's Required**

- Windows Build Requirements as listed on the [BranchesAndBuilding](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md#markdown-header-development) Wiki page.
- Install required Visual Studio sub-components as described [here](https://bitbucket.org/chromiumembedded/cef/wiki/AutomatedBuildSetup#markdown-header-windows-configuration).
- Install the exact Windows SDK version specified in the default location to avoid build issues.
- At least 16GB of RAM (32GB+ recommended) and 150GB of free disk space (for a Debug build).
- Approximately 4 hours with a fast internet connection (100Mbps) and fast build machine (2.4Ghz, 16 logical cores, SSD).

**Step-by-step Guide**

All of the below commands should be run using the system "cmd.exe" and not a Cygwin shell.

1\. Create the following directories.

```
c:\code\automate
c:\code\chromium_git
```

WARNING: If you change the above directory names/locations make sure to (a) use only ASCII characters and (b) choose a short file path (less than 35 characters total). Otherwise, some tooling may fail later in the build process due to invalid or overly long file paths.

2\. Download [depot_tools.zip](https://storage.googleapis.com/chrome-infra/depot_tools.zip) and extract to "c:\code\depot_tools". Do not use drag-n-drop or copy-n-paste extract from Explorer, this will not extract the hidden ".git" folder which is necessary for depot_tools to auto-update itself. You can use "Extract all..." from the context menu though. [7-zip](http://www.7-zip.org/download.html) is also a good tool for this.

3\. Run "update_depot_tools.bat" to install Python and Git.

```
cd c:\code\depot_tools
update_depot_tools.bat
```

4\. Add the "c:\code\depot_tools" folder to your system PATH. For example, on Windows 10:

- Run the "SystemPropertiesAdvanced" command.
- Click the "Environment Variables..." button.
- Double-click on "Path" under "System variables" to edit the value.

5\. Download the [automate-git.py](https://bitbucket.org/chromiumembedded/cef/raw/master/tools/automate/automate-git.py) script to "c:\code\automate\automate-git.py".

6\. Create the "c:\code\chromium_git\update.bat" script with the following contents.

```
set GN_DEFINES=is_component_build=true
set GN_ARGUMENTS=--ide=vs2022 --sln=cef --filters=//cef/*
python3 ..\automate\automate-git.py --download-dir=c:\code\chromium_git --depot-tools-dir=c:\code\depot_tools --no-distrib --no-build
```

Run the "update.bat" script and wait for CEF and Chromium source code to download. CEF source code will be downloaded to "c:\code\chromium_git\cef" and Chromium source code will be downloaded to "c:\code\chromium_git\chromium\src". After download completion the CEF source code will be copied to "c:\code\chromium_git\chromium\src\cef".

```
cd c:\code\chromium_git
update.bat
```

7\. Create the "c:\code\chromium_git\chromium\src\cef\create.bat" script with the following contents.

```
set GN_DEFINES=is_component_build=true
set GN_ARGUMENTS=--ide=vs2022 --sln=cef --filters=//cef/*
call cef_create_projects.bat
```

Run the "create.bat" script to generate Ninja and Visual Studio project files.

```
cd c:\code\chromium_git\chromium\src\cef
create.bat
```

This will generate a "c:\code\chromium_git\chromium\src\out\Debug_GN_x86\cef.sln" file that can be loaded in Visual Studio for debugging and compiling individual files. Replace “x86” with “x64” in this path to work with the 64-bit build instead of the 32-bit build. Always use Ninja to build the complete project. Repeat this step if you change the project configuration or add/remove files in the GN configuration (BUILD.gn file).

8\. Create a Debug build of CEF/Chromium using Ninja. Edit the CEF source code at "c:\code\chromium_git\chromium\src\cef" and repeat this step multiple times to perform incremental builds while developing.

```
cd c:\code\chromium_git\chromium\src
autoninja -C out\Debug_GN_x86 cef
```

Replace "Debug" with "Release" to generate a Release build instead of a Debug build. Replace “x86” with “x64” to generate a 64-bit build instead of a 32-bit build.

9\. Run the resulting cefclient sample application.

```
cd c:\code\chromium_git\chromium\src
out\Debug_GN_x86\cefclient.exe
```

See the [Windows debugging guide](https://www.chromium.org/developers/how-tos/debugging-on-windows) for detailed debugging instructions.

# Mac OS X Setup

**What's Required**

- macOS Build Requirements as listed on the [BranchesAndBuilding](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md#markdown-header-development) Wiki page.
- Building on an Intel Mac is supported with all versions. Building on an Apple Silicon (ARM64) Mac is supported starting with M93 (4577 branch).
- At least 16GB of RAM (32GB+ recommended) and 150GB of free disk space (for a Debug build).
- Approximately 4 hours with a fast internet connection (100Mbps) and fast build machine (2.4Ghz, 16 logical cores, SSD).

**Step-by-step Guide**

In this example "~" is "/Users/marshall". Note that in some cases the absolute path must be used. Environment variables described in this section can be added to your "~/.bash_profile" file to persist them across sessions.

1\. Create the following directories.

```
mkdir ~/code
mkdir ~/code/automate
mkdir ~/code/chromium_git
```

2\. Download "~/code/depot_tools" using Git.

```
cd ~/code
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
```

3\. Add the "~/code/depot_tools" directory to your PATH. Note the use of an absolute path here.

```
export PATH=/Users/marshall/code/depot_tools:$PATH
```

4\. Download the [automate-git.py](https://bitbucket.org/chromiumembedded/cef/raw/master/tools/automate/automate-git.py) script to "~/code/automate/automate-git.py".

5\. Create the "~/code/chromium_git/update.sh" script with the following contents.

```
#!/bin/bash
python3 ../automate/automate-git.py --download-dir=/Users/marshall/code/chromium_git --depot-tools-dir=/Users/marshall/code/depot_tools --no-distrib --no-build --x64-build
```

**⚠** Replace `--x64-build` with `--arm64-build` if using an Apple Silicon Mac instead of an Intel Mac.

Give it executable permissions.

```
cd ~/code/chromium_git
chmod 755 update.sh
```

Run the "update.sh" script and wait for CEF and Chromium source code to download. CEF source code will be downloaded to "~/code/chromium_git/cef" and Chromium source code will be downloaded to "~/code/chromium_git/chromium/src". After download completion the CEF source code will be copied to "~/code/chromium_git/chromium/src/cef".

```
cd ~/code/chromium_git
./update.sh
```

6\. Run the "~/code/chromium_git/chromium/src/cef/cef_create_projects.sh" script to create Ninja project files. Repeat this step if you change the project configuration or add/remove files in the GN configuration (BUILD.gn file).

```
cd ~/code/chromium_git/chromium/src/cef
./cef_create_projects.sh
```

**⚠** Add `export GN_DEFINES=is_component_build=true` before running `cef_create_projects.sh` if using an Apple Silicon Mac instead of an Intel Mac.

7\. Create a Debug build of CEF/Chromium using Ninja. Edit the CEF source code at "~/code/chromium_git/chromium/src/cef" and repeat this step multiple times to perform incremental builds while developing.

```
cd ~/code/chromium_git/chromium/src
autoninja -C out/Debug_GN_x64 cef
```

**⚠** Replace "x64" with "arm64" if using an Apple Silicon Mac instead of an Intel Mac. Replace "Debug" with "Release" to generate a Release build instead of a Debug build.

8\. Run the resulting cefclient, cefsimple and/or ceftests sample applications.

```
cd ~/code/chromium_git/chromium/src
open out/Debug_GN_x64/cefclient.app
# Or run directly in the console to see log output:
./out/Debug_GN_x64/cefclient.app/Contents/MacOS/cefclient
```

See the [Mac OS X debugging guide](https://www.chromium.org/developers/how-tos/debugging-on-os-x) for detailed debugging instructions.

# Linux Setup

**What's Required**

- Linux Build Requirements as listed on the [BranchesAndBuilding](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md#markdown-header-development) Wiki page.
- Building with other versions or distros has not been tested and may experience issues.
- At least 16GB of RAM (32GB+ recommended) and 120GB of free disk space (for a Debug build).
- Approximately 4 hours with a fast internet connection (100Mbps) and fast build machine (2.4Ghz, 16 logical cores, SSD).

**Step-by-step Guide**

In this example "~" is "/home/marshall". Note that in some cases the absolute path must be used. Environment variables described in this section can be added to your "~/.profile" or "~/.bashrc" file to persist them across sessions.

1\. Create the following directories.

```
mkdir ~/code
mkdir ~/code/automate
mkdir ~/code/chromium_git
```

2\. Download and run "~/code/install-build-deps.py" to install build dependencies. Answer Y (yes) to all of the questions.

```
cd ~/code
sudo apt-get install curl file lsb-release procps python3 python3-pip
curl 'https://chromium.googlesource.com/chromium/src/+/main/build/install-build-deps.py?format=TEXT' | base64 -d > install-build-deps.py
sudo python3 ./install-build-deps.py --no-arm --no-chromeos-fonts --no-nacl
python3 -m pip install dataclasses importlib_metadata
```

3\. Download "~/code/depot_tools" using Git.

```
cd ~/code
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
```

4\. Add the "~/code/depot_tools" directory to your PATH. Note the use of an absolute path here.

```
export PATH=/home/marshall/code/depot_tools:$PATH
```

5\. Download the "~/automate/automate-git.py" script.

```
cd ~/code/automate
wget https://bitbucket.org/chromiumembedded/cef/raw/master/tools/automate/automate-git.py
```

6\. Create the "~/code/chromium_git/update.sh" script with the following contents.

```
#!/bin/bash
python3 ../automate/automate-git.py --download-dir=/home/marshall/code/chromium_git --depot-tools-dir=/home/marshall/code/depot_tools --no-distrib --no-build
```

Give it executable permissions.

```
cd ~/code/chromium_git
chmod 755 update.sh
```

Run the "update.sh" script and wait for CEF and Chromium source code to download. CEF source code will be downloaded to "~/code/chromium_git/cef" and Chromium source code will be downloaded to "~/code/chromium_git/chromium/src".  After download completion the CEF source code will be copied to "~/code/chromium_git/chromium/src/cef".

```
cd ~/code/chromium_git
./update.sh
```

7\. Configure `GN_DEFINES` for your desired build environment.

Chromium provides [sysroot images](https://chromium.googlesource.com/chromium/src/+/refs/heads/main/docs/linux/sysroot.md) for consistent builds across Linux distros. The necessary files will have downloaded automatically as part of step 6 above. Usage of Chromium's sysroot is recommended if you don't want to deal with potential build breakages due to incompatibilities with the package or kernel versions that you've installed locally. To use the sysroot image configure the following GN_DEFINES:

```
export GN_DEFINES="use_sysroot=true use_allocator=none symbol_level=1 is_cfi=false use_thin_lto=false"
```

It is also possible to build using locally installed packages instead of the provided sysroot. Choosing this option may require additional debugging effort on your part to work through any build errors that result. On Ubuntu 18.04 the following GN_DEFINES have been tested to work reliably:

```
export GN_DEFINES="use_sysroot=false use_allocator=none symbol_level=1 is_cfi=false use_thin_lto=false use_vaapi=false"
```

Note that the "cefclient" target cannot be built directly when using the sysroot image. You can work around this limitation by creating a [binary distribution](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md#markdown-header-manual-packaging) after completing step 9 below, and then building the cefclient target using that binary distribution.

You can also create an [AddressSanitizer build](https://bitbucket.org/chromiumembedded/cef/wiki/UsingAddressSanitizer) for enhanced debugging capabilities. Just add `is_asan=true dcheck_always_on=true` to the GN_DEFINES listed above and build the `out/Release_GN_x64` directory in step 9 below. Run with the `asan_symbolize.py` script as described in the AddressSanitizer link to get symbolized output.

The various other listed GN arguments are based on recommendations from the [AutomateBuildSetup Wiki page](https://bitbucket.org/chromiumembedded/cef/wiki/AutomatedBuildSetup.md#markdown-header-linux-configuration). You can [search for them by name](https://source.chromium.org/search?q=use_allocator%20gni&ss=chromium) in the Chromium source code to find more details.

8\. Run the "~/code/chromium_git/chromium/src/cef/cef_create_projects.sh" script to create Ninja project files. Repeat this step if you change the project configuration or add/remove files in the GN configuration (BUILD.gn file).

```
cd ~/code/chromium_git/chromium/src/cef
./cef_create_projects.sh
```

9\. Create a Debug build of CEF/Chromium using Ninja. Edit the CEF source code at "~/code/chromium_git/chromium/src/cef" and repeat this step multiple times to perform incremental builds while developing. Note the additional "chrome_sandbox" target may be required by step 10. The "cefclient" target will only build successfully if you set `use_sysroot=false` in step 7, so remove that target if necessary.

```
cd ~/code/chromium_git/chromium/src
autoninja -C out/Debug_GN_x64 cefclient cefsimple ceftests chrome_sandbox
```

Replace "Debug" with "Release" to generate a Release build instead of a Debug build.

10\. Set up the [Linux SUID sandbox](https://chromium.googlesource.com/chromium/src.git/+/HEAD/docs/linux/suid_sandbox_development.md) if you are using an older kernel (< 3.8). See [here](https://chromium.googlesource.com/chromium/src.git/+/HEAD/docs/linux/sandboxing.md) for more background on Linux sandboxing technology.

```
# This environment variable should be set at all times.
export CHROME_DEVEL_SANDBOX=/usr/local/sbin/chrome-devel-sandbox

# This command only needs to be run a single time.
cd ~/code/chromium_git/chromium/src
sudo BUILDTYPE=Debug_GN_x64 ./build/update-linux-sandbox.sh
```

11\. Run the cefclient, cefsimple and/or ceftests sample applications. Note that the cefclient application will only have built successfully if you set `use_sysroot=false` in step 7.

```
cd ~/code/chromium_git/chromium/src
./out/Debug_GN_x64/cefclient
```

See the [Linux debugging guide](https://chromium.googlesource.com/chromium/src/+/master/docs/linux_debugging.md) for detailed debugging instructions.

# Next Steps

- If you're seeking a good code editor on Linux check out the [Eclipse](https://chromium.googlesource.com/chromium/src/+/master/docs/linux_eclipse_dev.md) and [Emacs](https://chromium.googlesource.com/chromium/src/+/master/docs/emacs.md) tutorials.
- Review the [Tutorial](https://bitbucket.org/chromiumembedded/cef/wiki/Tutorial.md) and [GeneralUsage](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md) Wiki pages for details on CEF implementation and usage.
- Review the Chromium debugging guide for [Windows](https://www.chromium.org/developers/how-tos/debugging-on-windows), [Mac OS X](https://www.chromium.org/developers/how-tos/debugging-on-os-x) or [Linux](https://chromium.googlesource.com/chromium/src/+/master/docs/linux_debugging.md).
- When you’re ready to contribute your changes back to the CEF project see the [ContributingWithGit](https://bitbucket.org/chromiumembedded/cef/wiki/ContributingWithGit.md) Wiki page for instructions on creating a pull request.

This Wiki page lists notes for CEF releases.

**Note to Editors: Changes made to this Wiki page without prior approval via the [CEF Forum](http://magpcss.org/ceforum/) or [Issue Tracker](https://github.com/chromiumembedded/cef/issues) may be lost or reverted.**

***

**This page is deprecated and will no longer be updated. View the [Commits list](https://bitbucket.org/chromiumembedded/cef/commits/branch/master) to see the complete list of revisions included in automated branch builds.**

Not all revisions are listed here. View the [Commits list](https://bitbucket.org/chromiumembedded/cef/commits/branch/master) to see the complete list of revisions.

**January 27, 2015:** Version 3.2171.1979 for Windows, Mac OS X and Linux includes the following enhancements and bug fixes.

  * Update to Chromium version 39.0.2171.95
  * Add spell checking support (issue #137).
  * Remove the CefBrowserHost::ParentWindowWillClose method that was added for modal dialog support but never implemented (issue #519).
  * Add support for DevTools inspect element via a new |inspect\_element\_at| parameter added to CefBrowserHost::ShowDevTools (issue #586).
  * Linux: Fix crash due to Chromium modifying the |argv| passed to main() (issue #620).
  * Windows: Fix assertion on shutdown when using multi-threaded-message-loop mode (issue #755).
  * Remove CefDOMEvent and CefDOMEventListener which are no longer supported by Chromium (issue #933).
  * Windows/Linux: Fix positioning of select popups and dismissal on window move/resize by calling new CefBrowserHost::NotifyMoveOrResizeStarted() method from client applications (issue #1208).
  * Fix CefBrowser::GetFrameIdentifiers and GetFrameNames to return correct values in the renderer process (issue #1236).
  * Re-implement off-screen rendering using the new delegated rendering approach (issue #1257). This implementation supports both GPU compositing and software compositing (used when GPU is not supported or when passing `--disable-gpu --disable-gpu-compositing` command-line flags). GPU-accelerated features (WebGL and 3D CSS) that did not work with the previous off-screen rendering implementation do work with this implementation when GPU support is available.
  * Linux: Remove GTK+ dependency from libcef and switch to Aura/X11 implementation (issue #1258).
  * Fix configuration of the User-Agent string during startup (issue #1275).
  * Fix execution of OnBeforePluginLoad (issue #1284).
  * Fix delivery of focus/blur events (issue #1301).
  * Mac: Fix flash on resize (issue #1307).
  * Bundle pepper PDF plugin on all platforms (issue #1331). The new pdf.dll is also required on Windows for printing.
  * Move scaled resources from cef.pak into separate cef\_100\_percent.pak and cef\_200\_percent.pak files (issue #1331).
  * Introduce the use of Chromium types in a new include/base folder (issue #1336). See associated changes in cefclient/cefsimple.
  * Windows: Add wow\_helper.exe to the 32-bit binary distribution for Vista 64-bit sandbox support (issue #1366).
  * Reduce resource usage when the window is minimized (issue #1369). See associated changes in cefclient/cefsimple.
  * Fix identification of the focused frame (issue #1381).
  * Mac: Add NSSupportsAutomaticGraphicsSwitching to Info.plist so that the GPU is only triggered when needed (issue #1396).
  * Call OnTitleChange after navigation/reload even if the title has not changed (issue #1441).
  * Add CefBrowserHost::GetNavigationEntries for retrieving a snapshot of navigation history (issue #1442).
  * Pass cursor type and custom cursor information to CefRenderHandler::OnCursorChange (issue #1443).
  * Linux: Fix "No URLRequestContext for NSS HTTP handler" error (issue #1490).

**June 12, 2014:** Version 3.1750.1738 for Windows, Mac OS X and Linux includes the following enhancements and bug fixes.

  * Update to Chromium version 33.0.1750.170
  * Add the cefsimple sample application which demonstrates the minimal functionality required to create a browser window.
  * Windows: Switch to aura/views architecture for content window creation (issue #180).
  * Windows: Support non-ASCII paths for CefStream file access (issue #481).
  * Add sandbox support (issue #524).
    * The sandbox is now enabled by default on all platforms. Use the CefSettings.no\_sandbox option or the "no-sandbox" command-line flag to disable sandbox support.
    * Windows: See cef\_sandbox\_win.h for requirements to enable sandbox support and usage of CEF\_ENABLE\_SANDBOX define in cefclient/cefsimple.
    * Windows: If Visual Studio isn't installed in the standard location set the CEF\_VCVARS environment variable before running make\_distrib.py or automate.py (see msvs\_env.bat).
    * Linux: For binary distributions a new chrome-sandbox executable with SUID permissions must be placed next to the CEF executable. See https://code.google.com/p/chromium/wiki/LinuxSUIDSandboxDevelopment for details on setting up the development environment when building CEF from source code.
  * Support DevTools without remote debugging via CefBrowserHost::ShowDevTools and CloseDevTools methods (issue #659).
  * Fix crash when using asynchronous continuation with a custom resource handler (issue #1066).
  * Windows: Support building with Visual Studio 2013 (issue #1094).
  * Add a CefGetMimeType function for retrieving the mime type of a file extension (issue #1098).
  * Enable print option in context menu (issue #1127).
  * Add breakpad support (issue #1131).
    * General usage instructions are available at https://sites.google.com/a/chromium.org/dev/developers/testing/webkit-layout-tests/using-breakpad-with-content-shell.
    * Mac: Generate "Chromium Embedded Framework.framework" using a new cef\_framework target (the libcef target is now only used on Windows and Linux). Rename "Libraries/libcef.dylib" to "Chromium Embedded Framework". Distribute Release and Debug builds of the "Chromium Embedded Framework.framework" folder as part of the binary distribution.
    * Mac: Fix the Xcode target compiler setting for the binary distribution so that it no longer needs to be set manually.
  * Remove the built-in about:tracing UI (issue #1146). Traces generated with CEF can be loaded in Google Chrome about:tracing.
  * Fix C compiler errors when using the C API (issue #1165).
  * Provide a generic JavaScript message router implementation (issue #1183).
  * Add MayBlock() method to stream classes which is used as a hint when determining the thread to access the stream from (issue #1187).
  * Mac: Change shutdown-related code to match Chromium (issue #1203)
  * Fix delivery of focus/blur events (issue #1301)

**January 13, 2014:** Version 3.1650.1562 for Windows, Mac OS X and Linux includes the following enhancements and bug fixes.

  * Update to Chromium version 31.0.1650.57
  * Add support for printing via CefBrowserHost::Print() and JavaScript window.print() (issue #505).
  * Add search/find support via CefBrowserHost::Find (issue #513).
  * Fix assignment of log file path (issue #903).
  * Make CefURLRequest::GetResponse() data available before download completion (issue #956).
  * Add CefURLRequestClient::GetAuthCredentials callback (issue #975).
  * Support file uploads in CefURLRequests originating from the browser process (issue #1013).
  * Fix memory corruption in browser-initiated CefURLRequests (issue #1118).
  * Add new CefRequestContext and CefRequestContextHandler classes (issue #1044).
  * Add CefRequestContext argument to CefBrowserHost static factory methods (issue #1044).
  * Move GetCookieManager from CefRequestHandler to CefRequestContextHandler (issue #1044).
  * Fix loading of popup windows with a newly created render process (issue #1057).
  * Expose resource type and transition type via CefRequest (issue #1071).
  * Add CefRequestHandler::OnBeforeBrowse callback (issue #1076).
  * Expose CefLoadHandler in the render process (issue #1077).
  * Fix loading of DevTools frontend from chrome-devtools scheme (issue #1095).
  * Fix a crash when closing an off-screen browser with no references (issue #1121).
  * Set "User-Agent" and "Accept-Language" headers for requests sent using CefURLRequest (issue #1125).
  * Fix crash when browser window creation is canceled (issue #1147).
  * Linux: Apply system font settings (issue #1148).
  * Linux: Honor the CefSettings.product\_version setting (issue #1149).
  * Allow customization of default background color (issue #1161).

**August 23, 2013:** Version 3.1547.1412 for Windows, Mac OS X and Linux includes the following enhancements and bug fixes.

  * Update to Chromium version 29.0.1547.59.
  * Introduce 64-bit builds for Windows and Mac OS X.
  * Windows: Add CefSetOSModalLoop function to improve native menu responsiveness (issue #194).
  * Linux: Add off-screen rendering support (issue #518).
  * Force flash and silverlight plugins to windowless mode when using off-screen rendering (issue #518).
  * Add support for CefDragHandler::OnDragEnter (issue #601).
  * Mac: Don't create extra Helper folders in the app bundle (issue #806).
  * Reduce locking in browser and frame implementations to avoid potential deadlocks (issue #824).
  * Add CefRenderHandler::OnScrollOffsetChanged callback (issue #889).
  * Add CefJSDialogHandler::OnDialogClosed callback (issue #943).
  * Add additional V8 performance tests (issue #960).
  * Fix keyboard handling for popups when using off-screen rendering (issue #962).
  * Use scoped class to implement CEF\_TRACE\_EVENTx (issue #971).
  * Mac: Add IME support for off-screen rendering (issue #973)
  * Linux: Force use of the sub-process executable path for the zygote process (issue #980).
  * Mac: Improve scroll event handling for the cefclient off-screen rendering example (issue #982).
  * Add a password argument to CefZipArchive::Load (issue #986).
  * Use chrome translations for most context menu strings (issue #992).
  * Improve V8 memory management and performance (issue #1001).
  * Linux: Include the libffmpegsumo.so library in binary distributions (issue #1003).
  * Windows: Force child processes to exit on main process termination (issue #1011).
  * Fix problem routing messages to CEF network-related callbacks (issue #1012).
  * Mac: Fix crash on 10.6 and older 10.7 versions when building with the 10.7 SDK (issue #1026).
  * Mac: Support retina display when building with the 10.6 SDK (issue #1038).

**May 9, 2013:** Version 3.1453.1255 for Windows, Mac OS X and Linux 64-bit includes the following enhancements and bug fixes.

  * Update to Chromium version 27.0.1453.73.
  * Add CefRequestHandler::OnCertificateError() callback (issue #345).
  * Remove V8 worker bindings (issue #451).
  * Mac: Add off-screen rendering support (issue #518).
  * Fix WebRTC support (issue #531).
  * Add support for x-webkit-speech (issue #758).
  * Change the way that application shutdown works in order to support JavaScript 'onbeforeunload' handling (issue #853). See comments in cef\_life\_span\_handler.h for a detailed description of the new shutdown process.
  * Add CefBrowserHost::SetMouseCursorChangeDisabled() method for disabling mouse cursor changes (issue #884).
  * Add new CefNowFromSystemTraceTime() method (issue #908).
  * Add CefBrowserHost::WasHidden() method for notifying a windowless browser that it has been hidden or shown (issue #909).
  * Use a platform API hash instead of build revision for checking libcef compatibility (issue #914).
  * Add support for the V8 LoadTimes extension (issue #917).
  * Support cross-origin XMLHttpRequest loads and redirects for custom standard schemes when enabled via the cross-origin whitelist (issue #950).
  * cefclient: Simplify and streamline the cefclient sample application.
  * cefclient: Enable DevTools by default.
  * cefclient: Resolve make build issues with the Linux binary distribution.
  * Introduce a new download organization with separate archives for documentation, distributions and the cefclient sample application.

**April 8, 2013:** Version 3.1364.1188 is the first binary release for 32-bit and 64-bit Linux. In addition to the changes described below for version 3.1364.1094 it includes the following enhancements and bug fixes.

  * Add the ability to persist session cookies (issue #881).
  * Add a new CefBrowserHost::StartDownload method for starting a download that can then be handled via CefDownloadHandler (issue #883)
  * Add new CefSettings.ignore\_certificate\_errors option to ignore errors related to invalid SSL certificates (issue #345).
  * cefclient: Add example of window manipulation via JavaScript (issue #925).

**March 4, 2013:** Version 1.1364.1123 contains the following enhancements and bug fixes.

  * Update to Chromium version 25.0.1364.152.
  * Mac: OS-X 10.5 (Leopard) is no longer supported.
  * Default Google API keys for geolocation are no longer supported. Register for custom API keys as described [here](http://www.chromium.org/developers/how-tos/api-keys).
  * Windows: Add new CefSetOSModalLoop function to improve menu responsiveness (issue #194).
  * Allow empty |mimeType| argument to GetDownloadHandler (issue #405).
  * Support implicit detachment of CEF V8 references when the associated context is released (issue #484).
  * Reduce persistent CEF V8 memory usage by tracking objects on a per-context basis and not persisting objects when the underlying V8 handle is unused (issue #484).
  * cefclient: Add performance tests for CEF V8 methods (issue #484).
  * Mac: cefclient: Call makeFirstResponder:nil from windowDidResignKey: to remove focus from the browser view when the window becomes inactive (issue #565).
  * Add CefApp::OnRegisterCustomSchemes callback to address url\_util thread safety issues (issue #630).
  * Add CefV8ContextHandler::OnUncaughtException callback (issue #736).
  * Add a new CefGetGeolocation function for retrieving geolocation information via an API callback (issue #764).
  * Windows: Enable IME for all text input types (issue #765).
  * Mac: Add default tooltip implementation (issue #770).
  * Add new CefDisplayHandler::OnFaviconURLChange callback (issue #779).
  * Add a switch to enable DCHECKs in release builds (issue #790).
  * Windows: Display select filters based on the "accept" attribute for input type="file" (issue #791).
  * Mac: Add default implementation for JavaScript confirm/prompt dialogs (issue #795).
  * Add new CefBrowser::GetIdentifier method (issue #811).
  * Mac: Fix crash with input element of type "file" when the default file name is empty (issue #823).
  * Allow empty |browser| parameter to CefSchemeHandlerFactory::Create (issue #834).
  * Windows: Add default implementation for JavaScript prompt dialog (issue #861).
  * Windows: cefclient: Remove dependency on ATL to allow building with Express versions of Visual Studio.

**February 9, 2013:** Version 3.1364.1094 contains the following enhancements and bug fixes.

  * Update to Chromium version 25.0.1364.68.
  * Mac: OS-X 10.5 (Leopard) is no longer supported.
  * Mac: Add support for Retina displays.
  * Default Google API keys for Geolocation are no longer supported. Register for custom API keys as described [here](http://www.chromium.org/developers/how-tos/api-keys).
  * Support custom V8 bindings on WebWorker threads (issue #451).
  * Add CefRenderProcessHandler callbacks for WebWorker context creation, release and uncaught exceptions (issue #451).
  * Add CefTaskRunner class that supports posting of tasks to standard threads and WebWorker threads (issue #451).
  * Support implicit detachment of CEF V8 references when the associated context is released (issue #484).
  * Reduce persistent CEF V8 memory usage by tracking objects on a per-context basis and not persisting objects when the underlying V8 handle is unused (issue #484).
  * cefclient: Add performance tests for CEF V8 methods (issue #484).
  * Windows: Add off-screen rendering support (issue #518).
  * Pass actual HTTP response code to CefLoadHandler::OnLoadEnd (issue #521).
  * Introduce chrome proxy implementation based on command-line flags (issue #600).
  * Expose tracing functionality via new cef\_trace.h and cef\_trace\_event.h headers (issue #711).
  * Add about:tracing UI support (issue #711).
  * Add new CefRenderProcessHandler::OnBeforeNavigation callback (issue #722).
  * Add about:version, about:credits and about:license internal URLs (issue #731).
  * Add CefV8ContextHandler::OnUncaughtException callback (issue #736).
  * Pass information to the renderer process synchronously on render thread creation and new browser creation to avoid race conditions (issue #744).
  * Add the ability to pass extra information to child processes using a new CefBrowserProcessHandler::OnRenderProcessThreadCreated callback (issue #744).
  * Pass resource-related command-line flags to all process types (issue #759).
  * Provide default implementations of the file chooser dialogs (open, open multiple, save) on all platforms (issue #761).
  * Add a new CefBrowserHost::RunFileDialog method that displays the specified file chooser dialog and returns the results asynchronously (issue #761).
  * Add a new CefDialogHandler::OnFileDialog callback that allows the application to provide custom UI for file chooser dialogs (issue #761).
  * Add a new CefGetGeolocation function for retrieving geolocation information via an API callback (issue #764).
  * Send OnTitleChange() notifications when navigating history (issue #766).
  * Add a CefSettings.release\_dcheck\_enabled option to enable DCHECKs in release builds (issue #790).
  * Do not send network requests for canceled popup windows (issue #816).
  * Fix assertion when returning NULL from CefClient::GetGeolocationHandler (issue #857).
  * Windows: Avoid assertion when entering zero-length text in the default JavaScript prompt dialog (issue #862).
  * Change CefBrowserSettings members that previously used a boolean to instead use a cef\_state\_t enumeration with default, enabled and disabled states (issue #878).
  * Remove CefBrowserSettings members that are unlikely to be used and that can also be set using Chromium command-line switches (issue #878).
  * Change the CEF command-line switch naming pattern to match Chromium and move the implementation out of cefclient (issue #878).

**October 2, 2012:** Version 1.1180.832 contains the following enhancements and bug fixes.

  * Update to Chromium version 21.0.1180.91. This fixes a number of bugs in Chromium and WebKit.
  * Add CefV8Value::CreateUInt method and indicate that integer types are 32bit via usage of int32 and uint32 types (issue #331).
  * Add geolocation support (issue #365).
  * Add CefV8Context::Eval method for synchronous JavaScript execution that returns a value or exception (issue #444).
  * Fix opening of password protected files with CefZipReader (issue #496).
  * Allow wmode="transparent" for Flash when using off-screen rendering (issue #527).
  * Remove support for HTML5 audio & video tags (issue #530).
  * Fix usage of ReadRawData return value in scheme handler implementation (issue #534).
  * Mac: Add off-screen rendering support (issue #540).
  * Add persistent HTML5 application cache support (issue #543).
  * Move exception handling from an ExecuteFunction argument to a CefV8Value attribute (issue #546).
  * Make user data an attribute for all CefV8Value object types and not just CreateObject (issue #547)
  * Rename PROXY\_TYPE values to avoid naming conflict (issue #548).
  * Mac: Add IME support (issue #557).
  * Change cefclient off-screen rendering example to account for premultiplied alpha values (issue #584).
  * Improve the cefclient transparency test by adding the ability to view individual pixel values (issue #584).
  * Windows: Fix mouse wheel scrolling in second monitor (issue #595).
  * Make the |target\_domain| parameter to CefAddCrossOriginWhitelistEntry and CefRemoveCrossOriginWhitelistEntry optional (issue #609).
  * Delay destroying the browser window until pending drag operations have completed (issue #610).
  * Fix misspelling of the Referer HTTP header (issue #619).
  * Mac: Add support for cef\_time\_t.day\_of\_week (issue #629).
  * Eliminate use of scoped directories (issue #670).
  * Only release the request context proxy if it exists (issue #677).
  * Add CefV8StackTrace and CefV8StackFrame interfaces to support retrieval of the JavaScript stack trace for the currently active V8 context (issue #682).
  * Fix crashes/assertions when CefBrowserImpl is destroyed on a non-UI thread (issue #694).
  * Improve the handling of invalidation/painting for off-screen rendering (issue #695).
  * Add the ability to customize the animation frame rate (issue #697).
  * Windows: Erase in-progress IME composition on cancel (issue #701).
  * Move devtools resources to a separate devtools\_resources.pak file to allow complete exclusion of devtools resources from client applications (issue #714).
  * Add a "url" command-line option to cefclient (issue #715).
  * Allow use of an empty key string with CefV8Value methods (issue #718).
  * Fix crash when pending navigation executes after the browser window has been destroyed (issue #724).
  * Mac: Add a CEF version number to dylib files (issue #730).
  * Add a CefZoomHandler interface to support custom zoom handling (issue #733).
  * Create a temporary cache\_path directory if none is specified (issue #735).
  * Windows: Fix VS2008 compile errors in CefByteReadHandler.

**September 28, 2012:** Version 3.1180.823 contains the following enhancements and bug fixes.

  * Update to Chromium version 21.0.1180.91. This fixes a number of bugs in Chromium and WebKit.
  * Fix opening of password protected files with CefZipReader (issue #496).
  * Rename PROXY\_TYPE values to avoid naming conflict (issue #548).
  * Add CefBrowserProcessHandler::OnBeforeChildProcessLaunch and CefCommandLine::PrependWrapper to support custom construction of the command line for child processes (issue #628).
  * Add CefRequestHandler::OnBeforePluginLoad callback and new cef\_web\_plugin.h functions for controlling plugin loading and life span (issue #645).
  * Implement CefDisplayHandler OnStatusMessage and OnConsoleMessage callbacks (issue #662).
  * Mac: Add libplugin\_carbon\_interpose.dylib to fix plugin crash (issue #680).
  * Add CefV8StackTrace and CefV8StackFrame interfaces to support retrieval of the JavaScript stack trace for the currently active V8 context (issue #682).
  * Fix a DCHECK failure when calling OnSetFocus from LoadRequest (issue #685).
  * Windows: Fix loading of custom cursor resources (issue #692).
  * Allow title change notifications after the document has loaded (issue #704).
  * Move devtools resources to a separate devtools\_resources.pak file to allow complete exclusion of devtools resources from client applications (issue #714).
  * Propagate logging command-line parameters to all processes (issue #717).
  * Allow use of an empty key string with CefV8Value methods (issue #718).
  * Mac: Add a CEF version number to dylib files (issue #730).
  * Add CefRequestHandler::OnQuotaRequest callback for handling webkitStorageInfo.requestQuota requests (issue #732).
  * Create a temporary cache\_path directory if none is specified (issue #735).
  * Windows: Fix VS2008 compile errors in CefByteReadHandler.

**June 28, 2012:** Version 3.1180.719 is the first binary release of CEF3. Below is an overview of features supported by this release. See the [CEF3 Development Status thread](http://magpcss.org/ceforum/viewtopic.php?f=10&t=645) for more information.

  * Windows, Mac OS-X and Linux support
  * Most HTML5 features
  * HTML5 drag&drop support (issue #504)
  * Geolocation support (issue #365)
  * GPU acceleration
  * Browser/frame load and navigation notifications
  * Resource request loading, interception and substitution
  * Custom scheme handling and cross-origin white lists
  * Custom proxy handling
  * Utility classes for command-line, URL, XML and zip reading/parsing
  * JavaScript binding & extension support (issue #506)
  * Direct cookie access (issue #512)
  * WebRTC support (issue #531)
  * User-defined cross-process communication (issue #544)
  * JavaScript dialog (alert, confirm, prompt, onbeforeunload) support (issue #507)
  * Context menu support (issue #509)
  * Direct DOM access (issue #511)
  * Keyboard and focus notifications/interception (issue #508)
  * Control over user data persistence (issue #510)
  * Zoom support (issue #514)
  * Download handling (issue #516)
  * External protocol handling (issue #582)
  * WebURLRequest support (issue #517)
  * Windows: multi\_threaded\_message\_loop run mode support (issue #522)

**April 26, 2012:** Version 1.1025.607 contains the following enhancements and bug fixes.

  * Break cef.h into multiple headers (issue #142).
  * Add the ability to disable or customize pack file load paths (issue #236).
  * Allow custom resource bundle handling via CefResourceBundleHandler (issue #236).
  * Remove the CefV8Value::CreateObject variant that accepts only one argument (issue #449).
  * Invalidate the entire scroll rect when using off-screen rendering mode (issue #469)
  * Add the ability to restrict V8 extension loading by frame using a new CefPermissionHandler::OnBeforeScriptExtensionLoad() callback (issue #471).
  * Make CEF compliant with Google/Chromium style (issue #473).
  * Add methods to CefV8Value for specifying the amount of externally allocated memory associated with the V8 object (issue #478).
  * Don't add the "--expose-gc" V8 flag by default for all CEF users (issue #483).
  * Fix bug in CefZipArchive::Clear() (issue #486).
  * Mac: Fix the "no autorelease pool in place" error by initializing an NSAutoreleasePool on every thread (issue #502).
  * Fix memory leak in CefV8ValueImpl::ExecuteFunctionWithContext (issue #526).
  * Fix the OnBeforeResourceLoad redirectUrl value to behave the same as using request->SetURL() (issue #529).
  * Windows: Fix crash in cefclient when entering long URLs via the address bar (issue #532).
  * Add CefCookieManager interface and CefRequestHandler::GetCookieManager for custom cookie handling (issue #542).
  * Support getting and setting cookies with custom scheme handlers (issue #555).
  * Support calling CefFrame::GetIdentifier and CefFrame::GetURL on any thread (issue #556).
  * Add CefCookieManager::SetSupportedSchemes method which supports cookie storage for non-http(s) schemes (issue #567).
  * Mac: Add flagsChanged: signal handler so that modifier keys are correctly passed to JS (issue #574).
  * Add the ability to directly retrieve plugin information via CefGetWebPluginInfo (issue #575).
  * Fix memory leak when returning zero-length strings from V8 (issue #581).

**December 22, 2011:** Version 1.963.439 contains the following enhancements and bug fixes.

  * Fix crash on shutdown due to CefProcess not being destroyed immediately (issue #277).
  * Introduce CefV8Exception for returning detailed exception information from CefV8Value::ExecuteFunction() and add the ability to re-throw exceptions (issue #317).
  * Include CEF and Chromium version information in binary distributions (issue #325).
  * Disable stats, histogram and event tracing to avoid memory leaks (issue #328).
  * Add the ability to observe and modify resource redirects (issue #346).
  * Replace JSBindingHandler with a new V8ContextHandler interface that contains callbacks for V8 context creation and release (issue #359).
  * Significant improvements in painting and scrolling performance (issue #360).
  * (win) Fix CefBrowser::GetImage() and add "Get Image" example to cefclient (issue #377).
  * Add a new CefProxyHandler interface to allow applications to resolve proxy information (issue #389).
  * Add a new CefApp interface that provides global handlers and gets passed to CefInitialize() (issue #399).
  * Pass a list of dirty rectangles instead of a single combined rectangle to CefRenderHandler::Paint() (issue #415).
  * (win) Use the module path for loading resources instead of hard-coding "libcef.dll" (issue #416).
  * Add a CefBrowserSettings.history\_disabled option to disable history back/forward navigation (issue #419).
  * Expose command line parsing support with a new CefCommandLine class (issue #422).
  * (win) Fix selection of multiple files with long combined paths (issue #423).
  * (mac) Fix handling of numeric pad characters. Don't send character events for non-character numeric and function keys (issue #426).
  * Add a CefV8Context::InContext() method to test if V8 is currently in a context (issue #427).
  * Verify that a current context exists when creating V8 arrays, functions and objects (issue #427).
  * Add a v8::HandleScope in GetCurrentContext() and GetEnteredContext() to avoid "Cannot create a handle without a HandleScope" V8 errors (issue #427).
  * Add additional error checking for CefV8Value methods (issue #427).
  * Make navigator.language return the same value as CefSettings.locale (issue #429).
  * Verify that libcef build revision and API/header revision match when initializing CEF (issue #431).
  * Allow registration of V8 extensions with no native function handler (issue #433).
  * Fix crash caused by V8Proxy::retrieveFrameForCallingContext() returning 0 in V8DOMWindowCustom::handlePostMessageCallback (issue #436).
  * Fix BrowserFileSystem context creation race condition between UI and IO threads (issue #442).
  * Add CefQuitMessageLoop function (issue #443).
  * Allow media hosting from all schemes (issue #446).
  * (mac) Fix problem where images loaded using the background-image CSS attribute sometimes do not display after browsing back to the page (issue #447).
  * Expose unique identifiers for frames and the ability to retrieve a frame's parent frame (issue #450).
  * Reduce CPU usage with requestAnimationFrame by maintaining a consistent 60fps frame rate (issue #456).
  * (win) Reduce memory/CPU usage with UpdateInputMethod by restricting tasks to every 100ms (issue #454).
  * Add CefBrowserSettings.fullscreen\_enabled flag for enabling fullscreen mode (issue #457).


**November 8, 2011:** Binary release 365 contains the following enhancements and bug fixes.

  * (win) Add support for transparency (issue #99).
  * (win) Fix the transparent background for the toolbar in DevTools (issue #313).
  * Hide CEF internal V8 attributes from JavaScript (issue #316).
  * Fix problem calling CefBrowser::SetFocus() from a non-UI thread (issue #320).
  * (win) Add CefSettings.auto\_detect\_proxy\_settings\_enabled option for enabling automatic proxy detection (issue #332).
  * Add CefDisplayHandler::OnContentsSizeChange() notificiation (issue #341).
  * Add handling for empty V8 exception messages (issue #342).
  * Add CefFrame::GetV8Context() method for retrieving the V8 context of a frame (issue #344).
  * Add CefSetCookiePath() and CefSetStoragePath() functions for changing cookie and localStorage locations while CEF is running (issue #347).
  * (mac) Significant repaint and scroll performance improvements (issue #360).
  * Add a new NAVTYPE\_LINKDROPPED value that will be passed to OnBeforeResourceLoad() when navigation is resulting from drag & drop of URLs (issue #363).
  * Add a CefBrowserSettings.load\_drops\_disabled option for disabling default navigation resulting from drag & drop of URLs (issue #363).
  * (win) Look for the chrome.pak file in the module (libcef.dll) directory to match the locale folder location (issue #374).
  * Add CefFocusHandler::OnFocusedNodeChanged() notification (issue #379).
  * Add CefDOMNode::IsFormControlElement() and CefDOMNode::GetFormControlElementType() methods (issue #379).
  * Don't call OnTitleChange() and OnLoadEnd() for failed provisional loading (issue #381).
  * Use cef-error URLs for error pages (issue #382).
  * Use multimap type for storing header values (issue #386).
  * Re-register the internal chrome-devtools scheme handler after CefClearSchemeHandlerFactories() is called (issue #398).
  * Add the ability to specify full plugin matching parameters including multiple mime types and file extensions via CefPluginInfo (issue #401).
  * (mac) Add file chooser support for input type="file" (issue #403).
  * Add new call to OnKeyEvent() to allow handling of keyboard events before they're passed to the renderer (issue #406).
  * (win) Only scroll with the middle mouse button when the cursor is over the view (issue #410).
  * Add a PropertyAttribute parameter to CefV8Value::SetValue() (issue #412).
  * Add support for specifying custom V8 flags via a new CefSettings.javascript\_flags configuration option (issue #413).
  * Pass the |redirectUrl| parameter to GetResponseHeaders() instead of ProcessRequest() (issue #414).

**October 7, 2011:** Binary release 306 introduces the new distribution packaging system (issue #260) that provides binary distributions for both Windows and Mac OS X. It also contains the following enhancements and bug fixes.

  * (mac) Don't show magenta background when redrawing in release build (issue #300).
  * Increase the internal resource buffer size to the maximum allowed to improve resource loading speed (issue #301).
  * Add CefSettings.local\_storage\_quota and CefSettings.session\_storage\_quota options for setting localStorage and sessionStorage quota limits respectively (issue #348).
  * Add a CefBrowser::ClearHistory() method for clearing back/forward browsing history (issue #352).
  * Add support for loading localized strings from locale .pak files (issue #357).
  * Add support for loading DevTools resources from chrome.pak via the chrome-devtools scheme (issue #358).
  * Improve redraw and scrolling performance (issue #360).
  * Add storage functions and CefStorageVisitor interface for accessing localStorage and sessionStorage data via the native API (issue #361).
  * Pass the browser originating the request to CefSchemeHandlerFactory::Create() (issue #362).
  * Add a FocusSource parameter to OnSetFocus() that indicates where the focus request is originating from (issue #369).

**September 23, 2011:** Binary release 293 adds support for Visual Studio 2010 and contains the following enhancements and bug fixes.

  * Rename char16\_t to char16 to fix VS2010 compiler errors due to char16\_t becoming a built-in type (issue #243).
  * Support asynchronous continuation of custom scheme handler responses (issue #269).
  * Add CefDragHandler to support examination of drag data and cancellation of drag requests (issue #297).
  * Add a CefBrowser::HasDocument() method that tests if a document has been loaded in the browser window (issue #307).
  * Add a virtual destructor to the CefBase class (issue #321).
  * Fix memory leaks in V8 usage (issue #323).
  * Improve performance of V8 string conversions (issue #323).
  * Add the ability to return exceptions from V8 accessors (issue #327).
  * Return undefined instead of null from a V8 handler if no return value is specified (issue #329).
  * Accelerated compositing has been disabled by default due to implementation bugs (issue #334, issue #335, issue #337).

**August 9, 2011:** Binary release 275 contains the following enhancements and bug fixes.

  * Disable touch support to allow Google Maps API to function correctly (issue #134).
  * Fix a deadlock when executing many synchronous load requests (issue #192).
  * Fix OnResourceResponse spelling error (issue #270).
  * Fix a thread death race condition that can result in a crash in some circumstances on shutdown (issue #277).
  * Add support for accelerated\_video\_enabled, accelerated\_drawing\_enabled and accelerated\_plugins\_enabled browser settings (issue #278).
  * Add d3dx9\_43.dll and d3dcompiler\_43.dll files required by accelerated content on some Windows machines (issue #280).
  * Clean up the implementation of modal window callbacks (issue #281).
  * Disable speech input to avoid a crash when clicking the microphone icon on the Google search page (issue #282).
  * Add support for disabling HTML5 drag from the browser by setting drag\_drop\_disabled to true (issue #284).
  * Fix a crash when canceling a popup window (issue #285).

**June 14, 2011:** Binary release 259 significantly revamps the CEF API and contains the following enhancements and bug fixes.

  * Use the angle library for GL support (issue #136).
  * Add Date type support to CefV8Value (issue #190).
  * Add a workaround for the SyncRequestProxy deadlock problem (issue #192).
  * Replace CefHandler with a new CefClient interface and separate handler interfaces (issue #218).
  * Add support for virtual inheritance to allow multiple CefBase parented interfaces to be implemented in the same class (issue #218).
  * Replace CefThreadSafeBase with IMPLEMENT macros to support virtual inheritance and to only provide locking implementations when needed (issue #218).
  * Move the CefBrowserSettings parameter from CefInitialize to CreateBrowser (issue #218).
  * Add a new cef\_build.h header that provides platform-specific and OS defines (issue #218).
  * Introduce the use of OVERRIDE to generate compiler errors on Windows if a child virtual method declaration doesn't match the parent declaration (issue #218).
  * Move CEF header files that should not be directly included by the client to the "include/internal" folder (issue #218).
  * Add support for navigator.onLine and online/offline window events (issue #234).
  * Use NDEBUG instead of `_`DEBUG because `_`DEBUG is not defined on Mac (issue #240).
  * Add OnResourceReponse and CefContentFilter for viewing and filtering response content (issue #241).
  * Add support for setting response header values (issue #246).
  * Break CefRegisterScheme into separate CefRegisterCustomScheme and CefRegisterSchemeHandlerFactory functions (issue #246).
  * Allow registration of handlers for built-in schemes (issue #246).
  * Supply scheme and request information to CefSchemeHandlerFactory::Create (issue #246).
  * Add CrossOriginWhitelist functions for bypassing the same-origin policy with both built-in and custom standard schemes (issue #246).
  * Add support for modal dialogs (issue #250).
  * Add support for IME-aware applications (issue #254).
  * Restore keyboard focus on window activation (issue #256).
  * Fix bug when dragging to a window before mouse events have been detected (issue #262).

**May 10, 2011:** Binary release 231 contains the following enhancements and bug fixes.

  * Add cookie get/set support (issue #88).
  * Allow custom schemes to cause redirects (issue #98).
  * Set the libcef.dll version number to the build revision number (issue #108).
  * Add support for returning an HTTP status code from HandleBeforeResourceLoad and custom scheme handlers via the CefResponse class (issue #202).
  * Add support for V8 accessors and entering a V8 context asynchronously (issue #203).
  * Don't load URLs twice for popup windows (issue #215).
  * Make modal popup windows behave the same as non-modal popup windows (issue #216).
  * Force Flash and Silverlight plugins to use windowless mode when rendering off-screen (issue #214).
  * Don't download files that will be loaded by a plugin (issue #227).
  * Add a CefDOMNode::IsSame() method for comparing CefDOMNode objects.

**March 25, 2011:** Binary release 212 contains the following enhancements and bug fixes.

  * Add off-screen rendering support (issue #100).
  * Add persistent storage support for cookie data (issue #193).
  * Allow registration of non-standard schemes (issue #195).
  * Improve the behavior of HandleAddressChange, HandleTitleChange, HandleLoadStart and HandleLoadEnd notifications (issue #200).
  * Respect the WS\_VISIBLE flag when creating browser windows (issue #201).
  * Fix a bug in CefWebURLRequest that could result in inappropriate calls to CefHandler methods (issue #204).
  * Add a history entry when navigating to anchors within the same page (issue #207).
  * Add a HandleNavStateChange notification for back/forward state changes (issue #208).
  * Fix crash when using WebKit inspector break points (issue #210).
  * Add support for retrieving values from DOM form elements using CefDOMNode::GetValue.
  * Add the CefRunMessageLoop() function for efficient message loop processing in single-threaded message loop mode.

**February 28, 2011:** Binary release 195 contains the following enhancements and bug fixes.

  * Add the CefWebURLRequest class that supports direct loading of URL resources from client applications (issue #51).
  * Add CefDOM classes and the CefFrame::VisitDOM method that allow direct access to and modification of the DOM (issue #60).
  * Add support for the HTML5 drag and drop API and support for dragging content to other applications or the desktop (issue #140).
  * Add a CefV8Context object and CefV8Value::ExecuteFunctionWithContext method to support asynchronous V8 callbacks (issue #188).
  * CefRegisterPlugin now only supports a single mime type per registration.
  * Fix a bug where URL and title change notifications were not being sent for CefFrame::LoadString.

**February 1, 2011:** Binary release 181 will be the last binary release that includes libraries for VS2005. It contains the following enhancements and bug fixes.

  * Improve thread safety by making some methods only callable on the UI thread (issue #175).
  * Add NewCefRunnableMethod and NewCefRunnableFunction templates (in cef\_runnable.h) that simplify task posting (issue #175).
  * Add a boolean argument to the HandleLoadStart and HandleLoadEnd events that will be true while the main content frame is loading (issue #166, issue #183).
  * Add an HTTP status code argument to the HandleLoadEnd event (issue #177).
  * Only call the HandleAddressChange and HandleTitleChange events for the main content frame load (issue #166)
  * Pass the URL for new popup windows to the HandleBeforeCreated event (issue #5).
  * Add members to the CefSettings structure for specifying log file location and severity level (issue #172).
  * Add single sign-on support (issue #148).
  * Add zooming support (issue #116).
  * Add developer tools support (issue #127).
  * Add a HandleProtocolExecution event for handling unregistered protocols (issue #155).
  * Add a HandleStatus event for status messages, mouse over URLs and keyboard focus URLs (issue #61).
  * Add support for creating and parsing URLs via CefCreateURL and CefParseURL (issue #181).
  * Fix the problem with frame activation when displaying SELECT list popups (issue #169).
  * Accelerated compositing and HTML5 video now work together (issue #143).

**November 22, 2010:** Revision 149 introduces a number of long-awaited features and a few important bug fixes.

  * The API now uses CefString and cef\_string\_t types instead of std::wstring and wchar\_t. The new types support conversion between ASCII, UTF-8, UTF-16 and UTF-32 character formats and the default character type can be changed by recompiling CEF (issue #146).
  * Allow customization of global and per-browser settings for everything from User-Agent and plugin search paths to specific WebKit features (issue #145).
  * Add support for accelerated compositing and fast WebGL (issue #136). You will need to disable accelerated compositing to watch HTML5 video with this release (issue #143).
  * Expose popup window feature information via the CefPopupFeatures argument passed to CefHandler::HandleBeforeCreated (issue #135).
  * Fix a crash caused by Flash-related JavaScript (issue #115).

**November 15, 2010:** Revision 137 provides the first working build of CEF for the Mac OS X platform (issue #68). There's still a lot of work required to bring it up to par with the Windows port. Missing features are indicated by "TODO(port)" comments in the source code. Help with fixing bugs and implementing missing features is welcome.

**October 24, 2010:** Revision 126 disables WebGL support due to performance problems with the default Chromium implementation. WebGL support will be re-enabled once a better-performing implementation is available (issue #136).

  * Add a CefHandler::HandleDownloadResponse() method for handling Content-Disposition initiated file downloads (issue #6).
  * Add XML parsing support with CefXmlReader and CefXmlObject classes (issue #28).
  * Add Zip archive reading support with CefZipReader and CefZipArchive classes (issue #30).
  * Add a new cef\_wrapper.h header file that exposes helpful utility classes provided as part of the libcef\_dll\_wrapper target.

**October 15, 2010:** Revision 116 causes CEF to ignore the "Automatically detect settings" option under LAN Settings in order to fix a problem with slow resource loading on Windows (issue #81). Manual configuration under LAN Settings is still respected.

  * Add a CefBrowser::ReloadIgnoreCache() method and MENU\_ID\_NAV\_RELOAD\_NOCACHE menu support. (issue #118).
  * Add support for audio playback with HTML5 video (issue #121).
  * Fix back/forward navigation when the history contains pages that failed to load (issue #125).
  * Change the CEF User-Agent product version to "Chrome/7.0.517.0"

**August 31, 2010:** Revision 100 re-introduces the CefHandler::HandleJSBinding method.  The memory leaks that necessitated the elimination of this method have now been fixed (issue #72).

  * The CefRequest argument to CefHandler::HandleBeforeResourceLoad can now be modified allowing the request to be changed on the fly (issue #41).
  * A default tooltip implementation is now provided and tooltip text can be modified using CefHandler::HandleTooltip (issue #61).
  * Printing paper size, orientation and margins can now be changed using CefHandler::HandlePrintOptions (issue #112).
  * Find in page with result highlighting is now supported using CefBrowser::Find, CefBrowser::StopFinding and CefHandler::HandleFindResult.
  * The binary release of revision 100 provides libraries for both Visual Studio 2005 and Visual Studio 2008. It can be downloaded from the project Downloads page.

**April 7, 2010:** Revision 73 eliminates the CefHandler::HandleJSBinding method.  This modification addresses memory leaks that many users have been reporting.  For more information see issue #72.

**October 2, 2009:** Revision 50 adds [GYP support](http://code.google.com/p/gyp/) for generating the CEF project files. This makes it easy to build CEF with both VS2005 and VS2008. See the "Source Distributions" section above for additional details.

**August 21, 2009:** Revision 37 adds support for custom scheme handlers. Use the new CefRegisterScheme() function in combination with the CefSchemeHandlerFactory and CefSchemeHandler classes to create handlers for requests using custom schemes such as myscheme://mydomain.

**July 24, 2009:** Revision 32 helps to speed up the addition of new features and bug fixes to CEF. It adds the CEF Patch Application Tool and the "patch" project which together support automatic application of patches to the Chromium and WebKit source trees as part of the build process. See the README.txt file in the new patch directory for additional information.

**July 8, 2009:** CEF now has a dedicated build bot thanks to Nicolas Sylvain and Darin Fisher over at Google. The build bot synchronizes to each Chromium revision and then builds CEF, reporting on any compile errors that occur. Having a build bot for CEF will help the Chromium developers avoid accidentally breaking API features required by CEF, and help the CEF developers keep up with the frequently changing Chromium HEAD revision. You can view the build bot output at http://build.chromium.org/buildbot/waterfall.fyi/waterfall?branch=&builder=Chromium+Embedded

**June 20, 2009:** Revision 30 adds the CEF Translator Tool which facilitates automatic generation of the C API header file (cef\_capi.h) and CToCpp/CppToC wrapper classes. See the translator.README.txt file in the new tools directory for additional information.  Introduction of this tool required minor changes to the CEF C++ and C APIs.

  * The C++ 'arguments' attribute of CefV8Handler::Execute() and CefV8Value::ExecuteFunction() now has the 'const' qualifier.
  * C API global function names that were previously in the cef\_create\_classname() format are now in the cef\_classname\_create() format.
  * The C API cef\_frame\_t::get\_frame\_names() member function now has return type 'void' instead of 'size\_t'.
  * The C API cef\_frame\_t::execute\_javascript() member function has been renamed to cef\_frame\_t::execute\_java\_script().

**May 27, 2009:** Revision 26 introduces two major changes to the CEF framework.

  * Frame-dependent functions such as loading content and executing JavaScript have been moved to a new CefFrame class.  Use the new CefBrowser::Get\*Frame() methods to retrieve the appropriate CefFrame instance.  A CefFrame instance will now also be passed to CefHandler callback methods as appropriate.
  * The CEF JavaScript API now uses the V8 framework directly instead of creating NPObjects.  JavaScript object hierarchies can be written in native C++ code and exported to the JavaScript user-space, and user-space object hierarchies can be accessed from native C++ code. Furthermore, support for the V8 extension framework has been added via the new CefRegisterExtension() function. The CefJSHandler and CefVariant classes have been removed in favor of new CefV8Handler and CefV8Value classes. To attach values to the JavaScript 'window' object you must now implement the CefHandler::HandleJSBinding() callback method instead of calling the (now removed) CefBrowser::AddJSHandler() method.

This Wiki page explains how to create a simple application using CEF3.

**Note to Editors: Changes made to this Wiki page without prior approval via the [CEF Forum](http://magpcss.org/ceforum/) or [Issue Tracker](https://github.com/chromiumembedded/cef/issues) may be lost or reverted.**

***
[TOC]
***

# Introduction

This tutorial explains how to create a simple application using CEF3. It references the [cefsimple example project](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/?at=master). For complete CEF3 usage information visit the [GeneralUsage](GeneralUsage.md) Wiki page.

# Getting Started

CEF provides a sample project that makes it really easy to get started with CEF development. Simply browse over to the [cef-project](https://bitbucket.org/chromiumembedded/cef-project) website and follow the step-by-step instructions. The source files linked from this tutorial are for the current CEF3 master branch and may differ slightly from the versions that are downloaded by cef-project.

## Loading a Custom URL

The cefsimple application loads google.com by default but you can change it to load a custom URL instead. The easiest way to load a different URL is via the command-line.

```
# Load the local file “c:\example\example.html”
cefsimple.exe --url=file://c:/example/example.html
```

You can also edit the source code in [cefsimple/simple\_app.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_app.cc?at=master) and recompile the application to load your custom URL by default.

```
// Load the local file “c:\example\example.html”
…
if (url.empty())
  url = "file://c:/example/example.html";
…
```

# Application Components

All CEF applications have the following primary components:

  1. The CEF dynamic library (libcef.dll on Windows, libcef.so on Linux, “Chromium Embedded Framework.framework” on OS X).
  1. Support files (\*.pak and \*.bin binary blobs, etc).
  1. Resources (html/js/css for built-in features, strings, etc).
  1. Client executable (cefsimple in this example).

The CEF dynamic library, support files and resources will be the same for every CEF-based application. They are included in the Debug/Release or Resources directory of the binary distribution. See the README.txt file included in the binary distribution for details on which of these files are required and which can be safely left out. See below for a detailed description of the required application layout on each platform.

# Architecture in 60 Seconds

The below list summarizes the items of primary importance for this tutorial:

  * CEF uses multiple processes. The main application process is called the “browser” process. Sub-processes will be created for renderers, plugins, GPU, etc.
  * On Windows and Linux the same executable can be used for the main process and sub-processes. On OS X you are required to create a separate executable and app bundle for sub-processes.
  * Most processes in CEF have multiple threads. CEF provides functions and interfaces for posting tasks between these various threads.
  * Some callbacks and functions may only be used in particular processes or on particular threads. Make sure you read the source code comments in the API headers before you begin using a new callback or function for the first time.

Read the [GeneralUsage](GeneralUsage.md) Wiki page for complete discussion of the above points.

# Source Code

The cefsimple application initializes CEF and creates a single popup browser window. The application terminates when all browser windows have been closed. Program flow is as follows:

  1. The OS executes the browser process entry point function (main or wWinMain).
  1. The entry point function:
    1. Creates an instance of SimpleApp which handles process-level callbacks.
    1. Initializes CEF and runs the CEF message loop.
  1. After initialization CEF calls SimpleApp::OnContextInitialized(). This method:
    1. Creates the singleton instance of SimpleHandler.
    1. Creates a browser window using CefBrowserHost::CreateBrowser().
  1. All browsers share the SimpleHandler instance which is responsible for customizing browser behavior and handling browser-related callbacks (life span, loading state, title display, etc).
  1. When a browser window is closed SimpleHandler::OnBeforeClose() is called. When all browser windows have closed the OnBeforeClose implementation quits the CEF message loop to exit the application.

Your binary distribution may include newer versions of the below files. However, the general concepts remain unchanged.

## Entry Point Function

Execution begins in the browser process entry point function. This function is responsible for initializing CEF and any OS-related objects. For example, it installs X11 error handlers on Linux and allocates the necessary Cocoa objects on OS X. OS X has a separate entry point function for helper processes.

  * Windows platform implementation: [cefsimple/cefsimple\_win.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/cefsimple_win.cc?at=master)
  * Linux platform implementation: [cefsimple/cefsimple\_linux.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/cefsimple_linux.cc?at=master)
  * Mac OS X platform implementation
    * For the browser process: [cefsimple/cefsimple\_mac.mm](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/cefsimple_mac.mm?at=master)
    * For sub-processes: [cefsimple/process\_helper\_mac.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/process_helper_mac.cc?at=master)

## SimpleApp

SimpleApp is responsible for handling process-level callbacks. It exposes some interfaces/methods that are shared by multiple processes and some that are only called in a particular process. The CefBrowserProcessHandler interface, for example, is only called in the browser process. There’s a separate CefRenderProcessHandler interface (not shown in this example) that is only called in the render process. Note that GetBrowserProcessHandler() must return |this| because SimpleApp implements both CefApp and CefBrowserProcessHandler. See the [GeneralUsage](GeneralUsage.md) Wiki page or API header files for additional information on CefApp and related interfaces.

  * Shared implementation: [cefsimple/simple\_app.h](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_app.h?at=master), [cefsimple/simple\_app.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_app.cc?at=master)

## SimpleHandler

SimpleHandler is responsible for handling browser-level callbacks. These callbacks are executed in the browser process. In this example we use the same CefClient instance for all browsers, but your application can use different CefClient instances as appropriate. See the [GeneralUsage](GeneralUsage.md) Wiki page or API header files for additional information on CefClient and related interfaces.

  * Shared implementation: [cefsimple/simple\_handler.h](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_handler.h?at=master), [cefsimple/simple\_handler.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_handler.cc?at=master)
  * Windows platform implementation: [cefsimple/simple\_handler\_win.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_handler_win.cc?at=master)
  * Linux platform implementation: [cefsimple/simple\_handler\_linux.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_handler_linux.cc?at=master)
  * Mac OS X platform implementation: [cefsimple/simple\_handler\_mac.mm](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_handler_mac.mm?at=master)

# Build Steps

Build steps vary depending on the platform. Explore the CMake files included with the binary distribution for a complete understanding of all required steps. The build steps common to all platforms can generally be summarized as follows:

  1. Compile the libcef\_dll\_wrapper static library.
  1. Compile the application source code files. Link against the libcef dynamic library and the libcef\_dll\_wrapper static library.
  1. Copy libraries and resources to the output directory.

## Windows Build Steps

  1. Compile the libcef\_dll\_wrapper static library.
  1. Compile/link cefsimple.exe.
    * Required source code files include: cefsimple\_win.cc, simple\_app.cc, simple\_handler.cc, simple\_handler\_win.cc.
    * Required link libraries include: comctl32.lib, shlwapi.lib, rcprt4.lib, libcef\_dll\_wrapper.lib, libcef.lib, cef\_sandbox.lib. Note that cef\_sandbox.lib (required for sandbox support) is a static library currently built with Visual Studio 2022 and it may not compile with other Visual Studio versions. See comments in cefsimple\_win.cc for how to disable sandbox support.
    * Resource file is cefsimple.rc.
    * Manifest files are cefsimple.exe.manifest and compatibility.manifest.
  1. Copy all files from the Resources directory to the output directory.
  1. Copy all files from the Debug/Release directory to the output directory.

The resulting directory structure looks like this for 2526 branch:

```
Application/
    cefsimple.exe  <= cefsimple application executable
    libcef.dll <= main CEF library
    icudtl.dat <= unicode support data
    libEGL.dll, libGLESv2.dll, ... <= accelerated compositing support libraries
    cef.pak, devtools_resources.pak, ... <= non-localized resources and strings
    natives_blob.bin, snapshot_blob.bin <= V8 initial snapshot
    locales/
        en-US.pak, ... <= locale-specific resources and strings
```

## Linux Build Steps

  1. Compile the libcef\_dll\_wrapper static library.
  1. Compile/link cefsimple.
    * Required source code files include: cefsimple\_linux.cc, simple\_app.cc, simple\_handler.cc, simple\_handler\_linux.cc.
    * Required link libraries include: libcef\_dll\_wrapper.a, libcef.so and dependencies (identified at build time using the “pkg-config” tool).
    * Configure the rpath to find libcef.so in the current directory (“-Wl,-rpath,.”) or use the LD\_LIBRARY\_PATH environment variable.
  1. Copy all files from the Resources directory to the output directory.
  1. Copy all files from the Debug/Release directory to the output directory.
  1. Set SUID permissions on the chrome-sandbox executable to support the sandbox. See binary distribution build output for the necessary command.

The resulting directory structure looks like this for 2526 branch:

```
Application/
    cefsimple <= cefsimple application executable
    chrome-sandbox <= sandbox support binary
    libcef.so <= main CEF library
    icudtl.dat <= unicode support data
    cef.pak, devtools_resources.pak, ... <= non-localized resources and strings
    natives_blob.bin, snapshot_blob.bin <= V8 initial snapshot
    locales/
        en-US.pak, ... <= locale-specific resources and strings
    files/
        binding.html, ... <= cefclient application resources
```

## Mac OS X Build Steps

  1. Compile the libcef\_dll\_wrapper static library.
  1. Compile/link/package the “cefsimple Helper” app.
    * Required source code files include: process\_helper\_mac.cc.
    * Required link frameworks include: AppKit.framework.
    * App bundle configuration is provided via “cefsimple/mac/helper-Info.plist”.
    * Load the CEF Framework as described [here](https://groups.google.com/d/msg/cef-announce/Fith0A3kWtw/6ds_mJVMCQAJ).
  1. Compile/link/package the “cefsimple” app.
    * Required source code files include: cefsimple\_mac.mm, simple\_app.cc, simple\_handler.cc, simple\_handler\_mac.mm.
    * Required link frameworks include: AppKit.framework.
    * App bundle configuration is provided via “cefsimple/mac/Info.plist”.
    * Load the CEF Framework as described [here](https://groups.google.com/d/msg/cef-announce/Fith0A3kWtw/6ds_mJVMCQAJ).
  1. Create a Contents/Frameworks directory in the cefsimple.app bundle. Copy the following files to that directory: “cefsimple Helper.app”, “Chromium Embedded Framework.framework”.

The resulting directory structure looks like this for 2526 branch:

```
cefsimple.app/
    Contents/
        Frameworks/
            Chromium Embedded Framework.framework/
                Chromium Embedded Framework <= main application library
                Resources/
                    cef.pak, devtools_resources.pak, ... <= non-localized resources and strings
                    icudtl.dat <= unicode support data
                    natives_blob.bin, snapshot_blob.bin <= V8 initial snapshot
                    en.lproj/, ... <= locale-specific resources and strings
            cefsimple Helper.app/
                Contents/
                    Info.plist
                    MacOS/
                        cefsimple Helper <= helper executable
                    Pkginfo
        Info.plist
        MacOS/
            cefsimple <= cefsimple application executable
        Pkginfo
        Resources/
            cefsimple.icns, ... <= cefsimple application resources
```

This Wiki page describes the setup of an automated build system for CEF.

**Note to Editors: Changes made to this Wiki page without prior approval via the [CEF Forum](http://magpcss.org/ceforum/) or [Issue Tracker](https://github.com/chromiumembedded/cef/issues) may be lost or reverted.**

***
[TOC]
***

# Overview

CEF provides tools for automatically downloading, building and packaging Chromium and CEF source code. These tools are the recommended way of building CEF locally and can also be integrated with an automated build system. Examples of automated build systems for CEF include https://cef-builds.spotifycdn.com/index.html hosted by Spotify. This document outlines how to build a system similar to Spotify's. For general CEF build instructions see the [BranchesAndBuilding](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md) Wiki page.

# Services

An automated build system tracks changes to a CEF branch, automatically builds a new version of CEF when changes are detected, and uploads the resulting artifacts (CEF binary files) to an Internet or Intranet Website.

The following high-level services are necessary for an automated build system:

1. Build execution service. Used to perform the CEF automated builds using agents running on different OS platforms. For example, a build system like [TeamCity](http://www.jetbrains.com/teamcity/) or [Jenkins](http://jenkins-ci.org/).
2. Artifact storage service. Used to store artifacts referenced and generated by the build agents. For example, a shared file system or a storage system like [Artifactory](https://www.jfrog.com/artifactory/).
3. Artifact publishing service. Used to publish artifacts for download on the Intranet or Internet. Generally provides some form of distributed caching. For example, a cloud provider like [Amazon S3 + Cloudfront](http://docs.aws.amazon.com/AmazonCloudFront/latest/DeveloperGuide/MigrateS3ToCloudFront.html) or [Google Cloud Storage](https://cloud.google.com/storage/).

Setup of these specific services is outside the scope of this document. However, we will refer to the capabilities of these services in generic terms.

# Execution Steps

The reference build system works by executing the following steps in order:

## 1\. Change Trigger

Runs automatically at some interval (for example, once daily) to check if a new change has been committed to the CEF repository. The [Bitbucket REST API](https://confluence.atlassian.com/bitbucket/commits-or-commit-resource-389775478.html) exposes an endpoint that can be queried to identify the most recent commit in a branch. For example, this will return the most recent commit for 2785 branch:

https://api.bitbucket.org/2.0/repositories/chromiumembedded/cef/commit/2785

The resulting "hash" value can then be compared to the last successful CEF build version to identify if a new build is required (e.g. a hash value of "1e15b7626244fa6c6455b7a24552664e4cc14fd3" from branch 2785 will match a build artifact file named "cef_binary_3.2785.\*.g1e15b76_\*.tar.bz2").

If artifacts are stored on a shared file system then languages like Python can access the file list directly. If artifacts are stored in Artifactory then see the [Artifactory REST API](https://www.jfrog.com/confluence/display/RTF/Artifactory+REST+API) documentation for instructions on how to retrieve a directory listing.

## 2\. Platform Builder

Initiated by the change trigger when a new commit is detected. A separate platform builder is configured for each supported build platform (linux32, linux64, macosx64, windows32, windows64). It executes a build on that platform and then uploads the resulting build artifact files to the storage service (shared file system, Artifactory, etc.).

See the "Platform Build Configurations" section below for recommended platform build configurations.

## 3\. Artifact Publisher

Initiated by the platform builder upon a successful build. It retrieves the list of build artifact files from the storage service and uses the [cefbuilds](https://bitbucket.org/chromiumembedded/cef/src/master/tools/cefbuilds/?at=master) tools to compare against the index.json file generated by the previous run. When changes are detected it uploads the new changes to the publishing service (Amazon S3, GCS, etc.). The new index.json file is then stored for future reference.

In addition to generating [index.json](https://cef-builds.spotifycdn.com/index.json) files the [cefbuilds](https://bitbucket.org/chromiumembedded/cef/src/master/tools/cefbuilds/?at=master) tools can also generate the [index.html](https://cef-builds.spotifycdn.com/index.html) file used by the Spotify builder (provide your own custom branding to cef_html_builder.py via the "branding" parameter). See the cef_json_builder_example.py and cef_html_builder.py files from the cefbuilds directory for sample usage.

# Platform Build Configurations

The following configurations are executed by the platform builder step. These are the recommended configurations for externally published artifacts but are not recommended for local builds. See the the [BranchesAndBuilding](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md) Wiki page for general build instructions.

These instructions are intended for use with the [currently supported release branches](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md#markdown-header-current-release-branches-supported). This Wiki page will be updated as build requirements change.

Lower-case values between %% (%download_dir%, %cef_branch%, etc.) must be provided by you.

## Linux Configuration

**What's Required**

- Linux Build Requirements as listed on the [BranchesAndBuilding](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md#markdown-header-current-release-branches-supported) Wiki page.
- At least 16GB of RAM and 200GB of free disk space.

We recommend building on Linux using a [Docker container](https://www.docker.com/resources/what-container). All architectures can be built using the same Linux container image with a configuration [as shown here](https://magpcss.org/ceforum/viewtopic.php?f=7&t=17776&p=46448#p54107) that utilizes Chromium's [install-build-deps.py](https://source.chromium.org/chromium/chromium/src/+/master:build/install-build-deps.py) script.

**64-bit Build Commands**

To build 64-bit CEF with a 64-bit Linux Docker image:

```
export GN_DEFINES="is_official_build=true use_sysroot=true symbol_level=1 is_cfi=false"
export CEF_ARCHIVE_FORMAT=tar.bz2
python3 automate-git.py --download-dir=%download_dir% --branch=%cef_branch% --minimal-distrib --client-distrib --force-clean --build-target=cefsimple --x64-build --with-pgo-profiles
```

**ARM Build Commands**

To build ARM CEF with a 64-bit Linux Docker image:

```
export CEF_INSTALL_SYSROOT=arm
export GN_DEFINES="is_official_build=true use_sysroot=true symbol_level=1 is_cfi=false use_thin_lto=false chrome_pgo_phase=0 use_vaapi=false"
export CEF_ARCHIVE_FORMAT=tar.bz2
python3 automate-git.py --download-dir=%download_dir% --branch=%cef_branch% --minimal-distrib --client-distrib --force-clean --build-target=cefsimple --arm-build
```

**ARM64 Build Commands**

To build ARM64 CEF with a 64-bit Linux Docker image:

```
export CEF_INSTALL_SYSROOT=arm64
export GN_DEFINES="is_official_build=true use_sysroot=true symbol_level=1 is_cfi=false use_thin_lto=false chrome_pgo_phase=0"
export CEF_ARCHIVE_FORMAT=tar.bz2
python3 automate-git.py --download-dir=%download_dir% --branch=%cef_branch% --minimal-distrib --client-distrib --force-clean --build-target=cefsimple --arm64-build
```

## MacOS Configuration

**What's Required**

- MacOS Build Requirements as listed on the [BranchesAndBuilding](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md#markdown-header-current-release-branches-supported) Wiki page.
- At least 16GB of RAM and 200GB of free disk space.

If Xcode is not installed to the default location then set the correct location using the `xcodeselect` tool.

**64-bit Build Commands**

To build Intel 64-bit CEF on a Intel 64-bit MacOS host system:

```
export GN_DEFINES=is_official_build=true
export CEF_ARCHIVE_FORMAT=tar.bz2
python3 automate-git.py --download-dir=%download_dir% --branch=%cef_branch% --minimal-distrib --client-distrib --force-clean --x64-build --with-pgo-profiles
```

To build Intel 64-bit CEF on a ARM64 MacOS host system:

```
export GN_DEFINES=is_official_build=true
export CEF_ARCHIVE_FORMAT=tar.bz2
export CEF_ENABLE_AMD64=1
python3 automate-git.py --download-dir=%download_dir% --branch=%cef_branch% --minimal-distrib --client-distrib --force-clean --x64-build --with-pgo-profiles
```

**ARM64 Build Commands**

ARM64 builds require Xcode 12.2 and CEF branch 4280 or newer.

To build ARM64 CEF on an ARM64 MacOS host system:

```
export GN_DEFINES=is_official_build=true
export CEF_ARCHIVE_FORMAT=tar.bz2
python3 automate-git.py --download-dir=%download_dir% --branch=%cef_branch% --minimal-distrib --client-distrib --force-clean --arm64-build --with-pgo-profiles
```

To build ARM64 CEF on a Intel 64-bit MacOS host system:

```
export GN_DEFINES=is_official_build=true
export CEF_ARCHIVE_FORMAT=tar.bz2
export CEF_ENABLE_ARM64=1
python3 automate-git.py --download-dir=%download_dir% --branch=%cef_branch% --minimal-distrib --client-distrib --force-clean --arm64-build --with-pgo-profiles
```

## Windows Configuration

**What's Required**

- Visual Studio and Windows SDK versions as listed on the [BranchesAndBuilding](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md#markdown-header-current-release-branches-supported) Wiki page.
- At least 16GB of RAM and 200GB of free disk space.

Install the required Visual Studio sub-components by passing the following arguments to the Visual Studio installer:

```
$ PATH_TO_INSTALLER.EXE ^
--add Microsoft.VisualStudio.Workload.NativeDesktop ^
--add Microsoft.VisualStudio.Component.VC.ATLMFC ^
--includeRecommended
```

If building for ARM64 install the following additional sub-components:

```
$ PATH_TO_INSTALLER.EXE ^
--add Microsoft.VisualStudio.Component.VC.Tools.ARM64 ^
--add Microsoft.VisualStudio.Component.VC.MFC.ARM64 ^
--includeRecommended
```

If Visual Studio or Windows SDK is not installed to the default location then set environment variables like the following before executing automate-git.py:

```
set WIN_CUSTOM_TOOLCHAIN=1
set CEF_VCVARS=none
set GYP_MSVS_OVERRIDE_PATH=%vs_root%
set VS_CRT_ROOT=%vs_crt_root%
set SDK_ROOT=%sdk_root%
set PATH=%sdk_root%\bin\%sdk_version%\x64;%vs_root%\VC\Tools\MSVC\%vc_tools_version%\bin\HostX64\x64;%vs_root%\VC\Redist\MSVC\%vc_redist_version%\x64\%vc_redist_crt%;%vs_root%\SystemCRT;%PATH%
set LIB=%sdk_root%\Lib\%sdk_version\um\%arch%;%sdk_root%\Lib\%sdk_version\ucrt\%arch%;%vs_root%\VC\Tools\MSVC\%vc_tools_version%\lib\%arch%;%vs_root%\VC\Tools\MSVC\%vc_tools_version%\atlmfc\lib\%arch%;%LIB%
set INCLUDE=%sdk_root%\Include\%sdk_version%\um;%sdk_root%\Include\%sdk_version%\ucrt;%sdk_root%\Include\%sdk_version%\shared;%vs_root%\VC\Tools\MSVC\%vc_tools_version%\include;%vs_root%\VC\Tools\MSVC\%vc_tools_version%\atlmfc\include;%INCLUDE%
```

The correct `PATH`, `LIB` and `INCLUDE` values for your installation can be determined by running the `vcvarsall.bat` script and passing in the desired architecture and SDK version.

See comments in [gclient_hook.py](https://bitbucket.org/chromiumembedded/cef/src/2785/tools/gclient_hook.py?at=2785&fileviewer=file-view-default#gclient_hook.py-54) for Windows custom toolchain requirements. VS + SDK can be packaged for distribution to build agents using a script like Chromium's [package_from_installed.py](https://source.chromium.org/chromium/chromium/tools/depot_tools/+/main:win_toolchain/package_from_installed.py).

**32-bit Build Commands**

To build 32-bit CEF on a 64-bit Windows host system:

```
set GN_DEFINES=is_official_build=true
set GYP_MSVS_VERSION=2022
set CEF_ARCHIVE_FORMAT=tar.bz2
python3 automate-git.py --download-dir=%download_dir% --branch=%cef_branch% --minimal-distrib --client-distrib --force-clean --with-pgo-profiles
```

**64-bit Build Commands**

To build 64-bit CEF on a 64-bit Windows host system:

```
set GN_DEFINES=is_official_build=true
set GYP_MSVS_VERSION=2022
set CEF_ARCHIVE_FORMAT=tar.bz2
python3 automate-git.py --download-dir=%download_dir% --branch=%cef_branch% --minimal-distrib --client-distrib --force-clean --x64-build --with-pgo-profiles
```

**ARM64 Build Commands**

To build ARM64 CEF on a 64-bit Windows host system:

```
set CEF_ENABLE_ARM64=1
set GN_DEFINES=is_official_build=true use_thin_lto=false chrome_pgo_phase=0
set GYP_MSVS_VERSION=2022
set CEF_ARCHIVE_FORMAT=tar.bz2
python3 automate-git.py --download-dir=%download_dir% --branch=%cef_branch% --minimal-distrib --client-distrib --force-clean --build-target=cefsimple --arm64-build
```

This Wiki page describes how to update CEF to use the newest Chromium revision.

**Note to Editors: Changes made to this Wiki page without prior approval via the [CEF Forum](http://magpcss.org/ceforum/) or [Issue Tracker](https://github.com/chromiumembedded/cef/issues) may be lost or reverted.**

***
[TOC]
***

# Overview

The Chromium developers work very hard to introduce new features and capabilities as quickly as possible. Consequently, projects like CEF that depend on Chromium must also be updated regularly. The update process can be complicated and must be done carefully to avoid introducing new bugs and breakage. Below are the steps to follow when updating CEF to work with a new Chromium revision.

# Recommended workflow

## 1\. Setup a local developer checkout

Setup a local developer checkout of the CEF/Chromium master branch as described on the [MasterBuildQuickStart](https://bitbucket.org/chromiumembedded/cef/wiki/MasterBuildQuickStart.md) Wiki page. The standard `automate-git.py` parameters discussed on that Wiki page are represented by the `[...]` placeholder in the below examples.

## 2\. Check for changes in Chromium's build requirements

Chromium's build requirements change over time. You may need to update your build environment before proceeding.

* **Windows**: Visual Studio and Windows SDK requirements are listed [here](https://chromium.googlesource.com/chromium/src/+/HEAD/docs/windows_build_instructions.md#Setting-up-Windows) and [here](https://source.chromium.org/chromium/chromium/src/+/main:build/vs_toolchain.py;l=20;bpv=0). Visual Studio bootstrap installers for specific versions can be found [here](https://learn.microsoft.com/en-us/visualstudio/releases/2022/release-history#fixed-version-bootstrappers).
* **MacOS**: SDK and Xcode version requirements are listed [here](https://source.chromium.org/chromium/chromium/src/+/main:build/config/mac/mac_sdk.gni;l=43?q=mac_sdk_official_version&ss=chromium) and [here](https://source.chromium.org/chromium/chromium/src/+/main:build/mac_toolchain.py;l=44?q=MAC_BINARIES_LABEL&ss=chromium%2Fchromium%2Fsrc). SDK versions can also be mapped to Xcode versions using [this table](https://en.wikipedia.org/wiki/Xcode#Xcode_11.x_-_13.x_(since_SwiftUI_framework)).
* **Linux**: Version and distro requirements are listed [here](https://chromium.googlesource.com/chromium/src/+/main/docs/linux/build_instructions.md#System-requirements). Most dependency version issues can be avoided by building with a [sysroot image](https://chromium.googlesource.com/chromium/src/+/main/docs/linux/sysroot.md) by setting `GN_DEFINES=use_sysroot=true`.

If the build requirements have changed the [MasterBuildQuickStart Wiki page](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md#markdown-header-development) will also need to be updated.

## 3\. Start the update

### A\. Identify the target Chromium version

The Chromium [release cycle](https://chromium.googlesource.com/chromium/src/+/master/docs/process/release_cycle.md) utilizes milestone versions (M98, M99, etc.) and [release channels](https://www.chromium.org/getting-involved/dev-channel/) (canary, beta, stable). New milestone versions branch approximately every 4 weeks and begin life in the canary channel. Over the next 4 weeks they progress through the beta and stable channels to reach Chrome's ~3 billion users globally.

The CEF project has a `master` branch for ongoing development and creates a [release branch](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md#markdown-header-release-branches) that tracks each milestone version. Automated builds are provided for [currently supported](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md#markdown-header-current-release-branches-supported) versions.

* **CEF master updates** target new milestone versions as they are announced to the chromium-dev mailing list. For example, [this email](https://groups.google.com/a/chromium.org/g/chromium-dev/c/uuVm_MltNY4/m/6GO-cpseAQAJ) was sent for the M100 branch. These versions should have 0 as the last component (e.g. "100.0.4896.0"). Updates are performed manually and usually take around 1 week to complete. The "Branch" date for upcoming milestones can be found on the [Chromium schedule](https://chromiumdash.appspot.com/schedule).
* **CEF beta and stable channel updates** use the version information published [here](https://chromiumdash.appspot.com/releases?platform=Windows). These versions should not have 0 as the last component (e.g. "99.0.4844.45"). Updates are performed automatically in most cases and a PR will be submitted to the public repository if the build succeeds.

To manually specify the Chromium version and update the checkout of master:

```
python automate-git.py [...] --no-build --fast-update --log-chromium-changes --chromium-checkout=refs/tags/100.0.4896.0
```

To manually specify the Chromium version and update the checkout of an existing release branch:

```
python automate-git.py [...] --no-build --fast-update --force-cef-update --branch=4844 --chromium-checkout=refs/tags/99.0.4844.45
```

### B\. Run CEF's `patch_updater.py` script

This will update the Chromium patch files in the `patch/patches` directory. If patch files cannot be updated automatically the `automate-git.py` script will fail.

**If one or more patch files have unresolved conflicts you will get a failure like the following:**

```
--------------------------------------------------------------------------------
!!!! FAILED PATCHES, fix manually and run with --resave
content_2015:
  Hunk #4 FAILED at 4220.
  1 out of 5 hunks FAILED -- saving rejects to file content/browser/frame_host/render_frame_host_impl.cc.rej
storage_partition_1973:
  Hunk #1 FAILED at 167.
  1 out of 1 hunk FAILED -- saving rejects to file content/browser/shared_worker/shared_worker_service_impl.cc.rej
--------------------------------------------------------------------------------
```

Use a text editor to manually fix the specified files, and then re-run the `patch_updater.py` script:

```
$ cd /path/to/chromium_git/chromium/src/cef/tools
$ python patch_updater.py
```

**If a patched file is missing (moved or deleted) you will get a failure like the following:**

```
--> Reading patch file /path/to/chromium_git/chromium/src/cef/patch/patches/content_mojo_3123.patch
--> Skipping non-existing file /path/to/chromium_git/chromium/src/content/public/browser/document_service_base.h
--> Applying patch to /path/to/chromium_git/chromium/src
(Stripping trailing CRs from patch; use --binary to disable.)
can't find file to patch at input line 5
...
Skip this patch? [y] 
Skipping patch.
2 out of 2 hunks ignored
--> Saving changes to /path/to/chromium_git/chromium/src/cef/patch/patches/content_mojo_3123.patch
Traceback (most recent call last):
  File "/path/to/chromium_git/chromium/src/cef/tools/patch_updater.py", line 294, in <module>
    raise Exception('Failed to add paths: %s' % result['err'])
Exception: Failed to add paths: fatal: pathspec '/path/to/chromium_git/chromium/src/content/public/browser/document_service_base.h' did not match any files

```

You can use this Git command to discover what happened to the missing Chromium file:

```
$ cd /path/to/chromium_git/chromium/src
$ git log --full-history -1 -- content/public/browser/document_service_base.h
```

Once you know the offending Git commit hash you can use the `git show <hash>` command or load `https://crrev.com/<hash>` in a web browser to see the contents of the change. Edit the patch file manually in a text editor to fix the paths and then re-run the `patch_updater.py` script as shown above.

**To create a new patch file use this command:**

```
$ cd /path/to/chromium_git/chromium/src
$ git diff --no-prefix --relative [path1] [path2] > [name].patch
```

Copy the resulting `[name].patch` file to the `src/cef/patch/patches` directory and add an appropriate entry to the `src/cef/patch/patch.cfg` file.

**To add additional files to an existing patch file use this command:**

```
$ cd /path/to/chromium_git/chromium/src/cef/tools
$ python patch_updater.py --resave --patch [name] --add [path1] --add [path2]
```

All paths are relative to the `src` directory by default.

**After all patch files have been fixed you can re-run `automate-git.py` with the additional `--resave` command-line flag to resave the patch files and continue the update process:**

```
python automate-git.py [...] --resave
```

### C\. Identify potentially relevant Chromium changes

This step creates a `chromium_update_changes.diff` file in your download directory that will act as your guide when updating the CEF source code. CEF began life as a customized version of content\_shell and there's still a one-to-one relationship between many of the files. The list of relevant paths is taken from CEF's [CHROMIUM_UPDATE.txt](https://bitbucket.org/chromiumembedded/cef/src/master/CHROMIUM_UPDATE.txt) file.

### D\. Identify problematic patterns in the Chromium source code

This step looks for patterns in Chromium src files that may cause issues for CEF. If these patterns are found then the `automate-git.py` script will create a `chromium_update_patterns.txt` file in your download directory and fail with output like the following:

```
Evaluating pattern: static_cast<StoragePartitionImpl\*>(
ERROR Matches found. See chromium_update_changes.diff for output.
```

In that case the contents of `chromium_update_patterns.txt` might look like this:

```
!!!! WARNING: FOUND PATTERN: static_cast<StoragePartitionImpl\*>(
New instances in non-test files should be converted to call StoragePartition methods.
See storage_partition_1973.patch.

content/browser/loader/navigation_url_loader_impl.cc:1295:  auto* partition = static_cast<StoragePartitionImpl*>(storage_partition);
content/browser/renderer_interface_binders.cc:189:        static_cast<StoragePartitionImpl*>(host->GetStoragePartition())
```

Use the process described in part B to fix these failures.

## 4\. Build CEF and fix whatever else is broken

The diff file created in step C above will assist you in this process. Refer to the [MasterBuildQuickStart](https://bitbucket.org/chromiumembedded/cef/wiki/MasterBuildQuickStart.md) Wiki page for build instructions. If Chromium changes are required refer to the "Resolving Chromium breakage" section below.

## 5\. Run CEF tests

After updating Chromium you should run some tests to verify that everything is working as expected. The tests should eventually be run on all supported platforms.

A\. The `cefclient` sample application supports various runtime modes that should be tested for startup/shutdown crashes or strange runtime behaviors (on MacOS use `open cefclient.app --args <args>`):

```
# Chrome bootstrap:
$ cefclient

# Chrome bootstrap + Alloy style:
$ cefclient --use-alloy-style
$ cefclient --use-alloy-style --use-native
$ cefclient --off-screen-rendering-enabled

# Multi-threaded message loop (Windows/Linux only):
$ cefclient --multi-threaded-message-loop
$ cefclient --use-alloy-style --multi-threaded-message-loop

# Chrome style + native parent (Windows/Linux only):
$ cefclient --use-native
```

B\. Run the various manual tests available via the Tests menu in `cefclient` to verify that everything still works. If any particular subsystem had significant changes (like printing, for example) then make sure to give that subsystem additional testing.

C\. Unit tests included in the `ceftests` application are expected to pass on all platforms, except as follows:

* Known flaky tests are tracked in [this list](https://github.com/chromiumembedded/cef/issues?q=is%3Aissue+is%3Aopen+label%3Atests).
* Missing Chrome runtime functionality is tracked in [this list](https://github.com/chromiumembedded/cef/issues?q=is%3Aissue+is%3Aopen+label%3Achrome).
* MacOS Views test support is tracked in [issue #3188](https://github.com/chromiumembedded/cef/issues/3188).

To run unit tests on Windows/Linux:

```
$ ceftests --use-views --gtest_filter=-AudioOutputTest.*:DisplayTest.AutoResize:LifeSpanTest.*:OSRTest.*:URLRequestTest.*
$ ceftests --use-views --use-alloy-style --gtest_filter=-DisplayTest.AutoResize:OSRTest.AccessibilityLocationChange
```

To run unit tests on MacOS:

```
$ ./ceftests.app/Contents/MacOS/ceftests --use-views --gtest_filter=-AudioOutputTest.\*:DisplayTest.AutoResize:LifeSpanTest.\*:NavigationTest.PopupJSWindowOpen:OSRTest.\*:URLRequestTest.\*:ViewsWindowTest.WindowAccelerator
$ ./ceftests.app/Contents/MacOS/ceftests --use-views --use-alloy-style --gtest_filter=-DisplayTest.AutoResize:OSRTest.AccessibilityLocationChange
```

## 6\. Update the compatible Chromium version

Update the compatible Chromium version listed in CEF's [CHROMIUM_BUILD_COMPATIBILITY.txt](https://bitbucket.org/chromiumembedded/cef/src/master/CHROMIUM_BUILD_COMPATIBILITY.txt) file.

## 7\. Create and submit a PR

Create a commit with your changes and submit a PR as described on the [ContributingWithGit](https://bitbucket.org/chromiumembedded/cef/wiki/ContributingWithGit.md) Wiki page. See the [CEF commit history](https://bitbucket.org/chromiumembedded/cef/commits/branch/master) for examples of the expected commit message format.

## 8\. Test your changes on other supported platforms

You can update, build and run tests automatically on those platforms with the following command:

```
python automate-git.py [...] --run-tests --build-failure-limit=1000 --fast-update --force-cef-update
```

If running in headless mode on Linux install the `xvfb` package and add the `--test-prefix=xvfb-run --test-args="--no-sandbox"` command-line flags.

Access to automated builders as described on the [AutomatedBuildSetup](https://bitbucket.org/chromiumembedded/cef/wiki/AutomatedBuildSetup.md) Wiki page is recommended for this step.

# Resolving automate_git.py update failures

The automate_git.py script can sometimes fail to update an existing Chromium checkout due to errors while running Chromium tooling (git, gclient, etc). When that occurs you can perform the update steps manually.

The following example assumes the usual [file structure](https://bitbucket.org/chromiumembedded/cef/wiki/MasterBuildQuickStart.md#markdown-header-file-structure) with `path/to/chromium_git` as your existing automate_git.py download directory.

```
# Check out the required Chromium version.
$ cd path/to/chromium_git/chromium/src
$ git checkout refs/tags/100.0.4896.0

# Copy the `cef` directory to `chromium/src/cef` if it doesn't already exist.
$ cd path/to/chromium_git
$ cp -R cef chromium/src/

# Windows only. Apply the patch for `runhooks`.
$ cd path/to/chromium_git/chromium/src/cef/tools
$ python patch_updater.py --reapply --patch runhooks

# Windows only. Don't expect VS to be installed via depot_tools.
$ set DEPOT_TOOLS_WIN_TOOLCHAIN=0

# Update Chromium third_party dependencies and execute runhooks.
$ cd path/to/chromium_git/chromium/src
$ gclient sync --with_branch_heads
```

You can then run `cef_create_projects.[bat|sh]` and `ninja` to build CEF in the usual way.

# Resolving Chromium breakage

In most cases (say, 90% of the time) any code breakage will be due to naming changes, minor code reorganization and/or project name/location changes. The remaining 10% can require pretty significant changes to CEF, usually due to the ongoing refactoring in Chromium code. If you identify a change to Chromium that has broken a required feature for CEF, and you can't work around the breakage by making reasonable changes to CEF, then you should work with the Chromium team to resolve the problem.

1\. Identify the specific Chromium revision that broke the feature and make sure you understand why the change was made.

2\. Post a message to the chromium-dev mailing list explaining why the change broke CEF and either seeking additional information or suggesting a fix that works for both CEF and Chromium.

3\. After feedback from the Chromium developers create an issue in the [Chromium issue tracker](https://crbug.com) and a [code review](http://www.chromium.org/developers/contributing-code) with the fix and publish it with the appropriate (responsible) developer(s) as reviewers.

4\. Follow through with the Chromium developer(s) to get the code review committed.

The CEF build currently contains a patch capability that should be used only as a last resort or as a stop-gap measure if you expect the code review to take a while. The best course of action is always to get your Chromium changes accepted into the Chromium trunk if possible.

This Wiki page provides information about using Git to contribute code changes to CEF.

**Note to Editors: Changes made to this Wiki page without prior approval via the [CEF Forum](http://magpcss.org/ceforum/) or [Issue Tracker](https://github.com/chromiumembedded/cef/issues) may be lost or reverted.**

***
[TOC]
***

# Overview

The CEF project uses the Git source code management system hosted via Bitbucket. The easiest way to contribute changes to CEF is by creating your own personal fork of the CEF Git repository and submitting pull requests with your proposed modifications. This document is intended as a quick-start guide for developers who already have some familiarity with Git. If you are completely new to Git you might want to review the series of [Git tutorials](https://www.atlassian.com/git/tutorials/) provided by Bitbucket.

# Initial Setup

Git can maintain your changes both locally and on a remote server. To work with Git efficiently the remote server locations must be configured properly.

1\. Log into Bitbucket and create a forked version of the cef.git repository: https://bitbucket.org/chromiumembedded/cef/fork.

2\. Check out CEF/Chromium source code as described on the [MasterBuildQuickStart](MasterBuildQuickStart.md) or [BranchesAndBuilding](BranchesAndBuilding.md) Wiki page. Note that if you use the automate-git.py tool you will want to back up your customized CEF checkout before changing branches or performing a clean build.

3\. Change the remote origin of the CEF checkout so that it points to your personal forked repository. This is the remote server location that the `git push` and `git pull` commands will operate on by default.

```
cd /path/to/chromium/src/cef

# Replace <UserName> with your Bitbucket user name.
git remote set-url origin https://<UserName>@bitbucket.org/<UserName>/cef.git
```

4\. Set the remote upstream of the CEF checkout so that you can merge changes from the main CEF repository.

```
git remote add upstream https://bitbucket.org/chromiumembedded/cef.git
```

5\. Verify that the remotes are configured correctly.

```
git remote -v
```

You should see output like the following:

```
origin  https://<UserName>@bitbucket.org/<UserName>/cef.git (fetch)
origin  https://<UserName>@bitbucket.org/<UserName>/cef.git (push)
upstream        https://bitbucket.org/chromiumembedded/cef.git (fetch)
upstream        https://bitbucket.org/chromiumembedded/cef.git (push)
```

6\. Configure your name and email address.

```
git config user.name "User Name"
git config user.email user@example.com
```

7\. Configure the correct handling of line endings in the repository.

```
# Use this value on all platforms. Files will be unchanged in the working
# directory and converted to LF line endings in the object database.
git config core.autocrlf input

# Cause Git to abort actions on files with mixed line endings if the change is
# not reversible (e.g. changes to binary files that are accidentally classified
# as text).
git config core.safecrlf true
```

# Working With Private Changes

You can now commit changes to your personal repository and merge upstream changes from the main CEF repository. To facilitate creation of a pull request or the sharing of your code changes with other developers you should make your changes in a branch.

## Creating a Branch

Create a new personal branch for your changes.

```
# Start with the branch that your changes will be based upon.
git checkout master

# Create a new personal branch for your changes.
# Replace <BranchName> with your new branch name.
git checkout -b <BranchName>
```

## Creating a Commit

After making local modifications you can commit them to your personal branch.

```
# For example, add a specified file by path.
git add path/to/file.txt

# For example, add all existing files that have been modified or deleted.
git add -u

# Commit the modifications locally.
git commit -m "A good description of the fix (issue #1234)"

# Push the modifications to your personal remote repository.
git push origin <BranchName>
```

## Modifying a Commit

You can also modify an existing commit if you need to make additional changes.

```
# For example, add all existing files that have been modified or deleted.
git add -u

# Update the current HEAD commit with the changes.
git commit --amend

# Push the modifications to your personal remote repository.
# Using the `--force` argument is not recommended if multiple people are sharing the
# same branch.
git push origin <BranchName> --force
```

## Rebasing on Upstream Changes

The main CEF repository will receive additional commits over time. You will want to include these changes in your personal repository. To keep Git history correct (showing upstream CEF commits on the CEF branch instead of on your personal branch) you will need to rebase the local CEF branch before rebasing your local personal branch.

```
# Fetch changes from the main CEF repository. This does not apply them to any
# particular branch.
git fetch upstream

# Check out the local CEF branch that tracks the upstream CEF branch.
# Replace "master" with a different branch name as appropriate (e.g. "2171", "2272", etc).
git checkout master

# Rebase your local CEF branch on top of the upstream CEF branch.
# After this command your local CEF branch should be identical to the upstream CEF branch.
git rebase upstream/master

# Check out the personal branch that you want to update with changes from the CEF branch.
# Replace <BranchName> with the name of your branch.
git checkout <BranchName>

# Rebase your personal branch on top of the local CEF branch.
# After this command your local commits will come after all CEF commits on the same branch.
git rebase master

# Push the modifications to your personal remote repository.
git push origin <BranchName>
```

You may get merge conflicts if your personal changes conflict with changes made to the main CEF respository. For instructions on resolving merge conflicts see [this articicle](https://help.github.com/articles/resolving-merge-conflicts-after-a-git-rebase/).

For more information on using the rebase command go [here](https://www.atlassian.com/git/tutorials/merging-vs-rebasing).

## Deleting a Branch

Once you no longer need a branch you can delete it both locally and remotely. Do not delete branches that are associated with open pull requests.

```
# Delete the branch locally.
git branch -D <BranchName>

# Delete the branch remotely.
git push origin --delete <BranchName>
```

## Cleaning a Checkout

You can remove all local changes from your checkout using the below commands.

```
# Check the current state of the repository before deleting anything.
git status

# Remove all non-committed files and directories from the local checkout.
# If you run this command with JCEF it will also remove all third_party directories and you will
# need to re-run the `gclient runhooks` command.
git clean -dffx

# Remove all local commits from the current branch and reset branch state to match 
# origin/master. Replace "origin/master" with a different remote branch name as appropriate.
git reset --hard origin/master
```

# Working With Pull Requests

Once your personal changes are complete you can request that they be merged into the main CEF (or JCEF) repository. This is done using a pull request. Before submitting a pull request you should:

* Rebase your changes on the upstream CEF (or JCEF) branch (see "Rebasing on Upstream Changes").
* Fix any coding style issues (see "Coding Style").
* Squash your changes into a single commit (see "Squashing Commits").
* Find or create an appropriate issue in the [CEF issue tracker](https://github.com/chromiumembedded/cef/issues) (or [JCEF issue tracker](https://github.com/chromiumembedded/java-cef/issues) if the change targets that project). Make sure the issue number is referenced in your commit description.
* Include new or modified unit tests as appropriate to the functionality.
* Remove unnecessary or unrelated changes.

## Coding Style

CEF uses the [Chromium coding style](https://chromium.googlesource.com/chromium/src/+/master/styleguide/styleguide.md). All C/C++, ObjC, Java and Python code must be formatted using the fix_style tool. For example, to fix the style of the unstaged files in your CEF Git checkout:

```
cd /path/to/chromium/src/cef
./tools/fix_style.sh unstaged
```

The fix_style tool supports file paths and git hashes as input. Run `tools/fix_style.[bat|sh]` without arguments for complete usage information.

Changes to other types of files should match the existing style in that file.

## Squashing Commits

Before creating a pull request you should squash, or combine, all of the commits on your personal branch into a single commit that includes all of your changes. This will make it easier for the CEF developers to review your changes. Note that this is a destructive process so you might want to use a new branch just in case you make a mistake.

1\. Create a new personal branch that will be squashed.

```
# Start with the existing personal branch that contains all of your commits.
git checkout <ExistingBranchName>

# Create a new personal branch that will be squashed.
git checkout -b <BranchName>
```

2\. Identify the first commit where your personal branch diverged from an existing CEF branch.

```
# Replace <BranchName> with your new branch name.
# Replace "master" with a different CEF branch as appropriate (e.g. "2272", "2171", etc).
git merge-base <BranchName> master
```

3\. Start an interactive rebase using the commit hash returned from step 2.

```
git rebase --interactive <hash>
```

This will launch a text editor with a list of all commits in your personal branch. It should look something like this:

```
pick 59d2a23 Initial implementation
pick 752ae4c Add more features
pick cd302a3 Fix something
pick 5410de3 Fix something else
```

4\. Change all but the first line to say "squash" instead of "pick". The contents should now look like this:

```
pick 59d2a23 Initial implementation
squash 752ae4c Add more features
squash cd302a3 Fix something
squash 5410de3 Fix something else
```

5\. Save the changes and close the file. A new file will now open containing the commit messages from all of the commits. Reword the commit message then save the changes and close the file.

6\. Push the modifications to your personal remote repository.

```
# If the branch has already been pushed to the remote repository you will need to add
# the  `--force` argument.
git push origin <BranchName>
```

For more information on using interactive rebase see [this article](https://help.github.com/articles/about-git-rebase/).

## Creating a Pull Request

Pull requests can only be created from a repository that was [forked](https://confluence.atlassian.com/bitbucket/forking-a-repository-221449527.html) using the Bitbucket interface. After pushing your changes to the forked repository you can create the pull request as described [here](https://confluence.atlassian.com/bitbucket/create-a-pull-request-945541466.html). Pull requests will only be accepted if they meet the requirements described above.

## Reviewing a Pull Request

Your pull request will be reviewed by one or more CEF developers.  Please address any comments and update your pull request. The easiest way to update a pull request is by pushing new commits to the same branch -- those new commits will be [automatically reflected](https://blog.bitbucket.org/2014/04/22/bitbucket-now-auto-updates-pull-requests/) in the pull request. Once your changes are deemed acceptable they will be squashed and merged into the main CEF repository.

Detailed instructions for locally testing a pull request created by someone else are available [here](http://www.electricmonk.nl/log/2014/03/31/test-a-pull-merge-request-before-accepting-on-bitbucket/).

The contents of a pull request can also be downloaded as a patch file and applied to your local Git checkout:

```
# Download the patch file.
curl -u user:password https://bitbucket.org/api/2.0/repositories/{user}/{repo}/pullrequests/{pull_no}/patch -L -o name.patch

# Apply the patch file to your local Git checkout.
git apply name.patch
```

This Wiki page explains how to handle crashes in CEF-based applications.

**Note to Editors: Changes made to this Wiki page without prior approval via the [CEF Forum](http://magpcss.org/ceforum/) or [Issue Tracker](https://github.com/chromiumembedded/cef/issues) may be lost or reverted.**

***
[TOC]
***

# Overview

As a developer or vendor that cares about software quality you would like to know when and how your application is crashing in the wild. A crash reporting system is an important part of that feedback loop. Applications using CEF version 3.2883.1543 or newer have the option to save crash reports locally or upload them to a web server for later analysis. CEF generates minidump files that include exception state, callstacks, stack memory, and loaded modules. The crash report upload, if enabled, includes the minidump file and configurable crash metadata information such as product state, command-line flags and crash keys. Minidump files can be analyzed using existing systems such as [Socorro](https://wiki.mozilla.org/Socorro) or command-line tools (see the "Decoding Minidumps" section).

CEF provides a simple HTTP server script written in Python that can be used for testing crash report upload (see the Testing section below). See the Socorro documentation for details on how to build a robust crash reporting server for use with production applications.

The crash reporting capability in CEF/Chromium is implemented using [Crashpad](https://chromium.googlesource.com/crashpad/crashpad/+/master/README.md) on Windows and macOS, and [Breakpad](https://chromium.googlesource.com/breakpad/breakpad) on Linux.

# Configuration

Crash reporting is configured using an INI-style config file named "crash_reporter.cfg". On Windows and Linux this file must be placed next to the main application executable. On macOS this file must be placed in the top-level app bundle Resources directory (e.g. "<appname>.app/Contents/Resources"). See comments in [include/cef_crash_util.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/cef_crash_util.h?at=master&fileviewer=file-view-default) for the most up-to-date version of this documentation.

The "crash_reporter.cfg" file contents are defined as follows:

```
# Comments start with a hash character and must be on their own line.

[Config]
ProductName=<Value of the "prod" crash key; defaults to "cef">
ProductVersion=<Value of the "ver" crash key; defaults to the CEF version>
AppName=<Windows only; App-specific folder name component for storing crash
         information; default to "CEF">
ExternalHandler=<Windows only; Name of the external handler exe to use
                 instead of re-launching the main exe; default to empty>
BrowserCrashForwardingEnabled=<macOS only; True if browser process crashes
                              should be forwarded to the system crash
                              reporter; default to false>
ServerURL=<crash server URL; default to empty>
RateLimitEnabled=<True if uploads should be rate limited; default to true>
MaxUploadsPerDay=<Max uploads per 24 hours, used if rate limit is enabled;
                  default to 5>
MaxDatabaseSizeInMb=<Total crash report disk usage greater than this value
                     will cause older reports to be deleted; default to 20>
MaxDatabaseAgeInDays=<Crash reports older than this value will be deleted;
                      default to 5>

[CrashKeys]
my_key1=<small|medium|large>
my_key2=<small|medium|large>
```

## [Config] section

Values in the "[Config]" section work as follows:

* If "ProductName" and/or "ProductVersion" are set then the specified values will be included in the crash dump metadata. On macOS if these values are set to empty then they will be retrieved from the Info.plist file using the "CFBundleName" and "CFBundleShortVersionString" keys respectively.
* If "AppName" is set on Windows then crash report information (metrics, database and dumps) will be stored locally on disk under the "C:\Users\\[CurrentUser]\AppData\Local\\[AppName]\User Data" folder. On other platforms the CefSettings.user_data_path value will be used.
* If "ExternalHandler" is set on Windows then the specified exe will be launched as the crashpad-handler instead of re-launching the main process exe. The value can be an absolute path or a path relative to the main exe directory. On Linux the CefSettings.browser_subprocess_path value will be used. On macOS the existing subprocess app bundle will be used.
* If "BrowserCrashForwardingEnabled" is set to true on macOS then browser process crashes will be forwarded to the system crash reporter. This results in the crash UI dialog being displayed to the user and crash reports being logged under "~/Library/Logs/DiagnosticReports". Forwarding of crash reports from non-browser processes and Debug builds is always disabled.
* If "ServerURL" is set then crashes will be uploaded as a multi-part POST request to the specified URL. Otherwise, reports will only be stored locally on disk.
* If "RateLimitEnabled" is set to true then crash report uploads will be rate limited as follows:
    * If "MaxUploadsPerDay" is set to a positive value then at most the specified number of crashes will be uploaded in each 24 hour period.
    * If crash upload fails due to a network or server error then an incremental backoff delay up to a maximum of 24 hours will be applied for retries.
    * If a backoff delay is applied and "MaxUploadsPerDay" is > 1 then the "MaxUploadsPerDay" value will be reduced to 1 until the client is restarted. This helps to avoid an upload flood when the network or server error is resolved.
    * Rate limiting is not supported on Linux.
* If "MaxDatabaseSizeInMb" is set to a positive value then crash report storage on disk will be limited to that size in megabytes. For example, on Windows each dump is about 600KB so a "MaxDatabaseSizeInMb" value of 20 equates to about 34 crash reports stored on disk. Not supported on Linux.
* If "MaxDatabaseAgeInDays" is set to a positive value then crash reports older than the specified age in days will be deleted. Not supported on Linux.

## [CrashKeys] section

A maximum of 26 crash keys of each size can be specified for use by the application. Crash key values will be truncated based on the specified size (small = 64 bytes, medium = 256 bytes, large = 1024 bytes). The value of crash keys can be set from any thread or process using the CefSetCrashKeyValue function. These key/value pairs will be sent to the crash server along with the crash dump file.

# Testing

CEF provides a [tools/crash_server.py](https://bitbucket.org/chromiumembedded/cef/src/master/tools/crash_server.py?at=master&fileviewer=file-view-default) script that implements a simple HTTP server for receiving crash report uploads from a Breakpad/Crashpad client (any application using CEF version 3.2883.1543 or newer). This script is intended for testing purposes only. An HTTPS server and a system such as [Socorro](https://wiki.mozilla.org/Socorro) should be used when uploading crash reports from production applications. See comments in the crash_server.py script for the most up-to-date version of this documentation.

Usage of the crash_server.py script is as follows:

1\. Run the script from the command-line. The first argument is the server port number and the second argument is the directory where uploaded report information will be saved:

```
python crash_server.py 8080 /path/to/dumps
```

2\. Create a "crash_reporter.cfg" file at the required platform-specific location. On Windows and Linux this file must be placed next to the main application executable. On macOS this file must be placed in the top-level app bundle Resources directory (e.g. "<appname>.app/Contents/Resources"). At a minimum it must contain a "ServerURL=http://localhost:8080" line under the "[Config]" section (make sure the port number matches the value specified in step 1). See the Configuration section above for a complete specification of this file's contents.

Example file contents:

```
[Config]
# Product information.
ProductName=cefclient
ProductVersion=1.0.0

# Required to enable crash dump upload.
ServerURL=http://localhost:8080

# Disable rate limiting so that all crashes are uploaded.
RateLimitEnabled=false
MaxUploadsPerDay=0

[CrashKeys]
# The cefclient sample application sets these values (see step 5 below).
testkey_small1=small
testkey_small2=small
testkey_medium1=medium
testkey_medium2=medium
testkey_large1=large
testkey_large2=large
```

3\. Load one of the following URLs in the CEF-based application to cause a crash:

* Main (browser) process crash:   chrome://inducebrowsercrashforrealz
* Renderer process crash:         chrome://crash
* GPU process crash:              chrome://gpucrash

4\. When the script successfully receives a crash report upload you will see console output like the following:

```
01/10/2017 12:31:23: Dump <id>
```

The "<id>" value is a 16 digit hexadecimal string that uniquely identifies the dump. Crash minidump contents and metadata (product state, command-line flags and crash keys) will be written to the "<id>.dmp" and "<id>.json" files respectively underneath the directory specified in step 1.

On Linux, Breakpad uses the `wget` utility to upload crash dumps, so make sure that utility is installed. If the crash is handled correctly then you should see console output like the following when the client uploads a crash dump:

```
--2017-01-10 12:31:22--  http://localhost:8080/
Resolving localhost (localhost)... 127.0.0.1
Connecting to localhost (localhost)|127.0.0.1|:8080... connected.
HTTP request sent, awaiting response... 200 OK
Length: unspecified [text/html]
Saving to: '/dev/fd/3'
Crash dump id: <id>
```

On macOS, when uploading a crash report to the script over HTTP you may receive an error like the following:

```
Transport security has blocked a cleartext HTTP (http://) resource load
since it is insecure. Temporary exceptions can be configured via your app's
Info.plist file.
```

You can work around this error by adding the "NSAppTransportSecurity" key to the Info.plist file in your application's Helper.app bundle (e.g. "tests/cefclient/resources/mac/helper-Info.plist" before building cefclient, or "<appname>.app/Contents/Frameworks/<appname> Helper. app/Contents/Info.plist" in the resulting app bundle):

```
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <!--Other existing keys here...-->
  <key>NSAppTransportSecurity</key>
  <dict>
    <!--Allow all connections (for testing only!)-->
    <key>NSAllowsArbitraryLoads</key>
    <true/>
  </dict>
</dict>
</plist>
```

5\. The cefclient sample application sets test crash key values in the browser and renderer processes. To work properly these values must also be defined in the "[CrashKeys]" section of "crash_reporter.cfg" as shown above.

In [tests/cefclient/browser/client_browser.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefclient/browser/client_browser.cc?at=master&fileviewer=file-view-default) (browser process):

```
CefSetCrashKeyValue("testkey_small1", "value1_small_browser");
CefSetCrashKeyValue("testkey_small2", "value2_small_browser");
CefSetCrashKeyValue("testkey_medium1", "value1_medium_browser");
CefSetCrashKeyValue("testkey_medium2", "value2_medium_browser");
CefSetCrashKeyValue("testkey_large1", "value1_large_browser");
CefSetCrashKeyValue("testkey_large2", "value2_large_browser");
```

In [tests/cefclient/renderer/client_renderer.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefclient/renderer/client_renderer.cc?at=master&fileviewer=file-view-default) (renderer process):

```
CefSetCrashKeyValue("testkey_small1", "value1_small_renderer");
CefSetCrashKeyValue("testkey_small2", "value2_small_renderer");
CefSetCrashKeyValue("testkey_medium1", "value1_medium_renderer");
CefSetCrashKeyValue("testkey_medium2", "value2_medium_renderer");
CefSetCrashKeyValue("testkey_large1", "value1_large_renderer");
CefSetCrashKeyValue("testkey_large2", "value2_large_renderer");
```

When crashing the browser or renderer processes with cefclient you should verify that the test crash key values are included in the metadata ("<id>.json") file. Some values may be chunked as described in the Configuration section above.

Example metadata for a browser process crash:

```
{
  "LOG_FATAL": "debug_urls.cc:208: Check failed: false. \n0   Chromium Embedded Framework         0x00000001161a1a0e _ZN4base5debug10StackTraceC2Ev + 30\n1   Chromium Embedded Framework         0x00000001161a1a75 _ZN4base5debug10StackTraceC1Ev + 21\n2   Chromium Embedded Fr",
  "guid": "6ada20da-7306-477e-a715-ada36448a23f",
  "num-switches": "3",
  "pid": "2073",
  "platform": "macos",
  "product": "cefclient",
  "ptype": "browser",
  "switch-1": "--url=chrome:\/\/inducebrowsercrashforrealz",
  "switch-2": "--lang=en-US",
  "testkey_large1": "value1_large_browser", 
  "testkey_large2": "value2_large_browser", 
  "testkey_medium1": "value1_medium_browser", 
  "testkey_medium2": "value2_medium_browser", 
  "testkey_small1": "value1_small_browser", 
  "testkey_small2": "value2_small_browser", 
  "version": "1.0.0",
}
```

Example metadata for a renderer process crash:

```
{
  "discardable-memory-allocated": "4194304",
  "discardable-memory-free": "3198976",
  "guid": "6ada20da-7306-477e-a715-ada36448a23f",
  "num-switches": "15",
  "origin_mismatch_origin": "https:\/\/www.google.com",
  "origin_mismatch_redirects": "2",
  "origin_mismatch_same_page": "0"
  "origin_mismatch_transition": "1",
  "origin_mismatch_url-1": "https:\/\/www.google.com\/?gws_rd=ssl",
  "pid": "2706",
  "platform": "macos",
  "product": "cefclient",
  "ptype": "renderer",
  "rvinit_main_frame_id": "2",
  "rvinit_proxy_id": "-2",
  "rvinit_view_id": "1",
  "switch-1": "--disable-databases",
  "switch-2": "--lang=en-US",
  "switch-3": "--lang=en-US",
  "switch-4": "--enable-pinch",
  "switch-5": "--num-raster-threads=4",
  "switch-6": "--enable-gpu-rasterization",
  "switch-7": "--enable-zero-copy",
  "switch-8": "--enable-gpu-memory-buffer-compositor-resources",
  "switch-9": "--enable-main-frame-before-activation",
  "switch-10": "--renderer-client-id=3",
  "testkey_large1": "value1_large_renderer", 
  "testkey_large2": "value2_large_renderer", 
  "testkey_medium1": "value1_medium_renderer", 
  "testkey_medium2": "value2_medium_renderer", 
  "testkey_small1": "value1_small_renderer", 
  "testkey_small2": "value2_small_renderer", 
  "v8-ignition": "N",
  "version": "1.0.0",
}
```

Example metadata for a GPU process crash:

```
{
  "guid": "6ada20da-7306-477e-a715-ada36448a23f",
  "num-switches": "17",
  "pid": "1664",
  "platform": "macos",
  "product": "cefclient",
  "ptype": "gpu-process",
  "switch-1": "--lang=en-US",
  "switch-2": "--supports-dual-gpus=true",
  "switch-3": "--gpu-driver-bug-workarounds=0,1,10,23,25,35,38,45,51,59,61,62,63,64,66,70,71,73,81,82,83,86,88,89",
  "switch-4": "--gpu-vendor-id=0x1002",
  "switch-5": "--gpu-device-id=0x6821",
  "switch-6": "--gpu-driver-vendor",
  "switch-7": "--gpu-driver-version",
  "switch-8": "--gpu-driver-date",
  "switch-9": "--gpu-secondary-vendor-ids=0x8086",
  "switch-10": "--gpu-secondary-device-ids=0x0d26"
  "switch-11": "--gpu-active-vendor-id=0x8086",
  "switch-12": "--gpu-active-device-id=0x0d26",
  "switch-13": "--lang=en-US",
  "version": "1.0.0",
}
```

# Decoding Minidumps

The crash report upload, if enabled, includes the minidump file with exception state, callstacks, stack memory, and loaded modules. The minidump file can be decoded using the `dump_syms` and `minidump_stackwalk` tools from Breakpad as described [here](https://www.chromium.org/developers/decoding-crash-dumps).

The Breakpad tools can be built from a local Chromium checkout on Windows, MacOS and Linux as follows:

1\. Download the Chromium code (for example, by using [CEF tooling](https://bitbucket.org/chromiumembedded/cef/wiki/MasterBuildQuickStart.md)).

2\. Write the following contents to the `out/Release/args.gn` file:

```
is_component_build=false
is_debug=false
target_cpu="x64"
is_official_build=true
```

3\. Generate the config and build:

```
gn gen out/Release
ninja -C out/Release dump_syms minidump_stackwalk
```

4\. Windows requires the `msdia140.dll` COM DLL. It must be placed next to `dump_syms.exe` or registered with the system:

```
regsvr32 /s msdia140.dll
```

Visual Studio 2022 includes a 64-bit version of this DLL at "C:\Program Files\Microsoft Visual Studio\2022\Professional\DIA SDK\bin\amd64". The 64-bit version 14.29.30133.0 or newer is recommended to avoid failures when loading large (> 4GB) PDB files ([details](https://developercommunity.visualstudio.com/t/pdb-limit-of-4-gib-is-likely-to-be-a-problem-in-a/904784)).

This Wiki page provides an overview of CEF3 and general usage information.

**Note to Editors: Changes made to this Wiki page without prior approval via the [CEF Forum](http://magpcss.org/ceforum/) or [Issue Tracker](https://github.com/chromiumembedded/cef/issues) may be lost or reverted.**

***
[TOC]
***

# Introduction

The Chromium Embedded Framework (CEF) is an open source project based on the [Google Chromium](http://www.chromium.org/Home) project. Unlike the Chromium project itself, which focuses mainly on Google Chrome application development, CEF focuses on facilitating embedded browser use cases in third-party applications. CEF insulates the user from the underlying Chromium and Blink code complexity by offering production-quality stable APIs, release branches tracking specific Chromium releases, and binary distributions. Most features in CEF have default implementations that provide rich functionality while requiring little or no integration work from the user. As of this article’s publication there are over 100 million installed instances of CEF around the world embedded in products from a wide range of companies and industries. A partial list of companies and products using CEF is available on the [CEF Wikipedia page](http://en.wikipedia.org/wiki/Chromium_Embedded_Framework#Applications_using_CEF). Some use cases for CEF include:

  * Embedding an HTML5-compliant Web browser control in an existing native application.
  * Creating a light-weight native “shell” application that hosts a user interface developed primarily using Web technologies.
  * Rendering Web content “off-screen” in applications that have their own custom drawing frameworks.
  * Acting as a host for automated testing of existing Web properties and applications.

CEF3 is the next generation of CEF based on the multi-process [Chromium Content API](http://www.chromium.org/developers/content-module/content-api). Advantages to CEF3’s multi-process architecture include:

  * Improved performance and stability (JavaScript and plugins run in a separate process).
  * Support for Retina displays.
  * GPU acceleration for WebGL and 3D CSS.
  * Cool new features like WebRTC (webcam support) and speech input.
  * Better automated UI testing via the DevTools remote debugging protocol and [ChromeDriver2](https://code.google.com/p/chromedriver/wiki/ChromeDriver2).
  * Faster access to current and future Web features and standards.

This document introduces the general concepts involved when developing an application using CEF3.

# Getting Started

## Using a Binary Distribution

Binary distributions of CEF3 are available from the [project Downloads page](https://cef-builds.spotifycdn.com/index.html). They contain all files necessary to build a particular version of CEF3 on a particular platform (Windows, MacOS or Linux). See the [Tutorial](Tutorial.md) Wiki page for detailed instructions on how to create a simple application using the CEF3 binary distribution.

No matter the platform all binary distributions share the same general structure:

  * **CMakeLists.txt** provides the [CMake configuration](https://cmake.org/overview/) for building the test applications included with the binary distribution. Platform-specific build instructions are provided as comments at the top of this file.
  * **Debug** contains a debug build the CEF shared library (libcef) and any other libraries required to run on the platform.
  * **include** contains all required CEF header files.
  * **libcef\_dll** contains the source code for the libcef\_dll\_wrapper static library that all applications using the CEF C++ API must link against. See the “C++ Wrapper” section for more information.
  * **Release** contains a release build the CEF shared library (libcef) and any other libraries required to run on the platform.
  * **Resources** contains resources required by applications using CEF (Windows and Linux only). This includes .pak files (binary files with globbed resources) and potentially other files such depending on the platform.
  * **tests/cefclient** contains the cefclient sample application configured to build using the files in the binary distribution. This application demonstrates a wide range of CEF functionalities.
  * **tests/cefsimple** contains the cefsimple sample application configured to build using the files in the binary distribution. This application demonstrates the minimal functionality required to create a browser window.
  * **tests/ceftests** contains the ceftests sample application configured to build using the files in the binary distribution. This application provides unit test coverage for CEF APIs and functionality.

Each binary distribution also contains a README.txt file that describes the platform-specific distribution in greater detail and a LICENSE.txt file that contains CEF’s BSD license. When distributing an application based on CEF you should include the license text somewhere in your application’s distribution. For example, you can list it on an “About” or “Credits” page in your application’s UI, or in the documentation bundled with your application. License and credit information is also available inside of a CEF3 browser window by loading “about:license” and “about:credits” respectively.

Applications based on CEF binary distributions can be built using standard platform build tools. This includes Visual Studio on Windows, Xcode on MacOS and gcc/make on Linux. The project Downloads page contains information about the OS and build tool versions required for specific binary releases. When building on Linux also pay careful attention to the listed package dependencies.

## Building from Source Code

CEF can be built from source code either locally or using automated build systems like [TeamCity](http://www.jetbrains.com/teamcity/). This requires the download of Chromium and CEF source code via Git. The Chromium code base is quite large and building Chromium from source code is only recommended on moderately powerful machines with more than 8GB of RAM. Detailed instructions for building Chromium and CEF from source code are available on the [BranchesAndBuilding](BranchesAndBuilding.md) Wiki page.

# Sample Application

The cefclient sample application is a complete working example of CEF integration and is included in source code form with each binary distribution. The easiest way to create a new application using CEF is to start with the cefclient application and remove the parts that you don’t need. Many of the examples in this document originate from the cefclient application.

# Important Concepts

There are some important underlying concepts to developing CEF3-based applications that should be understood before proceeding.

## C++ Wrapper

The libcef shared library exports a C API that isolates the user from the CEF runtime and code base. The libcef\_dll\_wrapper project, which is distributed in source code form as part of the binary release, wraps this exported C API in a C++ API that is then linked into the client application. The code for this C/C++ API translation layer is automatically generated by the [translator](https://bitbucket.org/chromiumembedded/cef/src/master/tools/translator.README.txt?at=master) tool. Direct usage of the C API is described on the [UsingTheCAPI](UsingTheCAPI.md) page.

## Processes

CEF3 runs using multiple processes. The main process which handles window creation, UI and network access is called the “browser” process. This is generally the same process as the host application and the majority of the application logic will run in the browser process. Blink rendering and JavaScript execution occur in a separate “render” process. Some application logic, such as JavaScript bindings and DOM access, will also run in the render process. The default [process model](http://www.chromium.org/developers/design-documents/process-models) will spawn a new render process for each unique origin (scheme + domain). Other processes will be spawned as needed, such as the “gpu” process to handle [accelerated compositing](http://www.chromium.org/developers/design-documents/gpu-accelerated-compositing-in-chrome).

By default the main application executable will be spawned multiple times to represent separate processes. This is handled via command-line flags that are passed into the CefExecuteProcess function. If the main application executable is large, takes a long time to load, or is otherwise unsuitable for non-browser processes the host can use a separate executable for those other processes. This can be configured via the CefSettings.browser\_subprocess\_path variable. See the “Application Structure” section for more information.

The separate processes spawned by CEF3 communicate using Inter-Process Communication (IPC). Application logic implemented in the browser and render processes can communicate by sending asynchronous messages back and forth. [JavaScriptIntegration](https://bitbucket.org/chromiumembedded/cef/wiki/JavaScriptIntegration.md) in the render process can expose asynchronous APIs that are handled in the browser process. See the “Inter-Process Communication” section for more information.

Platform-specific debugging tips are also available for [Windows](https://www.chromium.org/developers/how-tos/debugging-on-windows), [MacOS](https://chromium.googlesource.com/chromium/src/+/main/docs/mac/debugging.md) and [Linux](https://chromium.googlesource.com/chromium/src/+/master/docs/linux/debugging.md).

## Threads

Each process in CEF3 runs multiple threads. For a complete list of threads see the [cef\_thread\_id\_t](https://bitbucket.org/chromiumembedded/cef/src/81e7748fb5008295c2a1f2d75b0f0c907c1fdfe8/include/internal/cef_types.h#lines-1366) enumeration. These are some of the commonly-referenced threads:

  * **TID\_UI** thread is the main thread in the browser process. This thread will be the same as the main application thread if CefInitialize() is called with a CefSettings.multi\_threaded\_message\_loop value of false.
  * **TID\_IO** thread is used in the browser process to handle IPC and network messages.
  * **TID\_FILE\_\*** threads are used in the browser process to interact with the file system. Blocking operations should only be performed on this thread or a [CefThread](https://cef-builds.spotifycdn.com/docs/stable.html?classCefThread.html) created by the client application.
  * **TID\_RENDERER** thread is the main thread in the renderer process. All Blink and V8 interation must take place on this thread.

Due to the multi-threaded nature of CEF it’s important to use message passing or locking to protect data members from access on multiple threads. The CefPostTask family of functions support easy asynchronous message passing between threads. See the “Posting Tasks” section for more information.

The current thread can be verified using the CefCurrentlyOn() function. The CEF sample applications use the following defines to verify that methods are executed on the expected thread. These defines are included in the [include/wrapper/cef\_helpers.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/wrapper/cef_helpers.h?at=master) header file.

```
#define CEF_REQUIRE_UI_THREAD() DCHECK(CefCurrentlyOn(TID_UI));
#define CEF_REQUIRE_IO_THREAD() DCHECK(CefCurrentlyOn(TID_IO));
#define CEF_REQUIRE_FILE_BACKGROUND_THREAD() \
 DCHECK(CefCurrentlyOn(TID_FILE_BACKGROUND));
#define CEF_REQUIRE_FILE_USER_VISIBLE_THREAD() \
 DCHECK(CefCurrentlyOn(TID_FILE_USER_VISIBLE));
#define CEF_REQUIRE_FILE_USER_BLOCKING_THREAD() \
 DCHECK(CefCurrentlyOn(TID_FILE_USER_BLOCKING));
#define CEF_REQUIRE_RENDERER_THREAD() DCHECK(CefCurrentlyOn(TID_RENDERER));
```

To support synchronized access to blocks of code CEF provides base::Lock and base::AutoLock types via the [include/base/cef\_lock.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/base/cef_lock.h?at=master) header file. For example:

```
// Include the necessary header.
#include "include/base/cef_lock.h"

// Class declaration.
class MyClass : public CefBaseRefCounted {
 public:
  MyClass() : value_(0) {}
  // Method that may be called on multiple threads.
  void IncrementValue();
 private:
  // Value that may be accessed on multiple theads.
  int value_;
  // Lock used to protect access to |value_|.
  base::Lock lock_;
  IMPLEMENT_REFCOUNTING(MyClass);
};

// Class implementation.
void MyClass::IncrementValue() {
  // Acquire the lock for the scope of this method.
  base::AutoLock lock_scope(lock_);
  // |value_| can now be modified safely.
  value_++;
}
```

## Reference Counting

All framework classes implement the CefBase[RefCounted|Scoped] interface and all instance pointers are handled using the CefRefPtr smart pointer implementation that automatically handles reference counting via calls to AddRef() and Release(). The easiest way to implement these classes is as follows:

```
class MyClass : public CefBaseRefCounted {
 public:
  // Various class methods here...

 private:
  // Various class members here...

  IMPLEMENT_REFCOUNTING(MyClass);  // Provides atomic refcounting implementation.
};

// References a MyClass instance
CefRefPtr<MyClass> my_class = new MyClass();
```

## Strings

CEF defines its own data structure for representing strings. This is for a few different reasons:

  * The libcef library and the host application may use different runtimes for managing heap memory. All objects, including strings, need to be freed using the same runtime that allocated the memory.
  * The libcef library can be compiled to support different underlying string types (UTF8, UTF16 or wide). The default is UTF16 but it can be changed by modifying the defines in [cef\_string.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/internal/cef_string.h?at=master) and recompiling CEF. When choosing the wide string type keep in mind that the size will vary depending on the platform.

For UTF16 the string structure looks like this:

```
typedef struct _cef_string_utf16_t {
  char16* str;  // Pointer to the string
  size_t length;  // String length
  void (*dtor)(char16* str);  // Destructor for freeing the string on the correct heap
} cef_string_utf16_t;
```

The selected string type is then typedef’d to the generic type:

```
typedef char16 cef_char_t;
typedef cef_string_utf16_t cef_string_t;
```

CEF provides a number of C API functions for operating on the CEF string types (mapped via #defines to the type-specific functions). For example:

  * **cef\_string\_set** will assign a string value to the structure with or without copying the value.
  * **cef\_string\_clear** will clear the string value.
  * **cef\_string\_cmp** will compare two string values.

CEF also provides functions for converting between all supported string types (ASCII, UTF8, UTF16 and wide). See the [cef\_string.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/internal/cef_string.h?at=master) and [cef\_string\_types.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/internal/cef_string_types.h?at=master) headers for the complete list of functions.

Usage of CEF strings in C++ is simplified by the CefString class. CefString provides automatic conversion to and from std::string (UTF8) and std::wstring (wide) types. It can also be used to wrap an existing cef\_string\_t structure for assignment purposes.

Assignment to and from std::string:

```
std::string str = “Some UTF8 string”;

// Equivalent ways of assigning |str| to |cef_str|. Conversion from UTF8 will occur if necessary.
CefString cef_str(str);
cef_str = str;
cef_str.FromString(str);

// Equivalent ways of assigning |cef_str| to |str|. Conversion to UTF8 will occur if necessary.
str = cef_str;
str = cef_str.ToString();
```

Assignment  to and from std::wstring:

```
std::wstring str = “Some wide string”;

// Equivalent ways of assigning |str| to |cef_str|. Conversion from wide will occur if necessary.
CefString cef_str(str);
cef_str = str;
cef_str.FromWString(str);

// Equivalent ways of assigning |cef_str| to |str|. Conversion to wide will occur if necessary.
str = cef_str;
str = cef_str.ToWString();
```

Use the FromASCII() method if you know that the format is ASCII:

```
const char* cstr = “Some ASCII string”;
CefString cef_str;
cef_str.FromASCII(cstr);
```

Some structures like CefSettings have cef\_string\_t members. CefString can be used to simplify the assignment to those members:

```
CefSettings settings;
const char* path = “/path/to/log.txt”;

// Equivalent assignments.
CefString(&settings.log_file).FromASCII(path);
cef_string_from_ascii(path, strlen(path), &settings.log_file);
```

## Command Line Arguments

Many features in CEF3 and Chromium can be configured using command line arguments. These arguments take the form "--some-argument[=optional-param]" and are passed into CEF via CefExecuteProcess() and the CefMainArgs structure (see the “Application Structure” section below).

  * To disable processing of arguments from the command line set CefSettings.command\_line\_args\_disabled to true before passing the CefSettings structure into CefInitialize().
  * To specify CEF/Chromium command line arguments inside the host application implement the CefApp::OnBeforeCommandLineProcessing() method.
  * To pass application-specific (non-CEF/Chromium) command line arguments to sub-processes implement the CefBrowserProcessHandler::OnBeforeChildProcessLaunch() method.

See comments in [shared/common/client\_switches.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/shared/common/client_switches.cc?at=master) for more information on how to discover supported CEF/Chromium command line switches.

# Application Layout

Application layout can differ significantly depending on the platform. For example, on MacOS your application layout must follow a specific app bundle structure. Windows and Linux are more flexible, allowing you to customize the location where CEF libraries and resources are stored. For a complete working example of the required layout you can download a “Sample Application” from the [project Downloads page](https://cef-builds.spotifycdn.com/index.html). Some distributed files are required and some are optional. Requirements and additional information for each file can be found in the binary distribution README.txt file.

## Windows

On Windows the default layout places the libcef library and related resources next to the application executable. The directory structure looks like this for 4692 branch:

```
Application/
    cefclient.exe  <= cefclient application executable
    libcef.dll <= main CEF library
    icudtl.dat <= unicode support data
    libEGL.dll, libGLESv2.dll, ... <= accelerated compositing support libraries
    chrome_100_percent.pak, chrome_200_percent.pak, resources.pak <= non-localized resources and strings
    snapshot_blob.bin, v8_context_snapshot.bin <= V8 initial snapshot
    locales/
        en-US.pak, ... <= locale-specific resources and strings
```

The location of the CEF libraries and resource files can be customized using the CefSettings structure (see the README.txt file or “CefSettings” section for details). The cefclient application on Windows compiles in resources via the BINARY resource type in [cefclient/resources/win/cefclient.rc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefclient/resources/win/cefclient.rc?at=master) but an application could just as easily load resources from the local file system.

## Linux

On Linux the default layout places the libcef library and related resources next to the application executable. Note however that there’s a discrepancy between where libcef.so is located in the client distribution and where it’s located in the binary distribution that you build yourself. The location depends on how the linker rpath value is set when building the application executable. For example, a value of “-Wl,-rpath,.” (“.” meaning the current directory) will allow you to place libcef.so next to the application executable. The path to libcef.so can also be specified using the LD\_LIBRARY\_PATH environment variable. The directory structure looks like this for 4692 branch:

```
Application/
    cefclient  <= cefclient application executable
    chrome-sandbox <= sandbox support binary
    libcef.so <= main CEF library
    icudtl.dat <= unicode support data
    chrome_100_percent.pak, chrome_200_percent.pak, resources.pak <= non-localized resources and strings
    snapshot_blob.bin, v8_context_snapshot.bin <= V8 initial snapshot
    locales/
        en-US.pak, ... <= locale-specific resources and strings
    files/
        binding.html, ... <= cefclient application resources
```

The location of the CEF libraries and resource files can be customized using the CefSettings structure (see the README.txt file of “CefSettings” section for details).

## MacOS

The application (app bundle) layout on MacOS is mandated by the Chromium implementation and consequently is not very flexible. The directory structure looks like this for 4692 branch:

```
cefclient.app/
    Contents/
        Frameworks/
            Chromium Embedded Framework.framework/
                Chromium Embedded Framework <= main application library
                Resources/
                    chrome_100_percent.pak, chrome_200_percent.pak, resources.pak, ... <= non-localized resources and strings
                    icudtl.dat <= unicode support data
                    snapshot_blob.bin, v8_context_snapshot.bin <= V8 initial snapshot
                    en.lproj/, ... <= locale-specific resources and strings
            cefclient Helper.app/
                Contents/
                    Info.plist
                    MacOS/
                        cefclient Helper <= helper executable
                    Pkginfo
        Info.plist
        MacOS/
            cefclient <= cefclient application executable
        Pkginfo
        Resources/
            binding.html, ... <= cefclient application resources
```

The "Chromium Embedded Framework.framework" is an [unversioned framework](http://src.chromium.org/viewvc/chrome/trunk/src/build/mac/copy_framework_unversioned.sh?view=markup) that contains all CEF binaries and resources. Executables (cefclient, cefclient Helper, etc) dynamically load the CEF Framework as described [here](https://groups.google.com/d/msg/cef-announce/Fith0A3kWtw/6ds_mJVMCQAJ).

The "cefclient Helper" app is used for executing separate processes (renderer, plugin, etc) with different characteristics. It needs to have a separate app bundle and Info.plist file so that, among other things, it doesn’t show dock icons.

# Application Structure

Every CEF3 application has the same general structure.

  * Provide an entry-point function that initializes CEF and runs either sub-process executable logic or the CEF message loop.
  * Provide an implementation of [CefApp](https://cef-builds.spotifycdn.com/docs/stable.html?classCefApp.html) to handle process-specific callbacks.
  * Provide an implementation of [CefClient](https://cef-builds.spotifycdn.com/docs/stable.html?classCefClient.html) to handle browser-instance-specific callbacks.
  * Call CefBrowserHost::CreateBrowser() to create a browser instance and manage the browser life span using [CefLifeSpanHandler](https://cef-builds.spotifycdn.com/docs/stable.html?classCefLifeSpanHandler.html).

## Entry-Point Function

As described in the “Processes” section a CEF3 application will run multiple processes. The processes can all use the same executable or a separate executable can be specified for the sub-processes. Execution of the process begins in the entry-point function. Complete platform-specific examples for Windows, Linux and MacOS are available in [cefclient/cefclient\_win.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefclient/cefclient_win.cc?at=master), [cefclient/cefclient\_gtk.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefclient/cefclient_gtk.cc?at=master) and [cefclient/cefclient\_mac.mm](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefclient/cefclient_mac.mm?at=master) respectively.

When launching sub-processes CEF will specify configuration information using the command-line that must be passed into the CefExecuteProcess function via the CefMainArgs structure. The definition of CefMainArgs is platform-specific. On Linux and MacOS it accepts the argc and argv values which are passed into the [main() function](https://en.wikipedia.org/wiki/Main_function).

```
CefMainArgs main_args(argc, argv);
```

On Windows it accepts the instance handle (HINSTANCE) which is passed into the [wWinMain() function](http://msdn.microsoft.com/en-us/library/windows/desktop/ff381406(v=vs.85).aspx). The instance handle is also retrievable via GetModuleHandle(nullptr).

```
CefMainArgs main_args(hInstance);
```

### Single Executable

When running as a single executable the entry-point function is required to differentiate between the different process types. The single executable structure is supported on Windows and Linux but not on MacOS.

```
// Program entry-point function.
int main(int argc, char* argv[]) {
  // Structure for passing command-line arguments.
  // The definition of this structure is platform-specific.
  CefMainArgs main_args(argc, argv);

  // Implementation of the CefApp interface.
  CefRefPtr<MyApp> app(new MyApp);

  // Execute the sub-process logic, if any. This will either return immediately for the browser
  // process or block until the sub-process should exit.
  int exit_code = CefExecuteProcess(main_args, app.get());
  if (exit_code >= 0) {
    // The sub-process terminated, exit now.
    return exit_code;
  }

  // Populate this structure to customize CEF behavior.
  CefSettings settings;

  // Initialize CEF in the main process.
  CefInitialize(main_args, settings, app.get());

  // Run the CEF message loop. This will block until CefQuitMessageLoop() is called.
  CefRunMessageLoop();

  // Shut down CEF.
  CefShutdown();

  return 0;
}
```

### Separate Sub-Process Executable

When using a separate sub-process executable you need two separate executable projects and two separate entry-point functions.

Main application entry-point function:

```
// Program entry-point function.
int main(int argc, char* argv[]) {
  // Load the CEF framework library at runtime instead of linking directly
  // as required by the macOS sandbox implementation.
  CefScopedLibraryLoader library_loader;
  if (!library_loader.LoadInMain())
    return 1;

  // Structure for passing command-line arguments.
  // The definition of this structure is platform-specific.
  CefMainArgs main_args(argc, argv);

  // Implementation of the CefApp interface.
  CefRefPtr<MyApp> app(new MyApp);

  // Populate this structure to customize CEF behavior.
  CefSettings settings;

  // Specify the path for the sub-process executable.
  CefString(&settings.browser_subprocess_path).FromASCII(“/path/to/subprocess”);

  // Initialize CEF in the main process.
  CefInitialize(main_args, settings, app.get());

  // Run the CEF message loop. This will block until CefQuitMessageLoop() is called.
  CefRunMessageLoop();

  // Shut down CEF.
  CefShutdown();

  return 0;
}
```

Sub-process application entry-point function:

```
// Program entry-point function.
int main(int argc, char* argv[]) {
  // Initialize the macOS sandbox for this helper process.
  CefScopedSandboxContext sandbox_context;
  if (!sandbox_context.Initialize(argc, argv))
    return 1;

  // Load the CEF framework library at runtime instead of linking directly
  // as required by the macOS sandbox implementation.
  CefScopedLibraryLoader library_loader;
  if (!library_loader.LoadInHelper())
    return 1;

  // Structure for passing command-line arguments.
  // The definition of this structure is platform-specific.
  CefMainArgs main_args(argc, argv);

  // Implementation of the CefApp interface.
  CefRefPtr<MyApp> app(new MyApp);

  // Execute the sub-process logic. This will block until the sub-process should exit.
  return CefExecuteProcess(main_args, app.get());
}
```

## Message Loop Integration

CEF can also integrate with an existing application message loop instead of running its own message loop. There are two ways to do this.

  1. Call CefDoMessageLoopWork() on a regular basis instead of calling CefRunMessageLoop(). Each call to CefDoMessageLoopWork() will perform a single iteration of the CEF message loop. Caution should be used with this approach. Calling the method too infrequently will starve the CEF message loop and negatively impact browser performance. Calling the method too frequently will negatively impact CPU usage. See [CefBrowserProcessHandler::OnScheduleMessagePumpWork](https://cef-builds.spotifycdn.com/docs/stable.html?classCefBrowserProcessHandler.html#a7ff7d1618399ede096ba16486a71d76e) for advanced usage details. You can test this mode in cefclient by running with the “--external-message-pump” command-line flag.
  1. Set CefSettings.multi\_threaded\_message\_loop = true (Windows and Linux only). This will cause CEF to run the browser UI thread on a separate thread from the main application thread. With this approach neither CefDoMessageLoopWork() nor CefRunMessageLoop() need to be called. CefInitialize() and CefShutdown() should still be called on the main application thread. You will need to provide your own mechanism for communicating with the main application thread (see for example the message window usage in cefclient\_win.cpp). You can test this mode in cefclient on Windows or Linux by running with the “--multi-threaded-message-loop” command-line flag.

## CefSettings

The CefSettings structure allows configuration of application-wide CEF settings. Some commonly configured members include:

  * **browser\_subprocess\_path** The path to a separate executable that will be launched for sub-processes. See the “Separate Sub-Process Executable” section for more information.
  * **multi\_threaded\_message\_loop** Set to true to have the browser process message loop run in a separate thread. See the “Message Loop Integration” section for more information.
  * **command\_line\_args\_disabled** Set to true to disable configuration of browser process features using standard CEF and Chromium command-line arguments. See the “Command Line Arguments” section for more information.
  * **cache\_path** The location where cache data will be stored on disk. If empty an in-memory cache will be used for some features and a temporary disk cache will be used for others. HTML5 databases such as localStorage will only persist across sessions if a cache path is specified.
  * **locale** The locale string that will be passed to Blink. If empty the default locale of "en-US" will be used. This value is ignored on Linux where locale is determined using environment variable parsing with the precedence order: LANGUAGE, LC\_ALL, LC\_MESSAGES and LANG. Also configurable using the "lang" command-line switch.
  * **log\_file** The directory and file name to use for the debug log. If empty, the default name of "debug.log" will be used and the file will be written to the application directory. Also configurable using the "log-file" command-line switch.
  * **log\_severity** The log severity. Only messages of this severity level or higher will be logged. Also configurable using the "log-severity" command-line switch with a value of "verbose", "info", "warning", "error", "error-report" or "disable".
  * **resources\_dir\_path** The fully qualified path for the resources directory. If this value is empty the cef.pak and/or devtools\_resources.pak files must be located in the module directory on Windows/Linux or the app bundle Resources directory on MacOS. Also configurable using the "resources-dir-path" command-line switch.
  * **locales\_dir\_path** The fully qualified path for the locales directory. If this value is empty the locales directory must be located in the module directory. This value is ignored on MacOS where pack files are always loaded from the app bundle Resources directory. Also configurable using the "locales-dir-path" command-line switch.
  * **remote\_debugging\_port** Set to a value between 1024 and 65535 to enable remote debugging on the specified port. For example, if 8080 is specified the remote debugging URL will be http://localhost:8080. CEF can be remotely debugged from any CEF or Chrome browser window. Also configurable using the "remote-debugging-port" command-line switch.

## CefBrowser and CefFrame

The [CefBrowser](https://cef-builds.spotifycdn.com/docs/stable.html?classCefBrowser.html) and [CefFrame](https://cef-builds.spotifycdn.com/docs/stable.html?classCefFrame.html) objects are used for sending commands to the browser and for retrieving state information in callback methods. Each CefBrowser object will have a single main CefFrame object representing the top-level frame and zero or more CefFrame objects representing sub-frames. For example, a browser that loads two iframes will have three CefFrame objects (the top-level frame and the two iframes).

To load a URL in the browser main frame:

```
browser->GetMainFrame()->LoadURL(some_url);
```

To navigate the browser backwards:

```
browser->GoBack();
```
To retrieve the main frame HTML contents:

```
// Implementation of the CefStringVisitor interface.
class Visitor : public CefStringVisitor {
 public:
  Visitor() {}

  // Called asynchronously when the HTML contents are available.
  virtual void Visit(const CefString& string) override {
    // Do something with |string|...
  }

  IMPLEMENT_REFCOUNTING(Visitor);
};

browser->GetMainFrame()->GetSource(new Visitor());
```

CefBrowser and CefFrame objects exist in both the browser process and the render process. Host behavior can be controlled in the browser process via the CefBrowser::GetHost() method. For example, the native handle for a windowed browser can be retrieved as follows:

```
// CefWindowHandle is defined as HWND on Windows, NSView* on MacOS
// and (usually) X11 Window on Linux.
CefWindowHandle window_handle = browser->GetHost()->GetWindowHandle();
```

Other methods are available for history navigation, loading of strings and requests, sending edit commands, retrieving text/html contents, and more. See the class documentation for the complete list of supported methods.

## CefApp

The [CefApp](https://cef-builds.spotifycdn.com/docs/stable.html?classCefApp.html) interface provides access to process-specific callbacks. Important callbacks include:

  * **OnBeforeCommandLineProcessing** which provides the opportunity to programmatically set command-line arguments. See the “Command Line Arguments” section for more information.
  * **OnRegisterCustomSchemes** which provides an opportunity to register custom schemes. See the “”Request Handling” section for more information.
  * **GetBrowserProcessHandler** which returns the handler for functionality specific to the browser process including the OnContextInitialized() method.
  * **GetRenderProcessHandler** which returns the handler for functionality specific to the render process. This includes JavaScript-related callbacks and process messages. See the [JavaScriptIntegration](https://bitbucket.org/chromiumembedded/cef/wiki/JavaScriptIntegration.md) Wiki page and the “Inter-Process Communication” section for more information.

An example CefApp implementation can be seen in [cefsimple/simple\_app.h](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_app.h?at=master) and [cefsimple/simple\_app.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_app.cc?at=master).

## CefClient

The [CefClient](https://cef-builds.spotifycdn.com/docs/stable.html?classCefClient.html) interface provides access to browser-instance-specific callbacks. A single CefClient instance can be shared among any number of browsers. Important callbacks include:

  * Handlers for things like browser life span, context menus, dialogs, display notifications, drag events, focus events, keyboard events and more. The majority of handlers are optional. See the class documentation for the side effects, if any, of not implementing a specific handler.
  * **OnProcessMessageReceived** which is called when an IPC message is received from the render process. See the “Inter-Process Communication” section for more information.

An example CefClient implementation can be seen in [cefsimple/simple\_handler.h](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_handler.h?at=master) and [cefsimple/simple\_handler.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_handler.cc?at=master).

## Browser Life Span

Browser life span begins with a call to CefBrowserHost::CreateBrowser() or CefBrowserHost::CreateBrowserSync(). Convenient places to execute this logic include the  CefBrowserProcessHandler::OnContextInitialized() callback or platform-specific message handlers like WM\_CREATE on Windows.

```
// Information about the window that will be created including parenting, size, etc.
// The definition of this structure is platform-specific.
CefWindowInfo info;
// On Windows for example...
info.SetAsChild(parent_hwnd, client_rect);

// Customize this structure to control browser behavior.
CefBrowserSettings settings;

// CefClient implementation.
CefRefPtr<MyClient> client(new MyClient);

// Create the browser asynchronously. Initially loads the Google URL.
CefBrowserHost::CreateBrowser(info, client.get(), “http://www.google.com”, settings, nullptr, nullptr);
```

The [CefLifeSpanHandler](https://cef-builds.spotifycdn.com/docs/stable.html?classCefLifeSpanHandler.html) class provides the callbacks necessary for managing browser life span. Below is an extract of the relevant methods and members.

```
class MyClient : public CefClient,
                 public CefLifeSpanHandler,
                 ... {
  // CefClient methods.
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override {
    return this;
  }

  // CefLifeSpanHandler methods.
  void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  bool DoClose(CefRefPtr<CefBrowser> browser) override;
  void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

  // Member accessors.
  CefRefPtr<CefBrowser> GetBrower() const { return browser_; }
  bool IsClosing() const { return is_closing_; }

 private:
  CefRefPtr<CefBrowser> browser_;
  int browser_id_ = -1;  // invalid value
  int browser_count_ = 0;
  bool is_closing_ = false;

  IMPLEMENT_REFCOUNTING(MyClient);
};
```

The OnAfterCreated() method will be called immediately after the browser object is created. The host application can use this method to keep a reference to the main browser object.

```
void MyClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  // Must be executed on the UI thread.
  REQUIRE_UI_THREAD();
 
  if (!browser_) {
    // Keep a reference to the main browser.
    browser_ = browser;
    browser_id_ = browser->GetIdentifier();
  }

  // Keep track of how many browsers currently exist.
  browser_count_++;
}
```

To destroy the browser call CefBrowserHost::CloseBrowser().

```
// Notify the browser window that we would like to close it. This will result in a call to 
// MyHandler::DoClose() if the JavaScript 'onbeforeunload' event handler allows it.
browser->GetHost()->CloseBrowser(false);
```

If the browser is parented to another window then the close event may originate in the OS function for that parent window (for example, by clicking the X on the parent window). The parent window then needs to call CloseBrowser(false) and wait for a second OS close event to indicate that the browser has allowed the close. The second OS close event will not be sent if the close is canceled by a JavaScript ‘onbeforeunload’ event handler or by the DoClose() callback. Notice the IsClosing() check in the below examples -- it will return false for the first OS close event and true for the second (after DoClose is called).

Handling in the parent window WndProc on Windows:

```
case WM_CLOSE:
  if (g_handler && !g_handler->IsClosing()) {
    CefRefPtr<CefBrowser> browser = g_handler->GetBrowser();
    if (browser) {
      // Notify the browser window that we would like to close it. This will result in a call to 
      // MyHandler::DoClose() if the JavaScript 'onbeforeunload' event handler allows it.
      browser->GetHost()->CloseBrowser(false);

      // Cancel the close.
      return 0;
    }
  }

  // Allow the close.
  break;

case WM_DESTROY:
  // Quitting CEF is handled in MyHandler::OnBeforeClose().
  return 0;
}
```

Handling the “delete\_event” signal on Linux:

```
gboolean delete_event(GtkWidget* widget, GdkEvent* event,
                      GtkWindow* window) {
  if (g_handler && !g_handler->IsClosing()) {
    CefRefPtr<CefBrowser> browser = g_handler->GetBrowser();
    if (browser) {
      // Notify the browser window that we would like to close it. This will result in a call to 
      // MyHandler::DoClose() if the JavaScript 'onbeforeunload' event handler allows it.
      browser->GetHost()->CloseBrowser(false);

      // Cancel the close.
      return TRUE;
    }
  }

  // Allow the close.
  return FALSE;
}
```

Shutdown on OS X is more complicated. See comments in [cefsimple/cefsimple\_mac.mm](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/cefsimple_mac.mm?at=master) for a complete understanding of how shutdown works on that platform.

The DoClose() method sets the is\_closing\_ flag and returns false to send the second OS close event.

```
bool MyClient::DoClose(CefRefPtr<CefBrowser> browser) {
  // Must be executed on the UI thread.
  REQUIRE_UI_THREAD();

  // Closing the main window requires special handling. See the DoClose()
  // documentation in the CEF header for a detailed description of this
  // process.
  if (browser_id_ == browser->GetIdentifier()) {
    // Set a flag to indicate that the window close should be allowed.
    is_closing_ = true;
  }

  // Allow the close. For windowed browsers this will result in the OS close
  // event being sent.
  return false;
}
```

When the OS function receives the second OS close event it allows the parent window to actually close. This then results in a call to OnBeforeClose(). Make sure to release any references to the browser object in the OnBeforeClose() callback.

```
void MyHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  // Must be executed on the UI thread.
  REQUIRE_UI_THREAD();

  if (browser_id_ == browser->GetIdentifier()) {
    // Free the browser pointer so that the browser can be destroyed.
    browser_ = nullptr;
  }

  if (--browser_count_ == 0) {
    // All browser windows have closed. Quit the application message loop.
    CefQuitMessageLoop();
  }
}
```

See the cefclient application for complete working examples on each platform.

## Off-Screen Rendering

With off-screen rendering CEF does not create a native browser window. Instead, CEF provides the host application with invalidated regions and a pixel buffer and the host application notifies CEF of input events such as mouse, keyboard and focus. Off-screen rendering does not currently support accelerated compositing so performance may suffer as compared to a windowed browser. Off-screen browsers will receive the same notifications as windowed browsers including the life span notifications described in the previous section. To use off-screen rendering:

  1. Implement the [CefRenderHandler](https://cef-builds.spotifycdn.com/docs/stable.html?classCefRenderHandler.html) interface. All methods are required unless otherwise indicated.
  1. Call CefWindowInfo::SetAsWindowless() before passing the CefWindowInfo structure to CefBrowserHost::CreateBrowser(). If no parent window is passed to SetAsWindowless some functionality like context menus may not be available.
  1. The CefRenderHandler::GetViewRect() method will be called to retrieve the desired view rectangle.
  1. The CefRenderHandler::OnPaint() method will be called to provide invalid regions and the updated pixel buffer. The cefclient application draws the buffer using OpenGL but your application can use whatever technique you prefer.
  1. To resize the browser call CefBrowserHost::WasResized(). This will result in a call to GetViewRect() to retrieve the new size followed by a call to OnPaint().
  1. Call the CefBrowserHost::SendXXX() methods to notify the browser of mouse, keyboard and focus events.
  1. Call CefBrowserHost::CloseBrowser() to destroy browser.

Run cefclient with the “--off-screen-rendering-enabled” command-line flag for a working example.

# Posting Tasks

Tasks can be posted between the various threads in a single process using the CefPostTask family of methods (see the [include/cef\_task.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/cef_task.h?at=master) header file for the complete list). The task will execute asynchronously on the message loop of the target thread.

CEF provides base::Bind[Once|Repeating] and base::[Once|Repeating]Callback templated callback classes for passing bound methods, objects and arguments to CefPostTask. For complete base::Bind and base::Callback usage information see the [Chromium documentation](https://chromium.googlesource.com/chromium/src/+/HEAD/docs/callback.md). The [include/wrapper/cef\_closure\_task.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/wrapper/cef_closure_task.h?at=master) header provides helpers for converting a base::[Once|Repeating]Closure to a CefTask. For example:

```
// Include the necessary headers.
#include “include/base/cef_callback.h”
#include “include/wrapper/cef_closure_task.h”

// To execute a bound function:

// Define a function.
void MyFunc(int arg) { /* do something with |arg| on the UI thread */ }

// Post a task that will execute MyFunc on the UI thread and pass an |arg|
// value of 5.
CefPostTask(TID_UI, base::BindOnce(&MyFunc, 5));

// To execute a bound method:

// Define a class.
class MyClass : public CefBaseRefCounted {
 public:
  MyClass() {}
  void MyMethod(int arg) { /* do something with |arg| on the UI thread */ }
 private:
  IMPLEMENT_REFCOUNTING(MyClass);
};

// Create an instance of MyClass.
CefRefPtr<MyClass> instance = new MyClass();

// Post a task that will execute MyClass::MyMethod on the UI thread and pass
// an |arg| value of 5. |instance| will be kept alive until after the task
// completes.
CefPostTask(TID_UI, base::BindOnce(&MyClass::MyMethod, instance, 5));
```

If the host application needs to keep a reference to a run loop it can use the CefTaskRunner class. For example, to get the task runner for the UI thread:

```
CefRefPtr<CefTaskRunner> task_runner = CefTaskRunner::GetForThread(TID_UI);
```

# Inter-Process Communication (IPC)

Since CEF3 runs in multiple processes it is necessary to provide mechanisms for communicating between those processes. CefBrowser and CefFrame objects exist in both the browser and render processes which helps to facilitate this process. Each CefBrowser and CefFrame object also has a unique ID value associated with it that will match on both sides of the process boundary.

## Process Startup Messages

Startup data can be associated with a specific CefBrowser instance at creation time via the `CefRefPtr<CefDictionaryValue> extra_info` parameter to CefBrowserHost::CreateBrowser. That extra_info data will be delivered to every renderer process associated with that CefBrowser via the CefRenderProcessHandler::OnBrowserCreated callback. See the “Processes” section for more information about when and how a new renderer process will be spawned.

## Process Runtime Messages

Messages can be passed between processes at runtime using the [CefProcessMessage](https://cef-builds.spotifycdn.com/docs/stable.html?classCefProcessMessage.html) class. These messages are associated with a specific CefBrowser and CefFrame instance and sent using the CefFrame::SendProcessMessage() method. The process message should contain whatever state information is required via CefProcessMessage::GetArgumentList().

```
// Create the message object.
CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create(“my_message”);

// Retrieve the argument list object.
CefRefPtr<CefListValue> args = msg>GetArgumentList();

// Populate the argument values.
args->SetString(0, “my string”);
args->SetInt(0, 10);

// Send the process message to the main frame in the render process.
// Use PID_BROWSER instead when sending a message to the browser process.
browser->GetMainFrame()->SendProcessMessage(PID_RENDERER, msg);
```

A message sent from the browser process to the render process will arrive in CefRenderProcessHandler::OnProcessMessageReceived(). A message sent from the render process to the browser process will arrive in CefClient::OnProcessMessageReceived().

```
bool MyHandler::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {
  // Check the message name.
  const std::string& message_name = message->GetName();
  if (message_name == “my_message”) {
    // Handle the message here...
    return true;
  }
  return false;
}
```

## Asynchronous JavaScript Bindings

[JavaScriptIntegration](https://bitbucket.org/chromiumembedded/cef/wiki/JavaScriptIntegration.md) is implemented in the render process but frequently need to communicate with the browser process. The JavaScript APIs themselves should be designed to work asynchronously using [closures](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Closures) and [promises](http://www.html5rocks.com/en/tutorials/es6/promises/).

### Generic Message Router

CEF provides a generic implementation for routing asynchronous messages between JavaScript running in the renderer process and C++ running in the browser process. An application interacts with the router by passing it data from standard CEF C++ callbacks (OnBeforeBrowse, OnProcessMessageRecieved, OnContextCreated, etc). The renderer-side router supports generic JavaScript callback registration and execution while the browser-side router supports application-specific logic via one or more application-provided Handler instances. See the [message_router example](https://bitbucket.org/chromiumembedded/cef-project/src/master/examples/message_router/?at=master) for a stand-alone example application that demonstates CefMessageRouter usage. See [include/wrapper/cef\_message\_router.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/wrapper/cef_message_router.h?at=master) for complete usage documentation.

### Custom Implementation

A CEF-based application can also provide its own custom implementation of asynchronous JavaScript bindings. A simplistic implementation could work as follows:

1\. The JavaScript binding in the render process is passed a callback function.

```
// In JavaScript register the callback function.
app.setMessageCallback('binding_test', function(name, args) {
  document.getElementById('result').value = "Response: "+args[0];
});
```

2\. The render process keeps a reference to the callback function.

```
// Map of message callbacks.
typedef std::map<std::pair<std::string, int>,
                 std::pair<CefRefPtr<CefV8Context>, CefRefPtr<CefV8Value> > >
                 CallbackMap;
CallbackMap callback_map_;

// In the CefV8Handler::Execute implementation for “setMessageCallback”.
if (arguments.size() == 2 && arguments[0]->IsString() &&
    arguments[1]->IsFunction()) {
  std::string message_name = arguments[0]->GetStringValue();
  CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
  int browser_id = context->GetBrowser()->GetIdentifier();
  callback_map_.insert(
      std::make_pair(std::make_pair(message_name, browser_id),
                     std::make_pair(context, arguments[1])));
}
```

3\. The render process sends an asynchronous IPC message to the browser process requesting that work be performed.

4\. The browser process receives the IPC message and performs the work.

5\. Upon completion of the work the browser process sends an asynchronous IPC message back to the render process with the result.

6\. The render process receives the IPC message and executes the callback function with the result.

```
// Execute the registered JavaScript callback if any.
if (!callback_map_.empty()) {
  const CefString& message_name = message->GetName();
  CallbackMap::const_iterator it = callback_map_.find(
      std::make_pair(message_name.ToString(),
                     browser->GetIdentifier()));
  if (it != callback_map_.end()) {
    // Keep a local reference to the objects. The callback may remove itself
    // from the callback map.
    CefRefPtr<CefV8Context> context = it->second.first;
    CefRefPtr<CefV8Value> callback = it->second.second;

    // Enter the context.
    context->Enter();

    CefV8ValueList arguments;

    // First argument is the message name.
    arguments.push_back(CefV8Value::CreateString(message_name));

    // Second argument is the list of message arguments.
    CefRefPtr<CefListValue> list = message->GetArgumentList();
    CefRefPtr<CefV8Value> args = CefV8Value::CreateArray(list->GetSize());
    SetList(list, args);  // Helper function to convert CefListValue to CefV8Value.
    arguments.push_back(args);

    // Execute the callback.
    CefRefPtr<CefV8Value> retval = callback->ExecuteFunction(nullptr, arguments);
    if (retval.get()) {
      if (retval->IsBool())
        handled = retval->GetBoolValue();
    }

    // Exit the context.
    context->Exit();
  }
}
```

7\. Release any V8 references associated with the context in CefRenderProcessHandler::OnContextReleased().

```
void MyHandler::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefV8Context> context) {
  // Remove any JavaScript callbacks registered for the context that has been released.
  if (!callback_map_.empty()) {
    CallbackMap::iterator it = callback_map_.begin();
    for (; it != callback_map_.end();) {
      if (it->second.first->IsSame(context))
        callback_map_.erase(it++);
      else
        ++it;
    }
  }
}
```

## Synchronous Requests

In rare cases it may be necessary to implement synchronous communication between the browser and render processes. This should be avoided whenever possible because it will negatively impact performance in the render process. However, if you must have synchronous communication consider using [synchronous XMLHttpRequests](https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest/Synchronous_and_Asynchronous_Requests) which will block the render process while awaiting handling in the browser process network layer. The browser process can handle the requests using a custom scheme handler or network interception. See the “Network Layer” section for more information.

# Network Layer

By default network requests in CEF3 will be handled in a manner transparent to the host application. For applications wishing for a closer relationship with the network layer CEF3 exposes a range of network-related functionalities.

Network-related callbacks can occur on different threads so make sure to pay attention to the documentation and properly protect your data members (see the “Threads” section for background).

## Custom Requests

The simplest way to load a URL in a browser frame is via the CefFrame::LoadURL() method.

```
browser->GetMainFrame()->LoadURL(some_url);
```

Applications wishing to send more complex requests containing custom request headers or upload data can use the CefFrame::LoadRequest() method. This method accepts a [CefRequest](https://cef-builds.spotifycdn.com/docs/stable.html?classCefRequest.html) object as the single argument.

**WARNING: The LoadRequest method will fail with "bad IPC message" reason INVALID_INITIATOR_ORIGIN (213) unless you first navigate to the request origin (scheme + domain) using some other mechanism (LoadURL, link click, etc).**

```
// Create a CefRequest object.
CefRefPtr<CefRequest> request = CefRequest::Create();

// Set the request URL.
request->SetURL(some_url);

// Set the request method. Supported methods include GET, POST, HEAD, DELETE and PUT.
request->SetMethod(“POST”);

// Optionally specify custom headers.
CefRequest::HeaderMap headerMap;
headerMap.insert(
    std::make_pair("X-My-Header", "My Header Value"));
request->SetHeaderMap(headerMap);

// Optionally specify upload content.
// The default “Content-Type” header value is "application/x-www-form-urlencoded".
// Set “Content-Type” via the HeaderMap if a different value is desired.
const std::string& upload_data = “arg1=val1&arg2=val2”;
CefRefPtr<CefPostData> postData = CefPostData::Create();
CefRefPtr<CefPostDataElement> element = CefPostDataElement::Create();
element->SetToBytes(upload_data.size(), upload_data.c_str());
postData->AddElement(element);
request->SetPostData(postData);
```

### Browser-Independent Requests

Applications can send network requests via the [CefURLRequest](https://cef-builds.spotifycdn.com/docs/stable.html?classCefURLRequest.html) class. Implement the [CefURLRequestClient](https://cef-builds.spotifycdn.com/docs/stable.html?classCefURLRequestClient.html) interface to handle the resulting response.

  * Requests not associated with a particular CefBrowser/CefFrame can be sent via the CefURLRequest::Create method. These types of requests can only be used in the browser process.
  * Requests associated with a particular CefBrowser/CefFrame can be sent via the CefFrame::CreateURLRequest method. These types of requests can be used in both the browser and the renderer process.

See documentation on the above methods for additional usage restrictions.

Here is an example of CefURLRequest usage:

```
class MyRequestClient : public CefURLRequestClient {
 public:
  MyRequestClient()
    : upload_total_(0),
      download_total_(0) {}

  void OnRequestComplete(CefRefPtr<CefURLRequest> request) override {
    CefURLRequest::Status status = request->GetRequestStatus();
    CefURLRequest::ErrorCode error_code = request->GetRequestError();
    CefRefPtr<CefResponse> response = request->GetResponse();

    // Do something with the response...
  }

  void OnUploadProgress(CefRefPtr<CefURLRequest> request,
                        int64 current,
                        int64 total) override {
    upload_total_ = total;
  }

  void OnDownloadProgress(CefRefPtr<CefURLRequest> request,
                          int64 current,
                          int64 total) override {
    download_total_ = total;
  }

  void OnDownloadData(CefRefPtr<CefURLRequest> request,
                      const void* data,
                      size_t data_length) override {
    download_data_ += std::string(static_cast<const char*>(data), data_length);
  }

  bool GetAuthCredentials(bool isProxy,
                          const CefString& host,
                          int port,
                          const CefString& realm,
                          const CefString& scheme,
                          CefRefPtr<CefAuthCallback> callback) override {
    return false;  // Not handled.
  }

 private:
  int64 upload_total_;
  int64 download_total_;
  std::string download_data_;

 private:
  IMPLEMENT_REFCOUNTING(MyRequestClient);
};
```

To send the request:

```
// Set up the CefRequest object.
CefRefPtr<CefRequest> request = CefRequest::Create();
// Populate |request| as shown above...

// Create the client instance.
CefRefPtr<MyRequestClient> client = new MyRequestClient();

// Start the request. MyRequestClient callbacks will be executed asynchronously.
CefRefPtr<CefURLRequest> url_request = CefURLRequest::Create(request, client.get(), nullptr);
// To cancel the request: url_request->Cancel();
```

Requests made with CefURLRequest can also specify custom behaviors via the CefRequest::SetFlags() method. Supported bit flags include:

  * **UR\_FLAG\_SKIP\_CACHE** If set the cache will be skipped when handling the request.
  * **UR\_FLAG\_ALLOW\_STORED\_CREDENTIALS** If set cookies may be sent with the request and saved from the response.
  * **UR\_FLAG\_REPORT\_UPLOAD\_PROGRESS** If set upload progress events will be generated when a request has a body.
  * **UR\_FLAG\_NO\_DOWNLOAD\_DATA** If set the CefURLRequestClient::OnDownloadData method will not be called.
  * **UR\_FLAG\_NO\_RETRY\_ON\_5XX** If set 5XX redirect errors will be propagated to the observer instead of automatically re-tried. This currently only applies for requests originated in the browser process.

See [cef\_urlrequest\_flags\_t](https://bitbucket.org/chromiumembedded/cef/src/81e7748fb5008295c2a1f2d75b0f0c907c1fdfe8/include/internal/cef_types.h#lines-1243) for the complete list of supported flags.

For example, to skip the cache and not report download data:

```
request->SetFlags(UR_FLAG_SKIP_CACHE | UR_FLAG_NO_DOWNLOAD_DATA);
```

## Request Handling

CEF3 supports two approaches for handling network requests inside of an application. The scheme handler approach allows registration of a handler for requests targeting a particular origin (scheme + domain). The request interception approach allows handling of arbitrary requests at the application’s discretion.

**WARNING: Use the HTTP scheme instead of a custom scheme to avoid a range of potential issues.**

If you choose to use a custom scheme (anything other than “HTTP”, “HTTPS”, etc) you must register it with CEF so that it will behave as expected. If you would like your custom scheme to behave similar to HTTP (support POST requests and enforce [HTTP access control (CORS)](https://developer.mozilla.org/en-US/docs/HTTP/Access_control_CORS) restrictions) then it should be registered as a “standard” scheme. If you are planning to perform cross-origin requests to other schemes or send POST requests via XMLHttpRequest to your scheme handler then you should use the HTTP scheme instead of a custom scheme to avoid [potential issues](https://bitbucket.org/chromiumembedded/cef/issue/950). If you wish to use custom schemes the attributes are registered via the CefApp::OnRegisterCustomSchemes() callback which must be implemented in all processes.

```
void MyApp::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) {
  // Register "client" as a standard scheme.
  int options = CEF_SCHEME_OPTION_STANDARD;
  registrar->AddCustomScheme("client", options);
}
```

See [cef\_scheme\_options\_t](https://bitbucket.org/chromiumembedded/cef/src/81e7748fb5008295c2a1f2d75b0f0c907c1fdfe8/include/internal/cef_types.h#lines-2795) for the complete list of supported flags.

### Generic Resource Manager

CEF provides a generic implementation for managing resource requests from one or more data sources. This user registers handlers for different data sources, such as directories on disk, zip archives or custom implementations, and the manager handles the requests. An application interacts with the router by passing it data from standard CEF C++ callbacks (OnBeforeResourceLoad, GetResourceHandler). See the [resource_manager example](https://bitbucket.org/chromiumembedded/cef-project/src/master/examples/resource_manager/?at=master) for a stand-alone example application that demonstates CefResourceManager usage. See [include/wrapper/cef\_resource\_manager.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/wrapper/cef_resource_manager.h?at=master) for complete usage documentation.

### Scheme Handler

A scheme handler is registered via the CefRegisterSchemeHandlerFactory() function. A good place to call this function is from CefBrowserProcessHandler::OnContextInitialized(). For example, you can register a handler for “client://myapp/” requests:

```
CefRegisterSchemeHandlerFactory("client", “myapp”, new MySchemeHandlerFactory());
```

Handlers can be used with both built-in schemes (HTTP, HTTPS, etc) and custom schemes. When using a built-in scheme choose a domain name unique to your application (like “myapp” or “internal”). Implement the [CefSchemeHandlerFactory](https://cef-builds.spotifycdn.com/docs/stable.html?classCefSchemeHandlerFactory.html) and [CefResourceHandler](https://cef-builds.spotifycdn.com/docs/stable.html?classCefResourceHandler.html) classes to handle the request and provide response data. If using custom schemes don't forget to implement the CefApp::OnRegisterCustomSchemes method as described above. See the [scheme_handler example](https://bitbucket.org/chromiumembedded/cef-project/src/master/examples/scheme_handler/?at=master) for a stand-alone example application that demonstates CefSchemeHandlerFactory usage. See [include/cef\_scheme.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/cef_scheme.h?at=master) for complete usage documentation.

If the response data is known at request time the [CefStreamResourceHandler](https://cef-builds.spotifycdn.com/docs/stable.html?classCefStreamResourceHandler.html) class provides a convenient default implementation of CefResourceHandler.

```
// CefStreamResourceHandler is part of the libcef_dll_wrapper project.
#include “include/wrapper/cef_stream_resource_handler.h”

const std::string& html_content = “<html><body>Hello!</body></html>”;

// Create a stream reader for |html_content|.
CefRefPtr<CefStreamReader> stream =
    CefStreamReader::CreateForData(
        static_cast<void*>(const_cast<char*>(html_content.c_str())),
        html_content.size());

// Constructor for HTTP status code 200 and no custom response headers.
// There’s also a version of the constructor for custom status code and response headers.
return new CefStreamResourceHandler("text/html", stream);
```

### Request Interception

The CefRequestHandler::GetResourceHandler() method supports the interception of arbitrary requests. It uses the same CefResourceHandler class as the scheme handler approach. If using custom schemes don't forget to implement the CefApp::OnRegisterCustomSchemes method as described above.

```
CefRefPtr<CefResourceHandler> MyHandler::GetResourceHandler(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request) {
  // Evaluate |request| to determine proper handling...
  if (...)
    return new MyResourceHandler();

  // Return nullptr for default handling of the request.
  return nullptr;
}
```

### Response Filtering

The CefRequestHandler::GetResourceResponseFilter() method supports filtering of request response data. See [cefclient/browser/response\_filter\_test.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefclient/browser/response_filter_test.cc?at=master) (accessible via the Tests menu > Other Tests > Response Filtering from inside the cefclient sample application) for a working example.

## Other Callbacks

The [CefRequestHandler](https://cef-builds.spotifycdn.com/docs/stable.html?classCefRequestHandler.html) interface provides callbacks for various network-related events incuding authentication, cookie handling, external protocol handling, certificate errors and so on.

## Proxy Resolution

Proxy settings are configured in CEF3 using the same command-line flags as Google Chrome.

```
--proxy-server=host:port
      Specify the HTTP/SOCKS4/SOCKS5 proxy server to use for requests. An individual proxy
      server is specified using the format:

        [<proxy-scheme>://]<proxy-host>[:<proxy-port>]

      Where <proxy-scheme> is the protocol of the proxy server, and is one of:

        "http", "socks", "socks4", "socks5".

      If the <proxy-scheme> is omitted, it defaults to "http". Also note that "socks" is equivalent to
      "socks5".

      Examples:

        --proxy-server="foopy:99"
            Use the HTTP proxy "foopy:99" to load all URLs.

        --proxy-server="socks://foobar:1080"
            Use the SOCKS v5 proxy "foobar:1080" to load all URLs.

        --proxy-server="sock4://foobar:1080"
            Use the SOCKS v4 proxy "foobar:1080" to load all URLs.

        --proxy-server="socks5://foobar:66"
            Use the SOCKS v5 proxy "foobar:66" to load all URLs.

      It is also possible to specify a separate proxy server for different URL types, by prefixing
      the proxy server specifier with a URL specifier:

      Example:

        --proxy-server="https=proxy1:80;http=socks4://baz:1080"
            Load https://* URLs using the HTTP proxy "proxy1:80". And load http://*
            URLs using the SOCKS v4 proxy "baz:1080".

--no-proxy-server
      Disables the proxy server.

--proxy-auto-detect
      Autodetect  proxy  configuration.

--proxy-pac-url=URL
      Specify proxy autoconfiguration URL.
```

If the proxy requires authentication the CefRequestHandler::GetAuthCredentials() callback will be executed with an |isProxy| value of true to retrieve the username and password.

```
bool MyHandler::GetAuthCredentials(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    bool isProxy,
    const CefString& host,
    int port,
    const CefString& realm,
    const CefString& scheme,
    CefRefPtr<CefAuthCallback> callback) {
  if (isProxy) {
    // Provide credentials for the proxy server connection.
    callback->Continue("myuser", "mypass");
    return true;
  }
  return false;
}
```

Web content loading during application startup can be delayed due to network proxy resolution (for example, if "Automatically detect proxy settings" is checked on Windows). For best user experience consider designing your application to first show a static splash page and then redirect to the actual website using [meta refresh](http://en.wikipedia.org/wiki/Meta_refresh) -- the redirect will be blocked until proxy resolution completes. For testing purposes proxy resolution can be disabled using the "--no-proxy-server" command-line flag. Proxy resolution delays can also be duplicated in Google Chrome by running "chrome --url=..." from the command line.

This Wiki page provides information on how to use the CEF C API in a client application.

**Note to Editors: Changes made to this Wiki page without prior approval via the [CEF Forum](http://magpcss.org/ceforum/) or [Issue Tracker](https://github.com/chromiumembedded/cef/issues) may be lost or reverted.**

***

# Introduction

The CEF C API is a C-based interface exported by the libcef DLL.  The cef\_capi.h header file which defines the interface is automatically generated by the CEF translator tool and mirrors the CEF C++ API structure.

An example client application using the C API is available here: https://github.com/CzarekTomczak/cefcapi

# Reference Counting

Understanding reference counting is likely the most difficult part of working with the CEF C API.  The reference counting concept used by CEF is very similar to the concept used by COM.  Here are some general rules to follow to make your usage of reference counting as painless as possible.

1\. Do not increment/decrement the reference count when passing a structure to its own
member function:

```
struct->call_func(struct,...); // no reference counting change on 'struct'
```

2\. Increment the reference count on a struct before passing it as an argument
to another struct:

```
// Should have already added a reference to 'some_other_struct'
struct->call_func(...,some_other_struct,...);
```

3\. Decrement the reference count on a struct when receiving it as an argument
from somewhere else after you're done using it:

```
void my_func(...,some_other_struct,...)
{
  // remove a reference from 'some_other_struct' after you're done using it
}
```

4\. Add a reference to a handler before passing it into, for instance,
cef\_create\_browser().  The API will remove a reference from the handler
when the handler is no longer needed.

5\. Use an atomic reference counting implementation because add\_ref and release may be called from multiple threads.  The WinAPI InterlockedIncrement() and InterlockedDecrement() functions can be used for this purpose.

6\. The handler should delete itself when the reference count reaches zero in the release()
callback function that you've assigned for your structure.

```
// custom data structure that extends cef_handler_t
typedef struct _my_handler_t {
  cef_handler_t handler;  // cef_handler_t member comes first
  // custom members here, including the reference count variable
} my_handler_t;

// allocate the custom data structure (already initialized to zero)
my_handler_t* my_handler = (my_handler_t*)calloc(1, sizeof(my_handler_t));
// set the size member of cef_base_t appropriately
my_handler->handler.base.size = sizeof(cef_handler_t);
// assign the release callback function (and the other callback functions)
my_handler->handler.base = my_release;
...

// release callback function implementation for my_handler_t
int CEF_CALLBACK my_release(struct _cef_base_t* base) {
  // this cast works because cef_base_t is the first member of
  // cef_handler_t and cef_handler_t is the first member of my_handler_t
  my_handler_t* my_handler = (my_handler_t*)base;
  
  // decrement the reference count (stored as a custom member of my_handler_t)
  // free the my_handler_t structure when the reference count reaches zero
  if(reference_count_is_zero)
    free(my_handler);
}
```

7\. Reverse any additional references that your code adds to a struct (for instance, if you keep a reference to the cef\_browser\_t pointer in your handler implementation).  The last opportunity to release references is in cef\_handler\_t::handle\_before\_window\_closed().

8\. You'll know you're handling the reference counting correctly when nothing crashes and you no longer hit the DebugObjCt asserts on shutdown.