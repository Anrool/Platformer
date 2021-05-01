#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <SFML/System/NonCopyable.hpp>

#include <map>
#include <memory>

namespace sf {
class Music;
}

class MusicPlayer : private sf::NonCopyable {
public:
  enum class MUSIC_TYPE { MENU, GAME, INTERMEDIATE, FAILURE, SUCCESS, COUNT };

  static void createInstance();

  static void play(MUSIC_TYPE type);
  static void pause();
  static void resume();
  static void stop();

  static bool isPlaying();

  static void setVolume(float volume);

private:
  static std::unique_ptr<MusicPlayer> mInstance;

  static const std::map<MUSIC_TYPE, std::string> mMusicTypeToFileMap;

  static const float mMinVolume;
  static const float mMaxVolume;

  std::unique_ptr<sf::Music> mMusic;
  float mVolume;

  static MusicPlayer &getInstance();

  MusicPlayer();
};

#endif // MUSICPLAYER_H
