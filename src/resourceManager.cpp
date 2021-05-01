#define LOG_TAG "ResourceManager"

#include "resourceManager.h"
#include "core.h"
#include "levelParser.h"
#include "musicPlayer.h"
#include "soundPlayer.h"
#include "utils.h"

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>

template <typename RESOURCE_TYPE>
class ResourceManager::ResourceHolder : private sf::NonCopyable {
public:
  explicit ResourceHolder(const std::string &resourceDir);

  const RESOURCE_TYPE &get(const std::string &filename) const;

private:
  std::string mResourceDir;
  std::map<const std::string, const std::unique_ptr<const RESOURCE_TYPE>>
      mResourceMap;

  void load(const std::string &filename);
};

template <typename RESOURCE_TYPE>
ResourceManager::ResourceHolder<RESOURCE_TYPE>::ResourceHolder(
    const std::string &resourceDir)
    : mResourceDir(), mResourceMap() {
  CHECK(!resourceDir.empty());

  mResourceDir = resourceDir;

  for (const auto &filename : listDirectory(resourceDir)) {
    load(filename);
  }
}

template <typename ResourceType>
const ResourceType &ResourceManager::ResourceHolder<ResourceType>::get(
    const std::string &filename) const {
  CHECK(!filename.empty());

  const auto it = mResourceMap.find(filename);

  CHECK(it != mResourceMap.cend());

  return *it->second;
}

template <typename RESOURCE_TYPE>
void ResourceManager::ResourceHolder<RESOURCE_TYPE>::load(
    const std::string &filename) {
  CHECK(!filename.empty());

  auto resource = makeUnique<RESOURCE_TYPE>();

  CHECK(resource->loadFromFile(mResourceDir + filename));
  CHECK(mResourceMap.emplace(filename, std::move(resource)).second);
}

std::unique_ptr<const ResourceManager> ResourceManager::mInstance;

const std::string ResourceManager::mLevelTextures[] = {
    "Snow.png",
    "Rock.png",
    "Clouds.png",
};

void ResourceManager::createInstance() {
  /*const auto& instance = */ getInstance();
}

sf::RenderWindow &ResourceManager::getWindow() {
  return *getInstance().mWindow;
}

const sf::Texture &ResourceManager::getTexture(const std::string &filename) {
  return getInstance().mTextureHolder->get(filename);
}

const sf::Font &ResourceManager::getFont(const std::string &filename) {
  return getInstance().mFontHolder->get(filename);
}

const sf::SoundBuffer &
ResourceManager::getSoundBuffer(const std::string filename) {
  return getInstance().mSoundHolder->get(filename);
}

size_t ResourceManager::getLevelCount() { return arraySize(mLevelTextures); }

const LevelParser &ResourceManager::getLevelParser(size_t level) {
  CHECK(level < getLevelCount());

  const auto &levelParserMap = getInstance().mLevelParserMap;
  const auto it = levelParserMap.find(level);

  CHECK(it != levelParserMap.cend());

  return *it->second;
}

const sf::Texture &ResourceManager::getLevelTexture(size_t level) {
  CHECK(level < getLevelCount());

  return getTexture(mLevelTextures[level]);
}

const ResourceManager &ResourceManager::getInstance() {
  if (mInstance == nullptr) {
    mInstance.reset(new (std::nothrow) ResourceManager{});

    NOT_NULL(mInstance);
  }

  return *mInstance;
}

ResourceManager::ResourceManager()
    : mWindow(makeUnique<sf::RenderWindow>(
          sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), APPLICATION_NAME,
          static_cast<sf::Uint32>(sf::Style::Titlebar | sf::Style::Close))),
      mTextureHolder(makeUnique<TextureHolder>(TEXTURES_DIR)),
      mFontHolder(makeUnique<FontHolder>(FONTS_DIR)),
      mSoundHolder(makeUnique<SoundHolder>(SOUNDS_DIR)), mLevelParserMap() {
  const std::string levelPrefix = "Level_";
  const auto levelPrefixSize = levelPrefix.size();
  const std::string levelSuffix = ".tmx";
  const auto levelSuffixSize = levelSuffix.size();

  for (const auto &filename : listDirectory(LEVELS_DIR)) {
    const auto filenameSize = filename.size();

    CHECK(levelPrefixSize + levelSuffixSize < filenameSize);
    CHECK(filename.compare(0, levelPrefixSize, levelPrefix) == 0);
    CHECK(filename.compare(filenameSize - levelSuffixSize, levelSuffixSize,
                           levelSuffix) == 0);

    const auto levelString = filename.substr(
        levelPrefixSize, filenameSize - levelPrefixSize - levelSuffixSize);
    const auto levelNumber = static_cast<size_t>(strToUintSave(levelString));

    CHECK(levelNumber > 0 && levelNumber <= getLevelCount());

    auto levelParser = makeUnique<LevelParser>(filename);

    CHECK(mLevelParserMap.emplace(levelNumber - 1, std::move(levelParser))
              .second);
  }

  CHECK(mLevelParserMap.size() == arraySize(mLevelTextures));

  MusicPlayer::createInstance();
  SoundPlayer::createInstance();
}
