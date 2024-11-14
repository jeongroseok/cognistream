# Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.


#
# 편의
#

list(APPEND CEF_CXX_COMPILER_FLAGS /wd4267 /wd4199 /IGNORE:4199)


#
# 공통 설정
#

# 눈에 띄는 매크로 정의 추가
macro(message_ TITLE MESSAGE)
  message(STATUS "✨🚀✨===== ${TITLE} =====✨🚀✨")
  message(STATUS "🔔 ${MESSAGE} 🔔")
endmacro()

# 빌드 과정에서 디버그 정보를 출력
# set(CMAKE_VERBOSE_MAKEFILE ON)

# CMake 최소 버전 설정
cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# 타겟 실행 파일 이름 설정
set(COGNISTREAM_TARGET "cognistream")

# 프로젝트 이름 설정
project(${COGNISTREAM_TARGET})

# include 디렉토리 설정
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 라이브러리 찾기
find_package(CEF REQUIRED)
find_package(Torch REQUIRED)

# CMake 모듈 경로 설정
if(OS_MAC)
  set(CEF_HELPER_TARGET "cognistream_Helper")
  set(CEF_HELPER_OUTPUT_NAME "cognistream Helper")
else()
  # libcef 라이브러리 링크를 위한 논리 타겟
  ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")
endif()

# 타겟 출력 디렉토리 설정
SET_CEF_TARGET_OUT_DIR()


#
# libtorch 설정
#

# libtorch dll 파일 복사
macro(ADD_WINDOWS_TORCH_DLLS target install_prefix)
file(GLOB TORCH_DLLS ${install_prefix}/lib/*.dll)
add_custom_command(
  TARGET ${target}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  ${TORCH_DLLS}
  $<TARGET_FILE_DIR:${target}>)
endmacro()

# CEF 변수에 libtorch 변수 추가
list(REMOVE_ITEM CEF_COMPILER_FLAGS /GR- /WX)
list(REMOVE_ITEM CEF_CXX_COMPILER_FLAGS /GR- /WX)
list(APPEND CEF_CXX_COMPILER_FLAGS ${TORCH_CXX_FLAGS})
list(APPEND CEF_STANDARD_LIBS ${TORCH_LIBRARIES})


#
# 소스 파일 설정
#

# cognistream 소스 파일
set(COGNISTREAM_SRCS
  simple_app.cc
  simple_app.h
  simple_handler.cc
  simple_handler.h
)
set(COGNISTREAM_SRCS_LINUX
  main_linux.cc
  simple_handler_linux.cc
)
set(COGNISTREAM_SRCS_MAC
  main_mac.mm
  simple_handler_mac.mm
)
set(COGNISTREAM_SRCS_WINDOWS
  main_win.cc
  resource.h
  simple_handler_win.cc
)
APPEND_PLATFORM_SOURCES(COGNISTREAM_SRCS)
source_group(cognistream FILES ${COGNISTREAM_SRCS})

set(COGNISTREAM_WIN_SRCS_WINDOWS
  win/cognistream.rc
)
APPEND_PLATFORM_SOURCES(COGNISTREAM_WIN_SRCS)
source_group(cognistream\\win FILES ${COGNISTREAM_WIN_SRCS})

set(COGNISTREAM_SRCS
  ${COGNISTREAM_SRCS}
  ${COGNISTREAM_WIN_SRCS}
)

# cognistream 헬퍼 소스 파일
set(COGNISTREAM_HELPER_SRCS_MAC
  process_helper_mac.cc
)
APPEND_PLATFORM_SOURCES(COGNISTREAM_HELPER_SRCS)
source_group(cognistream FILES ${COGNISTREAM_HELPER_SRCS})

# cognistream 리소스 파일
set(COGNISTREAM_RESOURCES_MAC_SRCS_MAC
  mac/Info.plist.in
  mac/cognistream.icns
)
APPEND_PLATFORM_SOURCES(COGNISTREAM_RESOURCES_MAC_SRCS)
source_group(cognistream\\mac FILES ${COGNISTREAM_RESOURCES_MAC_SRCS})

set(COGNISTREAM_RESOURCES_MAC_ENGLISH_LPROJ_SRCS_MAC
  mac/English.lproj/InfoPlist.strings
  mac/English.lproj/MainMenu.xib
)
APPEND_PLATFORM_SOURCES(COGNISTREAM_RESOURCES_MAC_ENGLISH_LPROJ_SRCS)
source_group(cognistream\\mac\\English.lproj FILES ${COGNISTREAM_RESOURCES_MAC_ENGLISH_LPROJ_SRCS})

set(COGNISTREAM_RESOURCES_SRCS
  ${COGNISTREAM_RESOURCES_MAC_SRCS}
  ${COGNISTREAM_RESOURCES_MAC_ENGLISH_LPROJ_SRCS}
)


#
# 리눅스 구성
#

if(OS_LINUX)
  # 실행 파일 타겟 생성
  add_executable(${COGNISTREAM_TARGET} ${COGNISTREAM_SRCS})
  SET_EXECUTABLE_TARGET_PROPERTIES(${COGNISTREAM_TARGET})
  add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
  target_link_libraries(${COGNISTREAM_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})

  # rpath 설정하여 라이브러리를 실행 파일 옆에 배치 가능
  set_target_properties(${COGNISTREAM_TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN")
  set_target_properties(${COGNISTREAM_TARGET} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
  set_target_properties(${COGNISTREAM_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CEF_TARGET_OUT_DIR})

  # 바이너리 및 리소스 파일을 타겟 출력 디렉토리에 복사
  COPY_FILES("${COGNISTREAM_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
  COPY_FILES("${COGNISTREAM_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")
  if(EXISTS "${CEF_BINARY_DIR}/libminigbm.so")
    COPY_FILES("${COGNISTREAM_TARGET}" "libminigbm.so" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
  endif()

  # chrome-sandbox 타겟에 SUID 권한 설정
  SET_LINUX_SUID_PERMISSIONS("${COGNISTREAM_TARGET}" "${CEF_TARGET_OUT_DIR}/chrome-sandbox")
endif()


#
# Mac OS X 구성
#

if(OS_MAC)
  option(OPTION_USE_ARC "macOS에서 ARC(자동 참조 카운팅) 사용 여부." ON)
  if(OPTION_USE_ARC)
    list(APPEND CEF_COMPILER_FLAGS
      -fobjc-arc
    )
    set_target_properties(${target} PROPERTIES
      CLANG_ENABLE_OBJC_ARC "YES"
    )
  endif()

  # 메인 앱 번들 출력 경로
  set(CEF_APP "${CEF_TARGET_OUT_DIR}/${COGNISTREAM_TARGET}.app")

  # 주요 Info.plist 파일에서 참조되는 변수
  set(EXECUTABLE_NAME "${COGNISTREAM_TARGET}")
  set(PRODUCT_NAME "${COGNISTREAM_TARGET}")

  if(USE_SANDBOX)
    # cef_sandbox 라이브러리를 링크하기 위한 논리 타겟
    ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
  endif()

  # 메인 앱 번들 타겟 생성
  add_executable(${COGNISTREAM_TARGET} MACOSX_BUNDLE ${COGNISTREAM_RESOURCES_SRCS} ${COGNISTREAM_SRCS})
  SET_EXECUTABLE_TARGET_PROPERTIES(${COGNISTREAM_TARGET})
  add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
  target_link_libraries(${COGNISTREAM_TARGET} libcef_dll_wrapper ${CEF_STANDARD_LIBS})
  set_target_properties(${COGNISTREAM_TARGET} PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/mac/Info.plist.in
  )

  # CEF 프레임워크를 Frameworks 디렉토리에 복사
  add_custom_command(
    TARGET ${COGNISTREAM_TARGET}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
    "${CEF_APP}/Contents/Frameworks/Chromium Embedded Framework.framework"
    VERBATIM
  )

  # 여러 Helper 앱 번들 타겟 생성
  foreach(_suffix_list ${CEF_HELPER_APP_SUFFIXES})
    # 접미사 값을 추출
    string(REPLACE ":" ";" _suffix_list ${_suffix_list})
    list(GET _suffix_list 0 _name_suffix)
    list(GET _suffix_list 1 _target_suffix)
    list(GET _suffix_list 2 _plist_suffix)

    # Helper 타겟 및 출력 이름 설정
    set(_helper_target "${CEF_HELPER_TARGET}${_target_suffix}")
    set(_helper_output_name "${CEF_HELPER_OUTPUT_NAME}${_name_suffix}")

    # Helper-specific Info.plist 파일 생성
    set(_helper_info_plist "${CMAKE_CURRENT_BINARY_DIR}/helper-Info${_target_suffix}.plist")
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/mac/helper-Info.plist.in" _plist_contents)
    string(REPLACE "\${EXECUTABLE_NAME}" "${_helper_output_name}" _plist_contents ${_plist_contents})
    string(REPLACE "\${PRODUCT_NAME}" "${_helper_output_name}" _plist_contents ${_plist_contents})
    string(REPLACE "\${BUNDLE_ID_SUFFIX}" "${_plist_suffix}" _plist_contents ${_plist_contents})
    file(WRITE ${_helper_info_plist} ${_plist_contents})

    # Helper 실행 파일 타겟 생성
    add_executable(${_helper_target} MACOSX_BUNDLE ${COGNISTREAM_HELPER_SRCS})
    SET_EXECUTABLE_TARGET_PROPERTIES(${_helper_target})
    add_dependencies(${_helper_target} libcef_dll_wrapper)
    target_link_libraries(${_helper_target} libcef_dll_wrapper ${CEF_STANDARD_LIBS})
    set_target_properties(${_helper_target} PROPERTIES
      MACOSX_BUNDLE_INFO_PLIST ${_helper_info_plist}
      OUTPUT_NAME ${_helper_output_name}
    )

    if(USE_SANDBOX)
      target_link_libraries(${_helper_target} cef_sandbox_lib)
    endif()

    # Helper를 메인 타겟의 종속성으로 추가
    add_dependencies(${COGNISTREAM_TARGET} "${_helper_target}")

    # Helper 앱 번들을 Frameworks 디렉토리에 복사
    add_custom_command(
      TARGET ${COGNISTREAM_TARGET}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
      "${CEF_TARGET_OUT_DIR}/${_helper_output_name}.app"
      "${CEF_APP}/Contents/Frameworks/${_helper_output_name}.app"
      VERBATIM
    )
  endforeach()

  # 리소스 파일 수동 처리 및 복사
  set(PREFIXES "mac/")
  COPY_MAC_RESOURCES("${COGNISTREAM_RESOURCES_SRCS}" "${PREFIXES}" "${COGNISTREAM_TARGET}" "${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_APP}")
endif()


#
# Windows 구성
#

if(OS_WINDOWS)
  # 실행 파일 타겟 생성
  add_executable(${COGNISTREAM_TARGET} WIN32 ${COGNISTREAM_SRCS})
  add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
  SET_EXECUTABLE_TARGET_PROPERTIES(${COGNISTREAM_TARGET})
  target_link_libraries(${COGNISTREAM_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})

  if(USE_SANDBOX)
    # cef_sandbox 라이브러리를 링크하기 위한 논리 타겟
    ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
    target_link_libraries(${COGNISTREAM_TARGET} cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
  endif()

  # 실행 파일에 커스텀 매니페스트 파일 추가
  ADD_WINDOWS_MANIFEST("${CMAKE_CURRENT_SOURCE_DIR}/win" "${COGNISTREAM_TARGET}" "exe")
  ADD_WINDOWS_TORCH_DLLS(${COGNISTREAM_TARGET} ${TORCH_INSTALL_PREFIX})

  # 바이너리 및 리소스 파일을 타겟 출력 디렉토리에 복사
  COPY_FILES("${COGNISTREAM_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
  COPY_FILES("${COGNISTREAM_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")


endif()


#
# CEF 설정 출력
#
PRINT_CEF_CONFIG()


#
# 패키지 생성 설정
#

# if(OS_WINDOWS)
#   set(CPACK_GENERATOR "NSIS")
# elseif(OS_MAC)
#   set(CPACK_GENERATOR "DragNDrop")
# elseif(OS_LINUX)
#   set(CPACK_GENERATOR "TGZ")
# endif()

# set(CPACK_PACKAGE_NAME "cognistream")
# set(CPACK_PACKAGE_VENDOR "jeongro.net")
# set(CPACK_PACKAGE_VERSION "0.0.1")
# set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")

# install(TARGETS ${COGNISTREAM_TARGET} DESTINATION .)

# include(CPack)