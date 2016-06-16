#ifndef EXTENSION_CONFIG_PARSER_H_
#define EXTENSION_CONFIG_PARSER_H_

#include <libxml2/libxml/tree.h>

#include <string>
#include <map>
//#include <memory>
//#include <regex>
#include <vector>
#include <manifest_parser/utils/logging.h>
#include <manifest_parser/values.h>
#include <manifest_parser/manifest_util.h>


namespace wgt {
    std::unique_ptr<parser::DictionaryValue> LoadExtensionConfig(std::string configXml);
    std::string GetNodeDir(xmlNode* node, const std::string& inherit_dir);
    std::string GetNodeText(xmlNode* root, const std::string& inherit_dir);
    bool IsPropSupportDir(xmlNode* root, xmlAttr* prop);
    bool IsTrimRequiredForElement(xmlNode* root);
    bool IsTrimRequiredForProp(xmlNode* root, xmlAttr* prop);
    std::unique_ptr<parser::DictionaryValue> LoadXMLNode(xmlNode* root, const std::string& inherit_dir = "");
    std::vector<std::string> GetExtensionPrivilegeList(std::string configXml);
}

#endif  // EXTENSION_CONFIG_PARSER_H_
