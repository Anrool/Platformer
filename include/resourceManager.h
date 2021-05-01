#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <SFML/System/NonCopyable.hpp>

#include <map>
#include <memory>

namespace sf {
class RenderWindow;
class Texture;
class Font;
class SoundBuffer;
} // namespace sf

class LevelParser;

class ResourceManager : private sf::NonCopyable {
public:
  static void createInstance();

  static sf::RenderWindow &getWindow();
  static const sf::Texture &getTexture(const std::string &filename);
  static const sf::Font &getFont(const std::string &filename);
  static const sf::SoundBuffer &getSoundBuffer(const std::string filename);

  static size_t getLevelCount();
  static const LevelParser &getLevelParser(size_t level);
  static const sf::Texture &getLevelTexture(size_t level);

private:
  template <typename ResourceType> class ResourceHolder;

  using TextureHolder = ResourceHolder<sf::Texture>;
  using FontHolder = ResourceHolder<sf::Font>;
  using SoundHolder = ResourceHolder<sf::SoundBuffer>;

  static std::unique_ptr<const ResourceManager> mInstance;

  static const std::string mLevelTextures[];

  std::unique_ptr<sf::RenderWindow> mWindow;

  std::unique_ptr<const TextureHolder> mTextureHolder;
  std::unique_ptr<const FontHolder> mFontHolder;
  std::unique_ptr<const SoundHolder> mSoundHolder;

  std::map<const size_t, std::unique_ptr<const LevelParser>> mLevelParserMap;

  static const ResourceManager &getInstance();

  ResourceManager();
};

#endif // RESOURCEMANAGER_H
