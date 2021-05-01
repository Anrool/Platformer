#ifndef UTILITY_H
#define UTILITY_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <list>
#include <memory>
#include <string>

namespace sf {
class Keyboard;
}

const float PIXELS_IN_METER = 100.f;

const size_t TRIANGLE_VERTEX_NUM = 3;
const size_t RECTANGLE_VERTEX_NUM = 4;

const sf::Color STATE_HEADER_COLOR = {60u, 179u, 113u};
const unsigned int STATE_CHAR_SIZE = 50u;
const std::string STATE_TEXTURE_NAME = "Space.png";

class UserData;
class b2Fixture;
class b2Vec2;
enum class OBJECT_TYPE;
enum class ANIMATION_TYPE;
enum class STATE_TYPE;
enum class KEY_TYPE;

template <typename Type, typename... Args>
std::unique_ptr<Type> makeUnique(Args &&...args) {
  std::unique_ptr<Type> instance{new (std::nothrow)
                                     Type{std::forward<Args>(args)...}};

  NOT_NULL(instance);

  return instance;
}

template <typename Type, typename... Args>
std::shared_ptr<Type> makeShared(Args &&...args) {
  std::shared_ptr<Type> instance{new (std::nothrow)
                                     Type{std::forward<Args>(args)...}};

  NOT_NULL(instance);

  return instance;
}

UserData *getFixtureUserData(const b2Fixture *const fixture);

unsigned int strToUintSave(const std::string &str, size_t *const pos = nullptr,
                           int base = 10);

bool validateFile(const char *const pathToFile);

template <typename Type> inline void centerOrigin(Type &instance) {
  const auto bounds = instance.getLocalBounds();
  instance.setOrigin(bounds.left + bounds.width / 2.f,
                     bounds.top + bounds.height / 2.f);
}

float toMeters(float pixels);

float toPixels(float meters);

template <typename Type> inline Type toMeters(const Type &pixels) {
  return {toMeters(pixels.x), toMeters(pixels.y)};
}

template <typename Type> inline Type toPixels(const Type &meters) {
  return {toPixels(meters.x), toPixels(meters.y)};
}

template <typename Type> inline Type toB2Coords(const Type &pixels) {
  return toMeters(Type{pixels.x, -pixels.y});
}

template <typename Type> inline Type toSFMLCoords(const Type &meters) {
  return toPixels(Type{meters.x, -meters.y});
}

template <typename Type, size_t size>
inline size_t arraySize(const Type (&/*array*/)[size]) {
  return size;
}

float toDegree(float radian);

float toRadian(float degree);

void createTriangle(b2Vec2 (&vertices)[TRIANGLE_VERTEX_NUM], bool left,
                    const b2Vec2 &size);

void createRectangle(b2Vec2 (&vertices)[RECTANGLE_VERTEX_NUM],
                     const b2Vec2 &size);

std::list<std::string> listDirectory(const std::string &dirName);

const char *toString(OBJECT_TYPE objectType);
const char *toString(ANIMATION_TYPE animationType);
const char *toString(STATE_TYPE stateType);
const char *toString(KEY_TYPE keyType);
const char *toString(sf::Keyboard::Key key);

#endif // UTILITY_H
