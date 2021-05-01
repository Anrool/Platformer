#define LOG_TAG "MusicPlayer"

#include "musicPlayer.h"
#include "core.h"
#include "utils.h"

#include <SFML/Audio/Music.hpp>

#include <set>

std::unique_ptr<MusicPlayer> MusicPlayer::mInstance;

const std::map<MusicPlayer::MUSIC_TYPE, std::string>
    MusicPlayer::mMusicTypeToFileMap = {
        {MUSIC_TYPE::MENU, "Menu_Theme.ogg"},
        {MUSIC_TYPE::GAME, "Game_Theme.ogg"},
        {MUSIC_TYPE::INTERMEDIATE, "Intermediate_Theme.ogg"},
        {MUSIC_TYPE::FAILURE, "Failure_Theme.ogg"},
        {MUSIC_TYPE::SUCCESS, "Success_Theme.ogg"},
};

const float MusicPlayer::mMinVolume = 1.f;
const float MusicPlayer::mMaxVolume = 100.f;

void MusicPlayer::createInstance() { getInstance(); }

void MusicPlayer::play(MUSIC_TYPE type) {
  const auto it = mMusicTypeToFileMap.find(type);

  CHECK(it != mMusicTypeToFileMap.cend());

  const auto &music = getInstance().mMusic;

  if (music->getStatus() != sf::Music::Stopped) {
    music->stop();
  }

  CHECK(music->openFromFile(MUSIC_DIR + it->second));

  music->play();
}

void MusicPlayer::pause() {
  const auto &music = getInstance().mMusic;

  CHECK(music->getStatus() == sf::Music::Playing);

  music->pause();
}

void MusicPlayer::resume() {
  const auto &music = getInstance().mMusic;

  CHECK(music->getStatus() == sf::Music::Paused);

  music->play();
}

void MusicPlayer::stop() {
  const auto &music = getInstance().mMusic;

  CHECK(music->getStatus() != sf::Music::Stopped);

  music->stop();
}

bool MusicPlayer::isPlaying() {
  return getInstance().mMusic->getStatus() == sf::Music::Playing;
}

void MusicPlayer::setVolume(float volume) {
  CHECK(volume >= mMinVolume && volume <= mMaxVolume);

  auto &instance = getInstance();
  instance.mVolume = volume;
  instance.mMusic->setVolume(volume);
}

MusicPlayer &MusicPlayer::getInstance() {
  if (mInstance == nullptr) {
    mInstance.reset(new (std::nothrow) MusicPlayer{});

    NOT_NULL(mInstance);
  }

  return *mInstance;
}

MusicPlayer::MusicPlayer()
    : mMusic(makeUnique<sf::Music>()), mVolume(DEFAULT_SOUND_VOLUME) {
  CHECK(mMusicTypeToFileMap.size() == static_cast<size_t>(MUSIC_TYPE::COUNT));

  const auto fileList = listDirectory(MUSIC_DIR);

  CHECK(fileList.size() == mMusicTypeToFileMap.size());

  const std::set<std::string> fileSet{fileList.cbegin(), fileList.cend()};

  for (const auto &musicPair : mMusicTypeToFileMap) {
    CHECK(fileSet.find(musicPair.second) != fileSet.cend());
  }

  mMusic->setLoop(true);
  mMusic->setVolume(mVolume);
}
