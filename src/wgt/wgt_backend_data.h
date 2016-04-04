// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef WGT_WGT_BACKEND_DATA_H_
#define WGT_WGT_BACKEND_DATA_H_

#include <common/installer_context.h>
#include <common/utils/property.h>

#include <wgt_manifest_handlers/setting_handler.h>

#include <string>
#include <vector>

namespace wgt {

/**
 * \brief Class that is used within specific backends to keep additional
 *        information regarding package
 */
class WgtBackendData : public common_installer::BackendData {
 public:
  /**
   * \brief Property of SettingInfo
   */
  Property<parse::SettingInfo> settings;
};

}  // namespace wgt

#endif  // WGT_WGT_BACKEND_DATA_H_
