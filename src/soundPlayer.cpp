#define LOG_TAG "SoundPlayer"

#include "soundPlayer.h"
#include "core.h"
#include "resourceManager.h"

#include <SFML/Audio/Listener.hpp>
#include <SFML/Audio/Sound.hpp>

#include <cmath>

std::unique_ptr<SoundPlayer> SoundPlayer::mInstance;

const float SoundPlayer::mAttenuation = 50.f;
const float SoundPlayer::mListenerPositionZ = 200.f;
const float SoundPlayer::mMinDistance2D = 50.f;
const float SoundPlayer::mMinDistance3D =
    std::hypot(mMinDistance2D, mListenerPositionZ);

void SoundPlayer::createInstance() { getInstance(); }

void SoundPlayer::play(const std::string &filename) {
  play(filename, getListenerPosition());
}

void SoundPlayer::play(const std::string &filename,
                       const sf::Vector2f &position) {
  CHECK(!filename.empty());

  const auto &soundBuffer = ResourceManager::getSoundBuffer(filename);

  auto &sounds = getInstance().mSounds;
  sounds.emplace_back(soundBuffer);
  auto &newSound = sounds.back();

  newSound.setMinDistance(mMinDistance3D);
  newSound.setAttenuation(mAttenuation);
  newSound.setPosition(position.x, -position.y, 0.f);
  newSound.setVolume(DEFAULT_SOUND_VOLUME);
  newSound.play();
}

void SoundPlayer::removeStopped() {
  getInstance().mSounds.remove_if(
      [](sf::Sound &s) { return s.getStatus() == sf::Sound::Stopped; });
}

void SoundPlayer::setListenerPosition(const sf::Vector2f &position) {
  sf::Listener::setPosition(position.x, -position.y, mListenerPositionZ);
}

const sf::Vector2f SoundPlayer::getListenerPosition() {
  const auto position = sf::Listener::getPosition();
  return {position.x, -position.y};
}

SoundPlayer &SoundPlayer::getInstance() {
  if (mInstance == nullptr) {
    mInstance.reset(new (std::nothrow) SoundPlayer{});

    NOT_NULL(mInstance);
  }

  return *mInstance;
}
