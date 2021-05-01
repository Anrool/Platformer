#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <map>
#include <memory>

enum class KEY_TYPE {
  RIGHT,
  LEFT,
  UP,
  DOWN,
  SHOOT,
  NONE,
  COUNT = NONE,
};

class InputManager : private sf::NonCopyable {
public:
  using LayoutMap = std::map<const KEY_TYPE, sf::Keyboard::Key>;

  static void createInstance();

  static void setKey(KEY_TYPE type, sf::Keyboard::Key key);
  static sf::Keyboard::Key getKey(KEY_TYPE type);

  static bool isKeyPressed(KEY_TYPE type);

  static const LayoutMap &getLayout(bool isDefault);
  static void saveLayout();

private:
  static std::unique_ptr<InputManager> mInstance;

  static const LayoutMap mDefaultLayoutMap;
  static const std::map<std::string, KEY_TYPE> mKeyTitleTypeMap;

  static const std::string mLayoutFile;
  static const std::string mXMLVersion;
  static const std::string mXMLEncoding;
  static const std::string mXMLStandalone;
  static const std::string mBindingElemName;
  static const std::string mTitleAttributeName;
  static const std::string mKeyAttributeName;

  LayoutMap mLayoutMap;

  static InputManager &getInstance();

  InputManager();

  void parseLayout();

  static KEY_TYPE keyTitleToType(const std::string &title);
};

#endif // INPUTMANAGER_H
