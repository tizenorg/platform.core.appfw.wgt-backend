#include "wgt/step/common/extension_config_parser.h"
#include <manifest_parser/utils/string_util.h>
#include <cassert>
#include <utility>

namespace wgt {

namespace {
const xmlChar kExtensionNodeKey[] = "extension";
const xmlChar kNamePrivilegeKey[] = "privilege";
const xmlChar kPrivigeNameAttributeKey[] = "name";
const char kAttributePrefix[] = "@";
const xmlChar kDirAttributeKey[] = "dir";
const char kXmlTextKey[] = "#text";
const char kNamespaceKey[] = "@namespace";
const char kExtensionPath[] = "extension.privilege";
const char kExtensionNameKey[] = "@name";
}  // namespace

std::unique_ptr<parser::DictionaryValue> LoadExtensionConfig(
    std::string configXml) {
  xmlDoc * doc = nullptr;
  xmlNode* root_node = nullptr;
  doc = xmlReadFile(configXml.c_str(), nullptr, XML_PARSE_NOENT);
  if (!doc) {
    LOG(ERROR) << "doc is empty";
    return nullptr;
  }
  root_node = xmlDocGetRootElement(doc);
  std::unique_ptr<parser::DictionaryValue> dv = LoadXMLNode(root_node);
  std::unique_ptr<parser::DictionaryValue> result(new parser::DictionaryValue);
  if (dv)
    result->Set(reinterpret_cast<const char*>(root_node->name), dv.release());
  return result;
}

std::string GetNodeDir(xmlNode* node, const std::string& inherit_dir) {
  std::string dir(inherit_dir);
  xmlAttr* prop = nullptr;
  for (prop = node->properties; prop; prop = prop->next) {
    if (xmlStrEqual(prop->name, kDirAttributeKey)) {
      char* prop_value = reinterpret_cast<char *>(xmlNodeListGetString(
          node->doc, prop->children, 1));
      dir = prop_value;
      xmlFree(prop_value);
      break;
    }
  }
  return dir;
}
std::string GetNodeText(xmlNode* root, const std::string& inherit_dir) {
    if (root->type != XML_ELEMENT_NODE)
    return std::string();
  std::string current_dir(GetNodeDir(root, inherit_dir));
  std::string text;
  if (!current_dir.empty())
    text += parser::utils::GetDirUTF8Start(current_dir);
  for (xmlNode* node = root->children; node; node = node->next) {
    if (node->type == XML_TEXT_NODE || node->type == XML_CDATA_SECTION_NODE)
      text = text + std::string(reinterpret_cast<char*>(node->content));
    else
      text += GetNodeText(node, current_dir);
  }
  if (!current_dir.empty())
    text += parser::utils::GetDirUTF8End();
  return text;
}
bool IsPropSupportDir(xmlNode* root, xmlAttr* prop) {
  if (xmlStrEqual(root->name, kNamePrivilegeKey)
      && xmlStrEqual(prop->name, kPrivigeNameAttributeKey)) {
    return true;
  }
  return false;
}
bool IsTrimRequiredForElement(xmlNode* root) {
  if (xmlStrEqual(root->name, kNamePrivilegeKey)) {
    return true;
  }
  return false;}
bool IsTrimRequiredForProp(xmlNode* root, xmlAttr* prop) {
  if (xmlStrEqual(root->name, kNamePrivilegeKey) &&
      xmlStrEqual(prop->name, kPrivigeNameAttributeKey)) {
    return true;
  }
  return false;
}
std::unique_ptr<parser::DictionaryValue> LoadXMLNode(
    xmlNode* root, const std::string& inherit_dir) {
  std::unique_ptr<parser::DictionaryValue> value(new parser::DictionaryValue());
  if (root->type != XML_ELEMENT_NODE)
    return nullptr;

  std::string current_dir(GetNodeDir(root, inherit_dir));

  xmlAttr* prop = nullptr;
  for (prop = root->properties; prop; prop = prop->next) {
    xmlChar* value_ptr = xmlNodeListGetString(root->doc, prop->children, 1);
    std::string prop_value(reinterpret_cast<char*>(value_ptr));
    xmlFree(value_ptr);
    if (IsPropSupportDir(root, prop))
      prop_value = parser::utils::GetDirTextUTF8(prop_value, current_dir);

    if (IsTrimRequiredForProp(root, prop))
      prop_value = parser::utils::CollapseWhitespaceUTF8(prop_value);

    value->SetString(
        std::string(kAttributePrefix)
        + reinterpret_cast<const char*>(prop->name),
        prop_value);
  }

  if (root->ns)
    value->SetString(kNamespaceKey,
        reinterpret_cast<const char*>(root->ns->href));

  for (xmlNode* node = root->children; node; node = node->next) {
    std::string sub_node_name(reinterpret_cast<const char*>(node->name));
    std::unique_ptr<parser::DictionaryValue> sub_value =
        LoadXMLNode(node, current_dir);

    if (!sub_value) {
      continue;
    }
    if (!value->HasKey(sub_node_name)) {
      value->Set(sub_node_name, sub_value.release());
      continue;
    }

    parser::Value* temp;
    value->Get(sub_node_name, &temp);

    if (temp->IsType(parser::Value::TYPE_LIST)) {
      parser::ListValue* list;
      temp->GetAsList(&list);
      list->Append(sub_value.release());
    } else {
      assert(temp->IsType(parser::Value::TYPE_DICTIONARY));
      parser::DictionaryValue* dict;
      temp->GetAsDictionary(&dict);
      parser::DictionaryValue* prev_value = dict->DeepCopy();

      parser::ListValue* list = new parser::ListValue();
      list->Append(prev_value);
      list->Append(sub_value.release());
      value->Set(sub_node_name, list);
    }
  }

  std::string text;
  xmlChar* text_ptr = xmlNodeListGetString(root->doc, root->children, 1);
  if (text_ptr) {
    text = reinterpret_cast<char*>(text_ptr);
    xmlFree(text_ptr);
  }
  if (IsTrimRequiredForElement(root))
    text = parser::utils::CollapseWhitespaceUTF8(text);
  if (!text.empty())
    value->SetString(kXmlTextKey, text);

  return value;
}

std::vector<std::string> GetExtensionPrivilegeList(std::string configXml) {
  std::unique_ptr<parser::DictionaryValue> dic = LoadExtensionConfig(configXml);
  std::vector<std::string> privilege_list;
  parser::Value* value = nullptr;
  if (dic->Get(kExtensionPath, &value)) {
    // if one privilege : TYPE_DICTIONARY  , over one : TYPE_LIST
    if (value->GetType() == parser::Value::TYPE_LIST) {
      parser::ListValue* m_list;
      if (value->GetAsList(&m_list)) {
        for (unsigned int i = 0 ; i < m_list->GetSize() ; i++) {
          parser::DictionaryValue* dicVal;
          if (m_list->GetDictionary(i, &dicVal)) {
            std::string privilege;
            if (dicVal->GetString(kExtensionNameKey, &privilege)) {
              LOG(DEBUG) << "find privilege : " << privilege;
              privilege_list.push_back(privilege);
            }
          } else {
            LOG(ERROR) << "cannot get dic";
          }
        }
      }
    } else if (value->GetType() == parser::Value::TYPE_DICTIONARY) {
      parser::DictionaryValue* dicVal;
      if (value->GetAsDictionary(&dicVal)) {
        std::string privilege;
        if (dicVal->GetString(kExtensionNameKey, &privilege)) {
          LOG(DEBUG) << "find privilege : " << privilege;
          privilege_list.push_back(privilege);
        }
      }
    } else {
      LOG(ERROR) << "Unknown value type";
    }
  }
  return privilege_list;
}
}  // namespace wgt
