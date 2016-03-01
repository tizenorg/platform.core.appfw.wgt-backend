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
#include <vector>

#include "wgt/step/step_parse.h"
#include "wgt/wgt_backend_data.h"

#define ASSERT_CSTR_EQ(STR1, STR2)                                             \
  ASSERT_EQ(strcmp(STR1, STR2), 0)                                             \

namespace bf = boost::filesystem;
namespace ci = common_installer;

namespace {

const char kManifestTestcaseData[] =
    "/usr/share/wgt-backend-ut/test_samples/manifest/";

template<typename T>
gint Size(GListRange<T>* range) {
  return std::distance(range->begin(), range->end());
}

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
  ASSERT_EQ(Size(&apps), 1);
  application_x* app = *apps.begin();
  ASSERT_CSTR_EQ(app->appid, "package0id.appid");
}

TEST_F(ManifestTest, WidgetElement_InvalidNamespace) {
  StepParseRunner runner(GetMyName());
  ASSERT_FALSE(runner.Run());
}

TEST_F(ManifestTest, WidgetElement_MissingNamespace) {
  StepParseRunner runner(GetMyName());
  ASSERT_FALSE(runner.Run());
}

TEST_F(ManifestTest, WidgetElement_ManyNamespaces) {
  StepParseRunner runner(GetMyName());
  ASSERT_FALSE(runner.Run());
}

TEST_F(ManifestTest, ApplicationElement_ManyElements) {
  StepParseRunner runner(GetMyName());
  ASSERT_FALSE(runner.Run());
}

TEST_F(ManifestTest, ApplicationElement_ValidAppId) {
  StepParseRunner runner(GetMyName());
  ASSERT_TRUE(runner.Run());
  manifest_x* m = runner.GetManifest();
  ASSERT_NE(m, nullptr);
  auto apps = GListRange<application_x*>(m->application);
  application_x* app = *apps.begin();
  ASSERT_NE(app, nullptr);
  ASSERT_CSTR_EQ(app->appid, "package0id.appid");
}

TEST_F(ManifestTest, ApplicationElement_MissingAppId) {
  StepParseRunner runner(GetMyName());
  ASSERT_FALSE(runner.Run());
}

TEST_F(ManifestTest, ApplicationElement_InvalidAppId) {
  StepParseRunner runner(GetMyName());
  ASSERT_FALSE(runner.Run());
}

TEST_F(ManifestTest, ApplicationElement_ValidPackage) {
  StepParseRunner runner(GetMyName());
  ASSERT_TRUE(runner.Run());
  manifest_x* m = runner.GetManifest();
  ASSERT_NE(m, nullptr);
  ASSERT_CSTR_EQ(m->package, "package0id");
}

TEST_F(ManifestTest, ApplicationElement_InvalidPackage) {
  StepParseRunner runner(GetMyName());
  ASSERT_FALSE(runner.Run());
}

TEST_F(ManifestTest, ApplicationElement_MissingPackage) {
  StepParseRunner runner(GetMyName());
  ASSERT_FALSE(runner.Run());
}

TEST_F(ManifestTest, ApplicationElement_ValidVersion) {
  StepParseRunner runner(GetMyName());
  ASSERT_TRUE(runner.Run());
  manifest_x* m = runner.GetManifest();
  ASSERT_NE(m, nullptr);
  ASSERT_CSTR_EQ(m->version, "3.0");
}

TEST_F(ManifestTest, ApplicationElement_InvalidVersion) {
  StepParseRunner runner(GetMyName());
  ASSERT_FALSE(runner.Run());
}

TEST_F(ManifestTest, ApplicationElement_MissingVersion) {
  StepParseRunner runner(GetMyName());
  ASSERT_FALSE(runner.Run());
}

TEST_F(ManifestTest, PrivilegeElement_ValidName) {
  StepParseRunner runner(GetMyName());
  ASSERT_TRUE(runner.Run());
  manifest_x* m = runner.GetManifest();
  ASSERT_NE(m, nullptr);
  auto apps = GListRange<application_x*>(m->application);
  application_x* app = *apps.begin();
  std::vector<std::string> priv_vec;
  for (const char* priv : GListRange<char*>(m->privileges)) {
    priv_vec.push_back(priv);
  }
  ASSERT_FALSE(priv_vec.empty());
  const char* expected_name = "http://tizen.org/privilege/application.launch";
  ASSERT_CSTR_EQ(priv_vec[0].c_str(), expected_name);
}

TEST_F(ManifestTest, PrivilegeElement_ManyElements) {
  StepParseRunner runner(GetMyName());
  ASSERT_TRUE(runner.Run());
  manifest_x* m = runner.GetManifest();
  ASSERT_NE(m, nullptr);
  auto apps = GListRange<application_x*>(m->application);
  application_x* app = *apps.begin();
  std::vector<std::string> priv_vec;
  for (const char* priv : GListRange<char*>(m->privileges)) {
    priv_vec.push_back(priv);
  }
  ASSERT_EQ(priv_vec.size(), 2);
  const char* first_priv = "http://tizen.org/privilege/application.launch";
  ASSERT_CSTR_EQ(priv_vec[0].c_str(), first_priv);
  const char* second_priv = "http://tizen.org/privilege/application.close";
  ASSERT_CSTR_EQ(priv_vec[1].c_str(), second_priv);
}

TEST_F(ManifestTest, PrivilegeElement_InvalidName) {
  StepParseRunner runner(GetMyName());
  ASSERT_FALSE(runner.Run());
}

TEST_F(ManifestTest, PrivilegeElement_MissingName) {
  StepParseRunner runner(GetMyName());
  ASSERT_FALSE(runner.Run());
}

TEST_F(ManifestTest, AppControlElement_Valid) {
  StepParseRunner runner(GetMyName());
  ASSERT_TRUE(runner.Run());
  manifest_x* m = runner.GetManifest();
  ASSERT_NE(m, nullptr);
  auto apps = GListRange<application_x*>(m->application);
  application_x* app = *apps.begin();
  ASSERT_NE(app, nullptr);
  std::vector<appcontrol_x*> app_controls;
  for (appcontrol_x* app_control : GListRange<appcontrol_x*>(app->appcontrol)) {
    app_controls.push_back(app_control);
  }
  ASSERT_EQ(app_controls.size(), 1);
  ASSERT_CSTR_EQ(app_controls[0]->mime, "image/jpg");
  ASSERT_CSTR_EQ(app_controls[0]->operation,
                 "http://tizen.org/appcontrol/operation/edit");
  ASSERT_CSTR_EQ(app_controls[0]->uri, "myapp");
}

TEST_F(ManifestTest, AppControlElement_MissingMIME) {
  StepParseRunner runner(GetMyName());
  ASSERT_FALSE(runner.Run());
}

TEST_F(ManifestTest, AppControlElement_MissingOperation) {
  StepParseRunner runner(GetMyName());
  ASSERT_FALSE(runner.Run());
}

TEST_F(ManifestTest, AppControlElement_MissingSrc) {
  StepParseRunner runner(GetMyName());
  ASSERT_FALSE(runner.Run());
}

TEST_F(ManifestTest, AppControlElement_MissingURI) {
  StepParseRunner runner(GetMyName());
  ASSERT_FALSE(runner.Run());
}



