// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by an apache-2.0 license that can be
// found in the LICENSE file.

#include <boost/filesystem/path.hpp>

#include <common/installer_context.h>
#include <common/request.h>
#include <common/utils/glist_range.h>

#include <glib.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "wgt/step/step_parse.h"
#include "wgt/wgt_backend_data.h"

#define ASSERT_CSTR_EQ(STR1, STR2)                                             \
  ASSERT_EQ(strcmp(STR1, STR2), 0)                                             \

namespace bf = boost::filesystem;
namespace ci = common_installer;

namespace {

const char kManifestTestcaseData[] =
    "/usr/share/wgt-backend-ut/test_samples/manifest/";

class StepParseRunner {
 public:
  explicit StepParseRunner(const std::string& dir_suffix,
                           bool ignore_start_files = true)
      : dir_suffix_(dir_suffix),
        ignore_start_files_(ignore_start_files),
        context_(nullptr) {
  }

  bool Run() {
    PrepareContext();
    wgt::parse::StepParse step(context_.get(), !ignore_start_files_);
    return step.process() == ci::Step::Status::OK;
  }

  manifest_x* GetManifest() const {
    return context_->manifest_data.get();
  }

 private:
  void PrepareContext() {
    context_.reset(new ci::InstallerContext());
    context_->root_application_path.set(ci::GetRootAppPath(false));
    context_->unpacked_dir_path.set(
        bf::path(kManifestTestcaseData) / dir_suffix_);
    context_->backend_data.set(new wgt::WgtBackendData());
  }

  std::string dir_suffix_;
  bool ignore_start_files_;
  std::unique_ptr<ci::InstallerContext> context_;
};

}  // namespace

class ManifestTest : public testing::Test {
 public:
  std::string GetMyName() const {
    std::string suite =
        testing::UnitTest::GetInstance()->current_test_info()->test_case_name();
    std::string case_name =
        testing::UnitTest::GetInstance()->current_test_info()->name();
    return suite + '.' + case_name;
  }
};

TEST_F(ManifestTest, WidgetElement_Valid) {
  StepParseRunner runner(GetMyName());
  ASSERT_TRUE(runner.Run());
  manifest_x* m = runner.GetManifest();
  ASSERT_NE(m, nullptr);
  ASSERT_CSTR_EQ(m->package, "package0id");
  auto apps = GListRange<application_x*>(m->application);
  ASSERT_EQ(apps.Size(), 1);
  application_x* app = *apps.begin();
  ASSERT_CSTR_EQ(app->appid, "package0id.appid");
}

TEST_F(ManifestTest, WidgetElement_InvalidNamespace) {
  StepParseRunner runner(GetMyName());
  ASSERT_FALSE(runner.Run());
}
