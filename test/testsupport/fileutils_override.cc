/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "test/testsupport/fileutils_override.h"

#if defined(WEBRTC_WIN)
#include <direct.h>
#include <tchar.h>
#include <windows.h>
#include <algorithm>
#include <codecvt>
#include <locale>

#include "Shlwapi.h"
#include "WinDef.h"

#include "rtc_base/win32.h"
#define GET_CURRENT_DIR _getcwd
#else
#include <dirent.h>
#include <unistd.h>
#define GET_CURRENT_DIR getcwd
#endif

#if defined(WEBRTC_IOS)
#include "test/testsupport/iosfileutils.h"
#endif

#if defined(WEBRTC_MAC)
#include "test/testsupport/macfileutils.h"
#endif

#include "rtc_base/arraysize.h"
#include "rtc_base/checks.h"
#include "rtc_base/stringutils.h"

namespace webrtc {
namespace test {

std::string DirName(const std::string& path);
bool CreateDir(const std::string& directory_name);

namespace internal {

namespace {
#if defined(WEBRTC_WIN)
const char* kPathDelimiter = "\\";
#elif !defined(WEBRTC_IOS)
const char* kPathDelimiter = "/";
#endif

#if defined(WEBRTC_ANDROID)
// This is a special case in Chrome infrastructure. See
// base/test/test_support_android.cc.
const char* kAndroidChromiumTestsRoot = "/sdcard/chromium_tests_root/";
#endif

#if !defined(WEBRTC_IOS)
const char* kResourcesDirName = "resources";
#endif

}  // namespace

const char* kCannotFindProjectRootDir = "ERROR_CANNOT_FIND_PROJECT_ROOT_DIR";

// Finds the WebRTC src dir.
// The returned path always ends with a path separator.
std::string ProjectRootPath() {
#if defined(WEBRTC_ANDROID)
  return kAndroidChromiumTestsRoot;
#elif defined WEBRTC_IOS
  return IOSRootPath();
#elif defined(WEBRTC_MAC)
  std::string path;
  GetNSExecutablePath(&path);
  std::string exe_dir = DirName(path);
  // On Mac, tests execute in out/Whatever, so src is two levels up except if
  // the test is bundled (which our tests are not), in which case it's 5 levels.
  return DirName(DirName(exe_dir)) + kPathDelimiter;
#elif defined(WEBRTC_POSIX)
  char buf[PATH_MAX];
  ssize_t count = ::readlink("/proc/self/exe", buf, arraysize(buf));
  if (count <= 0) {
    RTC_NOTREACHED() << "Unable to resolve /proc/self/exe.";
    return kCannotFindProjectRootDir;
  }
  // On POSIX, tests execute in out/Whatever, so src is two levels up.
  std::string exe_dir = DirName(std::string(buf, count));
  return DirName(DirName(exe_dir)) + kPathDelimiter;
#elif defined(WEBRTC_WIN)
  wchar_t buf[MAX_PATH];
  buf[0] = 0;
  if (GetModuleFileName(NULL, buf, MAX_PATH) == 0)
    return kCannotFindProjectRootDir;

  std::string exe_path = rtc::ToUtf8(std::wstring(buf));
  std::string exe_dir = DirName(exe_path);
  return DirName(DirName(exe_dir)) + kPathDelimiter;
#endif
}

std::string OutputPath() {
#if defined(WEBRTC_IOS)
  return IOSOutputPath();
#elif defined(WEBRTC_ANDROID)
  return kAndroidChromiumTestsRoot;
#else
  std::string path = ProjectRootPath();
  RTC_DCHECK_NE(path, kCannotFindProjectRootDir);
  path += "out";
  if (!CreateDir(path)) {
    return "./";
  }
  return path + kPathDelimiter;
#endif
}

std::string WorkingDir() {
#if defined(WEBRTC_ANDROID)
  return kAndroidChromiumTestsRoot;
#endif
  char path_buffer[FILENAME_MAX];
  if (!GET_CURRENT_DIR(path_buffer, sizeof(path_buffer))) {
    fprintf(stderr, "Cannot get current directory!\n");
    return "./";
  } else {
    return std::string(path_buffer);
  }
}

std::string ResourcePath(const std::string& name,
                         const std::string& extension) {
#if defined(WEBRTC_IOS)
  return IOSResourcePath(name, extension);
#else
  std::string resources_path =
      ProjectRootPath() + kResourcesDirName + kPathDelimiter;
  return resources_path + name + "." + extension;
#endif
}

}  // namespace internal
}  // namespace test
}  // namespace webrtc