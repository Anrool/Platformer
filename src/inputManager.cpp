#define LOG_TAG "InputManager"

#include "inputManager.h"
#include "core.h"
#include "utils.h"

#include "tinyxml.h"

#include <algorithm>
#include <set>

std::unique_ptr<InputManager> InputManager::mInstance;

const InputManager::LayoutMap InputManager::mDefaultLayoutMap = {
    {KEY_TYPE::RIGHT, sf::Keyboard::D},     {KEY_TYPE::LEFT, sf::Keyboard::A},
    {KEY_TYPE::UP, sf::Keyboard::W},        {KEY_TYPE::DOWN, sf::Keyboard::S},
    {KEY_TYPE::SHOOT, sf::Keyboard::Space},
};
const std::map<std::string, KEY_TYPE> InputManager::mKeyTitleTypeMap = {
    {"RIGHT", KEY_TYPE::RIGHT}, {"LEFT", KEY_TYPE::LEFT},
    {"UP", KEY_TYPE::UP},       {"DOWN", KEY_TYPE::DOWN},
    {"SHOOT", KEY_TYPE::SHOOT},
};

const std::string InputManager::mLayoutFile =
    INPUT_LAYOUT_DIR + "InputLayout.xml";
const std::string InputManager::mXMLVersion = "1.0";
const std::string InputManager::mXMLEncoding = "UTF-8";
const std::string InputManager::mXMLStandalone = "yes";
const std::string InputManager::mBindingElemName = "binding";
const std::string InputManager::mTitleAttributeName = "title";
const std::string InputManager::mKeyAttributeName = "key";

void InputManager::createInstance() { getInstance(); }

void InputManager::setKey(KEY_TYPE type, sf::Keyboard::Key key) {
  CHECK(type != KEY_TYPE::NONE);
  CHECK(key != sf::Keyboard::Escape);
  CHECK(key != sf::Keyboard::Return);

  auto &currentLayout = getInstance().mLayoutMap;
  auto it = currentLayout.find(type);

  if (it->second == key) {
    return;
  }

  auto &keyPair = *it;

  if (key != sf::Keyboard::Key::Unknown) {
    it = std::find_if(
        currentLayout.begin(), currentLayout.end(),
        [key](const LayoutMap::value_type &p) { return p.second == key; });

    if (it != currentLayout.end()) {
      it->second = sf::Keyboard::Key::Unknown;
    }
  }

  keyPair.second = key;
}

sf::Keyboard::Key InputManager::getKey(KEY_TYPE type) {
  CHECK(type != KEY_TYPE::NONE);

  const auto &currentLayout = getLayout(false);
  const auto it = currentLayout.find(type);

  CHECK(it != currentLayout.cend());

  return it->second;
}

bool InputManager::isKeyPressed(KEY_TYPE type) {
  return sf::Keyboard::isKeyPressed(getKey(type));
}

const InputManager::LayoutMap &InputManager::getLayout(bool isDefault) {
  if (isDefault) {
    return mDefaultLayoutMap;
  }

  return getInstance().mLayoutMap;
}

void InputManager::saveLayout() {
  const auto &currentLayout = getLayout(false);

  TiXmlDocument doc;
  TiXmlDeclaration *const declarationElem = {
      new (std::nothrow)
          TiXmlDeclaration{mXMLVersion, mXMLEncoding, mXMLStandalone}};

  NOT_NULL(declarationElem);
  NOT_NULL(doc.LinkEndChild(declarationElem));

  for (const auto &layoutPair : currentLayout) {
    TiXmlElement *const bindingElem{new (std::nothrow)
                                        TiXmlElement{mBindingElemName}};

    NOT_NULL(bindingElem);

    const auto title = toString(layoutPair.first);
    const auto rawKey = static_cast<int>(layoutPair.second);

    bindingElem->SetAttribute(mTitleAttributeName, title);
    bindingElem->SetAttribute(mKeyAttributeName, rawKey);

    NOT_NULL(doc.LinkEndChild(bindingElem));
  }

  CHECK(doc.SaveFile(mLayoutFile));
}

InputManager &InputManager::getInstance() {
  if (mInstance == nullptr) {
    mInstance.reset(new (std::nothrow) InputManager{});

    NOT_NULL(mInstance);
  }

  return *mInstance;
}

InputManager::InputManager() : mLayoutMap() { parseLayout(); }

void InputManager::parseLayout() {
  TiXmlDocument doc;

  CHECK(doc.LoadFile(mLayoutFile));

  std::set<sf::Keyboard::Key> uniqueKeys;

  for (const TiXmlElement *bindingElem = doc.FirstChildElement();
       bindingElem != nullptr;
       bindingElem = bindingElem->NextSiblingElement()) {
    CHECK(bindingElem->ValueStr() == mBindingElemName);

    const std::string *const title =
        bindingElem->Attribute(mTitleAttributeName);

    NOT_NULL(title);

    const auto type = keyTitleToType(*title);
    int rawKey;

    CHECK(bindingElem->QueryIntAttribute(mKeyAttributeName, &rawKey) ==
          TIXML_SUCCESS);
    CHECK(rawKey >= sf::Keyboard::Unknown);

    const auto key = static_cast<sf::Keyboard::Key>(rawKey);

    CHECK(key == sf::Keyboard::Unknown || uniqueKeys.emplace(key).second);
    CHECK(mLayoutMap.emplace(type, key).second);
  }
}

KEY_TYPE InputManager::keyTitleToType(const std::string &title) {
  CHECK(!title.empty());

  const auto it = mKeyTitleTypeMap.find(title);

  CHECK(it != mKeyTitleTypeMap.cend());

  return it->second;
}
