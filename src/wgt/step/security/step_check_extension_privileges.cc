// Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/security/step_check_extension_privileges.h"
#include <pkgmgrinfo_basic.h>
#include <glob.h>
#include <sys/utsname.h>

#include <set>
#include <vector>
#include <cstdlib>
#include <string>
#include <memory>

#include "wgt/extension_config_parser.h"
#include "manifest_parser/values.h"
#include "common/certificate_validation.h"
#include "common/utils/glist_range.h"

namespace {
const char kPluginsDirectory[] = "/res/wgt/plugin/";
const char kArchArmv7l[] = "armv7l";
const char kArchI586[] = "i586";
const char kArchDefault[] = "default";
}

namespace wgt {
namespace security {

common_installer::Step::Status StepCheckExtensionPrivileges::precheck() {
  if (!context_->manifest_data.get()) {
    LOG(ERROR) << "Manifest data is not set";
    return Status::ERROR;
  }
  return Status::OK;
}
common_installer::Step::Status StepCheckExtensionPrivileges::process() {
  std::string app_ext_config_pattern(GetExtensionPath());

  manifest_x* m = context_->manifest_data.get();
  std::set<std::string> current_privileges;
  for (const char* priv : GListRange<char*>(m->privileges)) {
    current_privileges.insert(priv);
  }

  std::set<std::string> xmlFiles;
  {
    glob_t glob_result;
    glob(app_ext_config_pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
    for (unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
      xmlFiles.insert(glob_result.gl_pathv[i]);
    }
  }
  std::set<std::string> privileges;
  for (auto it = xmlFiles.begin(); it != xmlFiles.end(); ++it) {
    LOG(DEBUG) << "start to parse extension xml : " << *it;
    ExtensionConfigParser extensionParser(*it);
    std::vector<std::string> list = extensionParser.GetExtensionPrivilegeList();
    for (int i = 0 ; i < list.size() ; i++) {
      if (current_privileges.find(list[i]) == current_privileges.end()) {
        privileges.insert(list[i]);
      }
    }
  }

  if (!privileges.empty()) {
    if (!CheckPrivilegeLevel(privileges)) {
      LOG(DEBUG) << "Fail to validation of privilege";
      return Status::ERROR;
    }
    for (auto it = privileges.begin(); it != privileges.end(); ++it) {
      LOG(DEBUG) << "set list privilege : " << *it;
      m->privileges = g_list_append(m->privileges, strdup((*it).c_str()));
    }
  }
  return Status::OK;
}

std::string StepCheckExtensionPrivileges::GetExtensionPath() {
  std::string app_ext_config_pattern(context_->pkg_path.get().string());
  app_ext_config_pattern.append(kPluginsDirectory);
  struct utsname u;
  if (0 == uname(&u)) {
    std::string machine = u.machine;
    LOG(DEBUG) << "Machine archicture for user defined plugins: " << machine;
    if (!machine.empty()) {
      if (machine == kArchArmv7l) {
        app_ext_config_pattern.append(kArchArmv7l);
      } else if (machine == kArchI586) {
        app_ext_config_pattern.append(kArchI586);
      } else {
        app_ext_config_pattern.append(kArchDefault);
      }
    } else {
      LOG(ERROR) << "cannot get machine info";
      app_ext_config_pattern.append(kArchDefault);
    }
    app_ext_config_pattern.append("/");
  }
  app_ext_config_pattern.append("*");
  app_ext_config_pattern.append(".xml");
  return app_ext_config_pattern;
}

bool StepCheckExtensionPrivileges::CheckPrivilegeLevel(
    std::set<std::string> priv_set) {
  GList* privileges = nullptr;
  for (auto it = priv_set.begin(); it != priv_set.end(); ++it) {
    privileges = g_list_append(privileges, strdup((*it).c_str()));
  }
  guint size = g_list_length(privileges);
  if (size == 0) return true;

  std::string error_message;
  if (!common_installer::ValidatePrivilegeLevel(
         context_->privilege_level.get(),
         false,
         context_->manifest_data.get()->api_version,
         privileges,
         &error_message)) {
         g_list_free_full(privileges, free);
    if (!error_message.empty()) {
      LOG(ERROR) << "error_message: " << error_message;
    }
    return false;
  }
  g_list_free_full(privileges, free);
  return true;
}

}  // namespace security
}  // namespace wgt
