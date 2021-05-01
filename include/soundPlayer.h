#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Vector2.hpp>

#include <list>
#include <memory>

namespace sf {
class Sound;
}

class SoundPlayer : private sf::NonCopyable {
public:
  static void createInstance();

  static void play(const std::string &filename);
  static void play(const std::string &filename, const sf::Vector2f &position);

  static void removeStopped();

  static void setListenerPosition(const sf::Vector2f &position);
  static const sf::Vector2f getListenerPosition();

private:
  static std::unique_ptr<SoundPlayer> mInstance;

  static const float mListenerPositionZ;
  static const float mAttenuation;
  static const float mMinDistance2D;
  static const float mMinDistance3D;

  std::list<sf::Sound> mSounds;

  static SoundPlayer &getInstance();
};

#endif // SOUNDPLAYER_H
