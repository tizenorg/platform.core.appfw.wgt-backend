/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#include "wgt/step/configuration/step_parse.h"

#include <boost/filesystem/path.hpp>

#include <common/app_installer.h>
#include <common/installer_context.h>
#include <common/step/step.h>
#include <common/utils/glist_range.h>
#include <manifest_parser/utils/version_number.h>
#include <wgt_manifest_handlers/account_handler.h>
#include <wgt_manifest_handlers/app_control_handler.h>
#include <wgt_manifest_handlers/application_icons_handler.h>
#include <wgt_manifest_handlers/application_manifest_constants.h>
#include <wgt_manifest_handlers/background_category_handler.h>
#include <wgt_manifest_handlers/category_handler.h>
#include <wgt_manifest_handlers/content_handler.h>
#include <wgt_manifest_handlers/metadata_handler.h>
#include <wgt_manifest_handlers/service_handler.h>
#include <wgt_manifest_handlers/setting_handler.h>
#include <wgt_manifest_handlers/tizen_application_handler.h>
#include <wgt_manifest_handlers/widget_handler.h>
#include <wgt_manifest_handlers/ime_handler.h>

#include <pkgmgr/pkgmgr_parser.h>

#include <string.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "wgt/wgt_backend_data.h"

namespace bf = boost::filesystem;

namespace {

const char kCategoryWearableClock[] =
    "http://tizen.org/category/wearable_clock";
const char kCategoryWatchClock[] = "com.samsung.wmanager.WATCH_CLOCK";

const std::string kManifestVersion = "1.0.0";
const char kTizenPackageXmlNamespace[] = "http://tizen.org/ns/packages";

GList* GenerateMetadataListX(const wgt::parse::MetaDataInfo& meta_info) {
  GList* list = nullptr;
  for (auto& meta : meta_info.metadata()) {
    metadata_x* new_meta =
        static_cast<metadata_x*>(calloc(1, sizeof(metadata_x)));
    new_meta->key = strdup(meta.first.c_str());
    if (!meta.second.empty())
      new_meta->value = strdup(meta.second.c_str());
    list = g_list_append(list, new_meta);
  }
  return list;
}

void SetApplicationXDefaults(application_x* application) {
  application->effectimage_type = strdup("image");
  application->enabled = strdup("true");
  application->guestmode_visibility = strdup("true");
  application->hwacceleration = strdup("default");
  application->indicatordisplay = strdup("true");
  application->launchcondition = strdup("false");
  application->permission_type = strdup("normal");
  application->process_pool = strdup("false");
  application->recentimage = strdup("false");
  application->screenreader = strdup("use-system-setting");
  application->submode = strdup("false");
  application->support_disable = strdup("false");
  application->taskmanage = strdup("true");
  application->ui_gadget = strdup("false");
}

}  // namespace

namespace wgt {
namespace configuration {

namespace app_keys = wgt::application_widget_keys;
namespace sc = std::chrono;

StepParse::StepParse(common_installer::InstallerContext* context,
                     bool check_start_file)
    : Step(context),
      check_start_file_(check_start_file) {
}

std::set<std::string> StepParse::ExtractPrivileges(
    std::shared_ptr<const wgt::parse::PermissionsInfo> perm_info) const {
  return perm_info->GetAPIPermissions();
}

std::string StepParse::GetPackageVersion(
     const std::string& manifest_version) {
  if (manifest_version.empty()) {
    return kManifestVersion;
  }
  std::string version = manifest_version.substr(0,
      manifest_version.find_first_not_of("1234567890."));

  utils::VersionNumber version_number(version);

  if (!version_number.IsValidTizenPackageVersion()) {
    LOG(WARNING) << "Version number: " << manifest_version
                 << " is not valid version number for tizen package. "
                 << "Default value will be used.";
    return kManifestVersion;
  }

  return version_number.ToString();
}

bool StepParse::FillInstallationInfo(manifest_x* manifest) {
  manifest->root_path = strdup(
      (context_->root_application_path.get() / manifest->package).c_str());
  manifest->installed_time =
      strdup(std::to_string(sc::system_clock::to_time_t(
          sc::system_clock::now())).c_str());
  return true;
}

bool StepParse::FillIconPaths(manifest_x* manifest) {
  std::shared_ptr<const wgt::parse::TizenApplicationInfo> app_info =
      std::static_pointer_cast<const wgt::parse::TizenApplicationInfo>(
          parser_->GetManifestData(app_keys::kTizenApplicationKey));
  if (!app_info) {
    LOG(ERROR) << "Application info manifest data has not been found.";
    return false;
  }
  std::shared_ptr<const wgt::parse::ApplicationIconsInfo> icons_info =
      std::static_pointer_cast<const wgt::parse::ApplicationIconsInfo>(
          parser_->GetManifestData(app_keys::kIconsKey));
  if (icons_info.get()) {
    for (auto& application_icon : icons_info->icons()) {
      icon_x* icon = reinterpret_cast<icon_x*> (calloc(1, sizeof(icon_x)));
      bf::path icon_path = context_->root_application_path.get()
          / app_info->package() / "res" / "wgt" / application_icon.path();
      icon->text = strdup(icon_path.c_str());
      icon->lang = strdup(DEFAULT_LOCALE);
      manifest->icon = g_list_append(manifest->icon, icon);
    }
  }
  return true;
}

bool StepParse::FillWidgetInfo(manifest_x* manifest) {
  std::shared_ptr<const wgt::parse::WidgetInfo> wgt_info =
      std::static_pointer_cast<const wgt::parse::WidgetInfo>(
          parser_->GetManifestData(app_keys::kWidgetKey));
  if (!wgt_info.get()) {
    LOG(ERROR) << "Widget info manifest data has not been found.";
    return false;
  }

  const std::string& version = wgt_info->version();

  manifest->ns = strdup(kTizenPackageXmlNamespace);
  manifest->version = strdup(GetPackageVersion(version).c_str());

  for (auto& item : wgt_info->description_set()) {
    description_x* description = reinterpret_cast<description_x*>
        (calloc(1, sizeof(description_x)));
    description->text = strdup(item.second.c_str());
    description->lang = !item.first.empty() ?
        strdup(item.first.c_str()) : strdup(DEFAULT_LOCALE);
    manifest->description = g_list_append(manifest->description, description);
  }

  for (auto& item : wgt_info->name_set()) {
    label_x* label = reinterpret_cast<label_x*>(calloc(1, sizeof(label_x)));
    label->name = strdup(item.second.c_str());
    label->text = strdup(item.second.c_str());
    label->lang = !item.first.empty() ?
        strdup(item.first.c_str()) : strdup(DEFAULT_LOCALE);
    manifest->label = g_list_append(manifest->label, label);
  }

  manifest->type = strdup("wgt");
  manifest->appsetting = strdup("false");
  manifest->nodisplay_setting = strdup("false");
  manifest->preload = strdup("false");

  // For wgt package use the long name
  for (auto& item : wgt_info->name_set()) {
    application_x* app =
        reinterpret_cast<application_x*>(manifest->application->data);
    label_x* label = reinterpret_cast<label_x*>(calloc(1, sizeof(label_x)));
    label->name = strdup(item.second.c_str());
    label->text = strdup(item.second.c_str());
    label->lang = !item.first.empty() ?
        strdup(item.first.c_str()) : strdup(DEFAULT_LOCALE);
    app->label = g_list_append(app->label, label);
  }

  author_x* author = reinterpret_cast<author_x*>(calloc(1, sizeof(author_x)));
  if (!wgt_info->author().empty())
    author->text = strdup(wgt_info->author().c_str());
  if (!wgt_info->author_email().empty())
    author->email = strdup(wgt_info->author_email().c_str());
  if (!wgt_info->author_href().empty())
    author->href = strdup(wgt_info->author_href().c_str());
  author->lang = strdup(DEFAULT_LOCALE);
  manifest->author = g_list_append(manifest->author, author);

  std::shared_ptr<const wgt::parse::SettingInfo> settings_info =
      std::static_pointer_cast<const wgt::parse::SettingInfo>(
          parser_->GetManifestData(
              wgt::application_widget_keys::kTizenSettingKey));
  if (settings_info) {
    switch (settings_info->install_location()) {
    case wgt::parse::SettingInfo::InstallLocation::AUTO: {
      manifest->installlocation = strdup("auto");
      break;
    }
    case wgt::parse::SettingInfo::InstallLocation::INTERNAL: {
      manifest->installlocation = strdup("internal-only");
      break;
    }
    case wgt::parse::SettingInfo::InstallLocation::EXTERNAL: {
      manifest->installlocation = strdup("prefer-external");
      break;
    }
    }
  } else {
    manifest->installlocation = strdup("auto");
  }

  return true;
}

bool StepParse::FillMainApplicationInfo(manifest_x* manifest) {
  std::shared_ptr<const wgt::parse::TizenApplicationInfo> app_info =
      std::static_pointer_cast<const wgt::parse::TizenApplicationInfo>(
          parser_->GetManifestData(app_keys::kTizenApplicationKey));
  if (!app_info) {
    LOG(ERROR) << "Application info manifest data has not been found.";
    return false;
  }
  bool has_watch_catergory = false;
  std::shared_ptr<const wgt::parse::CategoryInfoList> category_info =
      std::static_pointer_cast<const wgt::parse::CategoryInfoList>(
          parser_->GetManifestData(app_keys::kTizenCategoryKey));
  if (category_info) {
    has_watch_catergory = std::find_if(category_info->categories.begin(),
                                       category_info->categories.end(),
                                       [](const std::string& category) {
      return category == kCategoryWearableClock ||
             category == kCategoryWatchClock;
    }) != category_info->categories.end();
  }

  // application data
  application_x* application = reinterpret_cast<application_x*>(
      calloc(1, sizeof(application_x)));
  application->component_type =
      has_watch_catergory ? strdup("watchapp") : strdup("uiapp");
  application->mainapp = strdup("true");
  application->nodisplay = strdup("false");
  application->multiple = strdup("false");
  application->appid = strdup(app_info->id().c_str());
  SetApplicationXDefaults(application);
  if (has_watch_catergory)
    application->ambient_support =
        strdup(app_info->ambient_support() ? "true" : "false");
  else
    application->ambient_support = strdup("false");
  application->package = strdup(app_info->package().c_str());

  application->exec =
      strdup((context_->root_application_path.get() / app_info->package()
              / "bin" / application->appid).c_str());
  application->type = strdup("webapp");
  application->onboot = strdup("false");
  application->autorestart = strdup("false");

  application->launch_mode = strdup(app_info->launch_mode().c_str());
  if (manifest->icon) {
    icon_x* icon = reinterpret_cast<icon_x*>(manifest->icon->data);
    icon_x* app_icon = reinterpret_cast<icon_x*>(calloc(1, sizeof(icon_x)));
    app_icon->text = strdup(icon->text);
    app_icon->lang = strdup(icon->lang);
    application->icon = g_list_append(application->icon, app_icon);
  }
  manifest->application = g_list_append(manifest->application, application);

  manifest->package = strdup(app_info->package().c_str());
  manifest->mainapp_id = strdup(app_info->id().c_str());
  return true;
}

bool StepParse::FillServiceApplicationInfo(manifest_x* manifest) {
  std::shared_ptr<const wgt::parse::ServiceList> service_list =
      std::static_pointer_cast<const wgt::parse::ServiceList>(
          parser_->GetManifestData(app_keys::kTizenServiceKey));
  if (!service_list)
    return true;
  for (auto& service_info : service_list->services) {
    application_x* application = reinterpret_cast<application_x*>
        (calloc(1, sizeof(application_x)));
    application->component_type = strdup("svcapp");
    application->mainapp = strdup("false");
    application->nodisplay = strdup("false");
    application->multiple = strdup("false");
    application->appid = strdup(service_info.id().c_str());
    application->exec =
        strdup((context_->root_application_path.get() / manifest->package
                / "bin" / application->appid).c_str());
    application->type = strdup("webapp");
    application->onboot =
        service_info.on_boot() ? strdup("true") : strdup("false");
    application->autorestart =
        service_info.auto_restart() ? strdup("true") : strdup("false");
    SetApplicationXDefaults(application);
    application->ambient_support = strdup("false");
    application->package = strdup(manifest->package);

    for (auto& pair : service_info.names()) {
      label_x* label = reinterpret_cast<label_x*>(calloc(1, sizeof(label_x)));
      label->lang = !pair.first.empty() ?
          strdup(pair.first.c_str()) : strdup(DEFAULT_LOCALE);
      label->name = strdup(pair.second.c_str());
      label->text = strdup(pair.second.c_str());
      application->label = g_list_append(application->label, label);
    }

    if (!service_info.icon().empty()) {
      icon_x* icon = reinterpret_cast<icon_x*>(calloc(1, sizeof(icon_x)));
      icon->text = strdup(service_info.icon().c_str());
      icon->lang = strdup(DEFAULT_LOCALE);
      application->icon = g_list_append(application->icon, icon);
    }

    // TODO(t.iwanek): what about description, how is it different from name?

    for (auto& category : service_info.categories()) {
      application->category = g_list_append(application->category,
                                            strdup(category.c_str()));
    }

    for (auto& pair : service_info.metadata_set()) {
      metadata_x* item = reinterpret_cast<metadata_x*>(
          calloc(1, sizeof(metadata_x)));
      item->key = strdup(pair.first.c_str());
      if (!pair.second.empty())
        item->value = strdup(pair.second.c_str());
      application->metadata = g_list_append(application->metadata, item);
    }

    manifest->application = g_list_append(manifest->application, application);
  }
  return true;
}

bool StepParse::FillBackgroundCategoryInfo(manifest_x* manifest) {
  auto manifest_data = parser_->GetManifestData(
      app_keys::kTizenBackgroundCategoryKey);
  std::shared_ptr<const wgt::parse::BackgroundCategoryInfoList> bc_list =
      std::static_pointer_cast<const wgt::parse::BackgroundCategoryInfoList>(
          manifest_data);

  if (!bc_list)
    return true;

  application_x* app =
      reinterpret_cast<application_x*>(manifest->application->data);

  for (auto& background_category : bc_list->background_categories) {
    app->background_category = g_list_append(
        app->background_category, strdup(background_category.value().c_str()));
  }

  return true;
}

bool StepParse::FillAppControl(manifest_x* manifest) {
  std::shared_ptr<const wgt::parse::AppControlInfoList> app_info_list =
      std::static_pointer_cast<const wgt::parse::AppControlInfoList>(
          parser_->GetManifestData(app_keys::kTizenApplicationAppControlsKey));

  application_x* app =
      reinterpret_cast<application_x*>(manifest->application->data);
  if (app_info_list) {
    for (const auto& control : app_info_list->controls) {
      appcontrol_x* app_control =
          static_cast<appcontrol_x*>(calloc(1, sizeof(appcontrol_x)));
      app_control->operation = strdup(control.operation().c_str());
      app_control->mime = strdup(control.mime().c_str());
      app_control->uri = strdup(control.uri().c_str());
      app->appcontrol = g_list_append(app->appcontrol, app_control);
    }
  }
  return true;
}

bool StepParse::FillPrivileges(manifest_x* manifest) {
  std::shared_ptr<const wgt::parse::PermissionsInfo> perm_info =
      std::static_pointer_cast<const wgt::parse::PermissionsInfo>(
          parser_->GetManifestData(app_keys::kTizenPermissionsKey));
  std::set<std::string> privileges;
  if (perm_info)
    privileges = ExtractPrivileges(perm_info);

  for (auto& priv : privileges) {
    manifest->privileges =
        g_list_append(manifest->privileges, strdup(priv.c_str()));
  }
  return true;
}

bool StepParse::FillCategories(manifest_x* manifest) {
  std::shared_ptr<const wgt::parse::CategoryInfoList> category_info =
      std::static_pointer_cast<const wgt::parse::CategoryInfoList>(
          parser_->GetManifestData(app_keys::kTizenCategoryKey));
  if (!category_info)
    return true;

  application_x* app =
      reinterpret_cast<application_x*>(manifest->application->data);
  // there is one app atm
  for (auto& category : category_info->categories) {
    app->category = g_list_append(app->category, strdup(category.c_str()));
  }
  return true;
}

bool StepParse::FillMetadata(manifest_x* manifest) {
  std::shared_ptr<const wgt::parse::MetaDataInfo> meta_info =
      std::static_pointer_cast<const wgt::parse::MetaDataInfo>(
          parser_->GetManifestData(app_keys::kTizenMetaDataKey));
  if (!meta_info)
    return true;

  for (application_x* app : GListRange<application_x*>(manifest->application)) {
    app->metadata = GenerateMetadataListX(*meta_info);
  }
  return true;
}

bool StepParse::FillAccounts(manifest_x* manifest) {
  std::shared_ptr<const wgt::parse::AccountInfo> account_info =
      std::static_pointer_cast<const wgt::parse::AccountInfo>(
          parser_->GetManifestData(app_keys::kAccountKey));
  if (!account_info)
    return true;
  common_installer::AccountInfo info;
  for (auto& account : account_info->accounts()) {
    common_installer::SingleAccountInfo single_info;
    single_info.capabilities = account.capabilities;
    single_info.icon_paths = account.icon_paths;
    single_info.multiple_account_support = account.multiple_account_support;
    single_info.names = account.names;
    // wgt can contain only one app so this assumes mainapp_id is valid here
    single_info.appid = manifest->mainapp_id;
    info.set_account(single_info);
  }
  context_->manifest_plugins_data.get().account_info.set(info);
  return true;
}

bool StepParse::FillImeInfo() {
  const auto ime_info = std::static_pointer_cast<const wgt::parse::ImeInfo>(
      parser_->GetManifestData(app_keys::kTizenImeKey));
  if (!ime_info)
    return true;

  common_installer::ImeInfo info;
  info.setUuid(ime_info->uuid());

  const auto &languages = ime_info->languages();
  for (const auto &language : languages)
    info.AddLanguage(language);

  context_->manifest_plugins_data.get().ime_info.set(std::move(info));
  return true;
}

bool StepParse::FillExtraManifestInfo(manifest_x* manifest) {
  return FillAccounts(manifest) && FillImeInfo();
}

bool StepParse::FillManifestX(manifest_x* manifest) {
  if (!FillIconPaths(manifest))
    return false;
  if (!FillMainApplicationInfo(manifest))
    return false;
  if (!FillWidgetInfo(manifest))
    return false;
  if (!FillInstallationInfo(manifest))
    return false;
  if (!FillPrivileges(manifest))
    return false;
  if (!FillAppControl(manifest))
    return false;
  if (!FillCategories(manifest))
    return false;
  if (!FillMetadata(manifest))
    return false;
  // TODO(t.iwanek): fix adding ui application element
  // for now adding application service is added here because rest of code
  // assumes that there is one application at manifest->application
  // so this must execute last
  if (!FillServiceApplicationInfo(manifest))
    return false;
  if (!FillBackgroundCategoryInfo(manifest))
    return false;
  if (!FillExtraManifestInfo(manifest))
    return false;
  return true;
}

bool StepParse::LocateConfigFile() {
  return StepParse::Check(context_->unpacked_dir_path.get());
}

common_installer::Step::Status StepParse::process() {
  if (!LocateConfigFile()) {
    LOG(ERROR) << "No config.xml";
    return common_installer::Step::Status::MANIFEST_NOT_FOUND;
  }

  parser_.reset(new wgt::parse::WidgetConfigParser());
  if (!parser_->ParseManifest(config_)) {
    LOG(ERROR) << "[Parse] Parse failed. " <<  parser_->GetErrorMessage();
    return common_installer::Step::Status::PARSE_ERROR;
  }
  if (check_start_file_) {
    if (!parser_->HasValidStartFile()) {
      LOG(ERROR) << parser_->GetErrorMessage();
      return common_installer::Step::Status::PARSE_ERROR;
    }
    if (!parser_->HasValidServicesStartFiles()) {
      LOG(ERROR) << parser_->GetErrorMessage();
      return common_installer::Step::Status::PARSE_ERROR;
    }
  }

  manifest_x* manifest =
      static_cast<manifest_x*>(calloc(1, sizeof(manifest_x)));
  if (!FillManifestX(manifest)) {
    LOG(ERROR) << "[Parse] Storing manifest_x failed. "
               <<  parser_->GetErrorMessage();
    return common_installer::Step::Status::PARSE_ERROR;
  }

  // Copy data from ManifestData to InstallerContext
  std::shared_ptr<const wgt::parse::TizenApplicationInfo> info =
      std::static_pointer_cast<const wgt::parse::TizenApplicationInfo>(
          parser_->GetManifestData(
              wgt::application_widget_keys::kTizenApplicationKey));
  std::shared_ptr<const wgt::parse::WidgetInfo> wgt_info =
      std::static_pointer_cast<const wgt::parse::WidgetInfo>(
          parser_->GetManifestData(
              wgt::application_widget_keys::kTizenWidgetKey));

  std::string name;
  const auto& name_set = wgt_info->name_set();
  if (name_set.find("") != name_set.end())
    name = name_set.find("")->second;
  if (name_set.begin() != name_set.end())
    name = name_set.begin()->second;

  std::string short_name;
  const auto& short_name_set = wgt_info->short_name_set();
  if (short_name_set.find("") != short_name_set.end())
    short_name = short_name_set.find("")->second;
  if (short_name_set.begin() != short_name_set.end())
    short_name = short_name_set.begin()->second;

  const std::string& package_version = wgt_info->version();
  const std::string& required_api_version = info->required_version();

  manifest->api_version = strdup(required_api_version.c_str());

  context_->pkgid.set(manifest->package);

  // write pkgid for recovery file
  if (context_->recovery_info.get().recovery_file) {
    context_->recovery_info.get().recovery_file->set_pkgid(manifest->package);
    context_->recovery_info.get().recovery_file->WriteAndCommitFileContent();
  }

  std::shared_ptr<const wgt::parse::PermissionsInfo> perm_info =
      std::static_pointer_cast<const wgt::parse::PermissionsInfo>(
          parser_->GetManifestData(
              wgt::application_widget_keys::kTizenPermissionsKey));
  parser::PermissionSet permissions;
  if (perm_info)
     permissions = perm_info->GetAPIPermissions();

  WgtBackendData* backend_data =
      static_cast<WgtBackendData*>(context_->backend_data.get());

  std::shared_ptr<const wgt::parse::SettingInfo> settings_info =
      std::static_pointer_cast<const wgt::parse::SettingInfo>(
          parser_->GetManifestData(
              wgt::application_widget_keys::kTizenSettingKey));
  if (settings_info)
    backend_data->settings.set(*settings_info);

  LOG(DEBUG) << " Read data -[ ";
  LOG(DEBUG) << "App id: " << info->id();
  LOG(DEBUG) << "  package     = " <<  info->package();
  LOG(DEBUG) << "  id          = " <<  info->id();
  LOG(DEBUG) << "  name        = " <<  name;
  LOG(DEBUG) << "  short_name  = " <<  short_name;
  LOG(DEBUG) << "  aplication version     = " <<  package_version;
  LOG(DEBUG) << "  api_version = " <<  info->required_version();
  LOG(DEBUG) << "  launch_mode = " <<  info->launch_mode();
  LOG(DEBUG) << "  privileges -[";
  for (const auto& p : permissions) {
    LOG(DEBUG) << "    " << p;
  }
  LOG(DEBUG) << "  ]-";
  LOG(DEBUG) << "]-";

  // TODO(t.iwanek): In delta mode this step is running two times
  if (context_->manifest_data.get())
    pkgmgr_parser_free_manifest_xml(context_->manifest_data.get());

  context_->manifest_data.set(manifest);
  return common_installer::Step::Status::OK;
}

bool StepParse::Check(const boost::filesystem::path& widget_path) {
  boost::filesystem::path config = widget_path / "config.xml";

  LOG(DEBUG) << "config.xml path: " << config;

  if (!boost::filesystem::exists(config))
    return false;

  config_ = config;
  return true;
}

}  // namespace configuration
}  // namespace wgt
