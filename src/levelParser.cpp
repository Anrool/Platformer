#define LOG_TAG "LevelParser"

#include "levelParser.h"
#include "core.h"
#include "objectType.h"
#include "utils.h"

#include "tinyxml.h"

#include <algorithm>

const std::set<OBJECT_TYPE> LevelParser::mRequiredObjectTypes = {
    OBJECT_TYPE::PLAYER,
    OBJECT_TYPE::SOLID,
    OBJECT_TYPE::FINISH,
};
const LevelParser::NameTypePair LevelParser::mNameTypeMap[] = {
    {"player", OBJECT_TYPE::PLAYER},
    {"enemy", OBJECT_TYPE::RUNNER},
    {"archer", OBJECT_TYPE::ARCHER},
    {"solid", OBJECT_TYPE::SOLID},
    {"VerticalPlatform", OBJECT_TYPE::VERTICAL_PLATFORM},
    {"HorizontalPlatform", OBJECT_TYPE::HORIZONTAL_PLATFORM},
    {"SlopeLeft", OBJECT_TYPE::SLOPE_LEFT},
    {"SlopeRight", OBJECT_TYPE::SLOPE_RIGHT},
    {"ladder", OBJECT_TYPE::LADDER},
    {"solidOnLadder", OBJECT_TYPE::SOLID_ON_LADDER},
    {"hazard", OBJECT_TYPE::HAZARD},
    {"finish", OBJECT_TYPE::FINISH},
};
const size_t LevelParser::mNameTypeMapSize = arraySize(mNameTypeMap);

LevelParser::LevelParser(const std::string &filename) : mInfo(), mObjectMap() {
  CHECK(!filename.empty());

  parseFile(filename);
}

const TileMapInfo &LevelParser::getTileMapInfo() const { return mInfo; }

std::vector<OBJECT_TYPE> LevelParser::getObjectTypes() const {
  std::vector<OBJECT_TYPE> types;
  types.reserve(mObjectMap.size());

  for (const auto &typeObject : mObjectMap) {
    types.emplace_back(typeObject.first);
  }

  return types;
}

bool LevelParser::hasType(OBJECT_TYPE type) const {
  CHECK(type != OBJECT_TYPE::NONE);

  return mObjectMap.find(type) != mObjectMap.cend();
}

const LevelParser::ObjectArray &
LevelParser::getObjectsFor(OBJECT_TYPE type) const {
  CHECK(type != OBJECT_TYPE::NONE);

  const auto it = mObjectMap.find(type);

  CHECK(it != mObjectMap.cend());

  return it->second;
}

bool LevelParser::isRequiredType(OBJECT_TYPE type) const {
  CHECK(type != OBJECT_TYPE::NONE);

  return mRequiredObjectTypes.find(type) != mRequiredObjectTypes.cend();
}

void LevelParser::parseFile(const std::string &filename) {
  TiXmlDocument doc;

  CHECK(doc.LoadFile(LEVELS_DIR + filename));

  TiXmlElement *const mapElem = doc.FirstChildElement("map");

  NOT_NULL(mapElem);

  sf::Vector2u mapRectNum;
  sf::Vector2u tileSize;

  CHECK(mapElem->QueryValueAttribute("width", &mapRectNum.x) == TIXML_SUCCESS);
  CHECK(mapElem->QueryValueAttribute("height", &mapRectNum.y) == TIXML_SUCCESS);
  CHECK(mapElem->QueryValueAttribute("tilewidth", &tileSize.x) ==
        TIXML_SUCCESS);
  CHECK(mapElem->QueryValueAttribute("tileheight", &tileSize.y) ==
        TIXML_SUCCESS);
  CHECK(mapRectNum.x > 0 && mapRectNum.y > 0);
  CHECK(tileSize.x > 0 && tileSize.x == tileSize.y);

  TiXmlElement *const tilesetElem = mapElem->FirstChildElement("tileset");

  NOT_NULL(tilesetElem);

  unsigned int firstgid;

  CHECK(tilesetElem->QueryValueAttribute("firstgid", &firstgid) ==
        TIXML_SUCCESS);

  const TiXmlElement *const imageElem = tilesetElem->FirstChildElement("image");

  NOT_NULL(imageElem);

  const char *const pathToTexture = imageElem->Attribute("source");

  NOT_NULL(pathToTexture);
  CHECK(validateFile(pathToTexture));

  const std::string copyPathToTexture = pathToTexture;
  const auto slashIndex = copyPathToTexture.find_last_of('/');

  CHECK(slashIndex != std::string::npos);

  const auto tilesetTextureName = copyPathToTexture.substr(slashIndex + 1);
  sf::Vector2u textureSize;

  CHECK(imageElem->QueryValueAttribute("width", &textureSize.x) ==
        TIXML_SUCCESS);
  CHECK(imageElem->QueryValueAttribute("height", &textureSize.y) ==
        TIXML_SUCCESS);
  CHECK(textureSize.x > 0 && textureSize.y > 0);

  TiXmlElement *const layerElem = mapElem->FirstChildElement("layer");

  NOT_NULL(layerElem);

  const TiXmlElement *const dataElem = layerElem->FirstChildElement("data");

  NOT_NULL(dataElem);

  const char *const encoding = dataElem->Attribute("encoding");

  NOT_NULL(encoding);
  CHECK(std::string(encoding) == "csv");

  const char *const gidStr = dataElem->GetText();

  NOT_NULL(gidStr);

  const std::vector<unsigned int> gids = parseGids(gidStr);

  CHECK(static_cast<size_t>(mapRectNum.x * mapRectNum.y) == gids.size());

  mInfo.set(tilesetTextureName, mapRectNum, gids, firstgid, tileSize.x);
  TiXmlElement *const objectgroupElem =
      mapElem->FirstChildElement("objectgroup");

  NOT_NULL(objectgroupElem);

  for (const TiXmlElement *objectElem =
           objectgroupElem->FirstChildElement("object");
       objectElem != nullptr; objectElem = objectElem->NextSiblingElement()) {
    const char *const name = objectElem->Attribute("name");

    NOT_NULL(name);

    const auto type = nameToType(name);

    CHECK(type != OBJECT_TYPE::NONE);

    sf::FloatRect object;

    CHECK(objectElem->QueryValueAttribute("x", &object.left) == TIXML_SUCCESS);
    CHECK(objectElem->QueryValueAttribute("y", &object.top) == TIXML_SUCCESS);
    CHECK(objectElem->QueryValueAttribute("width", &object.width) ==
          TIXML_SUCCESS);
    CHECK(objectElem->QueryValueAttribute("height", &object.height) ==
          TIXML_SUCCESS);
    CHECK(object.width > 0 && object.height > 0);

    auto it = mObjectMap.find(type);

    if (it == mObjectMap.cend()) {
      const auto insertResult = mObjectMap.emplace(type, ObjectArray{});

      CHECK(insertResult.second);

      it = insertResult.first;
    }

    it->second.emplace_back(object);
  }

  for (auto type : mRequiredObjectTypes) {
    CHECK(hasType(type));
  }

  CHECK(getObjectsFor(OBJECT_TYPE::PLAYER).size() == 1);
}

std::vector<unsigned int>
LevelParser::parseGids(const char *const gidStr) const {
  NOT_NULL(gidStr);

  std::string copy = gidStr;
  std::vector<unsigned int> gids;
  const char del = ',';
  unsigned int gid;
  size_t pos = 0;

  while (copy.find_first_of(del) != std::string::npos) {
    gid = strToUintSave(copy, &pos);
    gids.push_back(gid);
    copy.erase(0, pos + 1);
  }

  gid = strToUintSave(copy);
  gids.push_back(gid);

  return gids;
}

OBJECT_TYPE LevelParser::nameToType(const std::string &name) const {
  CHECK(!name.empty());

  const auto it =
      std::find_if(mNameTypeMap, mNameTypeMap + mNameTypeMapSize,
                   [name](const NameTypePair &p) { return p.name == name; });

  CHECK(it != mNameTypeMap + mNameTypeMapSize);

  return it->type;
}

void TileMapInfo::set(const std::string &tilesetTextureName,
                      const sf::Vector2u &mapRectNum,
                      const std::vector<unsigned int> &gids,
                      unsigned int firstgid, unsigned int tileSize) {
  mTilesetTextureName = tilesetTextureName;
  mMapRectNum = mapRectNum;
  mGids = gids;
  mFirstgid = firstgid;
  mTileSize = tileSize;
}
