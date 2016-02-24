/* 2014, Copyright © Intel Coporation, license APACHE-2.0, see LICENSE file */
// Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a apache 2.0 license that can be
// found in the LICENSE file.

#ifndef  WGT_STEP_STEP_GENERATE_XML_H_
#define  WGT_STEP_STEP_GENERATE_XML_H_

#include <manifest_parser/utils/logging.h>

#include <libxml/xmlwriter.h>

#include <common/installer_context.h>
#include <common/step/step.h>

namespace wgt {
namespace pkgmgr {

class StepGenerateXml : public common_installer::Step {
 public:
  using Step::Step;

  Status process() override;
  Status clean() override { return Status::OK; }
  Status undo() override;
  Status precheck() override;

 private:
  enum class AppCompType {
    UIAPP,
    SVCAPP,
    WIDGETAPP,
    WATCHAPP
  };

  Step::Status GenerateApplicationCommonXml(application_x* app,
                                            xmlTextWriterPtr writer,
                                            AppCompType type);

  SCOPE_LOG_TAG(GenerateXML)
};

}  // namespace pkgmgr
}  // namespace wgt

#endif  // WGT_STEP_STEP_GENERATE_XML_H_
