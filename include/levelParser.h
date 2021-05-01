#ifndef LEVELPARSER_H
#define LEVELPARSER_H

#include <SFML/System/NonCopyable.hpp>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include <map>
#include <set>
#include <vector>

enum class OBJECT_TYPE;

struct TileMapInfo {
  std::string mTilesetTextureName;
  sf::Vector2u mMapRectNum;
  std::vector<unsigned int> mGids;
  unsigned int mFirstgid;
  unsigned int mTileSize;

  void set(const std::string &pathToTexture, const sf::Vector2u &mapRectNum,
           const std::vector<unsigned int> &gids, unsigned int firstgid,
           unsigned int tileSize);
};

class LevelParser : private sf::NonCopyable {
public:
  using ObjectArray = std::vector<sf::FloatRect>;

  explicit LevelParser(const std::string &filename);

  const TileMapInfo &getTileMapInfo() const;

  std::vector<OBJECT_TYPE> getObjectTypes() const;
  bool hasType(OBJECT_TYPE type) const;

  const ObjectArray &getObjectsFor(OBJECT_TYPE type) const;

  bool isRequiredType(OBJECT_TYPE type) const;

private:
  struct NameTypePair {
    std::string name;
    OBJECT_TYPE type;
  };

  static const std::set<OBJECT_TYPE> mRequiredObjectTypes;
  static const NameTypePair mNameTypeMap[];
  static const size_t mNameTypeMapSize;

  TileMapInfo mInfo;
  std::map<OBJECT_TYPE, ObjectArray> mObjectMap;

  void parseFile(const std::string &filename);

  std::vector<unsigned int> parseGids(const char *const gidStr) const;

  OBJECT_TYPE nameToType(const std::string &name) const;
};

#endif // LEVELPARSER_H
