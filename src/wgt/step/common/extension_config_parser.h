#ifndef SRC_WGT_STEP_COMMON_EXTENSION_CONFIG_PARSER_H_
#define SRC_WGT_STEP_COMMON_EXTENSION_CONFIG_PARSER_H_

#include <libxml2/libxml/tree.h>
#include <manifest_parser/utils/logging.h>
#include <manifest_parser/values.h>
#include <manifest_parser/manifest_util.h>

#include <string>
#include <map>
#include <vector>

namespace wgt {
    std::unique_ptr<parser::DictionaryValue> LoadExtensionConfig(
        std::string configXml);
    std::string GetNodeDir(xmlNode* node, const std::string& inherit_dir);
    std::string GetNodeText(xmlNode* root, const std::string& inherit_dir);
    bool IsPropSupportDir(xmlNode* root, xmlAttr* prop);
    bool IsTrimRequiredForElement(xmlNode* root);
    bool IsTrimRequiredForProp(xmlNode* root, xmlAttr* prop);
    std::unique_ptr<parser::DictionaryValue> LoadXMLNode(
        xmlNode* root, const std::string& inherit_dir = "");
    std::vector<std::string> GetExtensionPrivilegeList(std::string configXml);
}  // namespace wgt

#endif  // SRC_WGT_STEP_COMMON_EXTENSION_CONFIG_PARSER_H_
