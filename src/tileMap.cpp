#define LOG_TAG "TileMap"

#include "tileMap.h"
#include "core.h"
#include "levelParser.h"
#include "resourceManager.h"
#include "utils.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>

TileMap::TileMap(const TileMapInfo &info)
    : mTexture(ResourceManager::getTexture(info.mTilesetTextureName)),
      mVertices(sf::Quads), mMapSize(), mTileSize(0u) {
  init(info);
}

const sf::Vector2u &TileMap::getMapSize() const { return mMapSize; }

unsigned int TileMap::getTileSize() const { return mTileSize; }

void TileMap::init(const TileMapInfo &info) {
  mVertices.resize(info.mMapRectNum.x * info.mMapRectNum.y *
                   RECTANGLE_VERTEX_NUM);

  const sf::Vector2u texRectNum = mTexture.getSize() / info.mTileSize;
  const unsigned int texRectLimit = texRectNum.x * texRectNum.y;
  const auto tileSize = static_cast<float>(info.mTileSize);

  for (unsigned int y = 0; y < info.mMapRectNum.y; y++) {
    for (unsigned int x = 0; x < info.mMapRectNum.x; x++) {
      const auto gidIndex = y * info.mMapRectNum.x + x;
      const unsigned int gid = info.mGids.at(gidIndex);

      // skip if there is no texture tile here
      if (gid < info.mFirstgid) {
        continue;
      }

      const unsigned int textureIndex = gid - info.mFirstgid;

      CHECK(textureIndex < texRectLimit);

      const sf::Vector2f texOrigin{
          static_cast<float>(textureIndex % texRectNum.x),
          static_cast<float>(textureIndex / texRectNum.x)};
      sf::Vertex *const quad = &mVertices[gidIndex * RECTANGLE_VERTEX_NUM];

      quad[0].position = sf::Vector2f(x, y) * tileSize;
      quad[1].position = sf::Vector2f(x + 1, y) * tileSize;
      quad[2].position = sf::Vector2f(x + 1, y + 1) * tileSize;
      quad[3].position = sf::Vector2f(x, y + 1) * tileSize;

      quad[0].texCoords = texOrigin * tileSize;
      quad[1].texCoords = sf::Vector2f(texOrigin.x + 1, texOrigin.y) * tileSize;
      quad[2].texCoords =
          sf::Vector2f(texOrigin.x + 1, texOrigin.y + 1) * tileSize;
      quad[3].texCoords = sf::Vector2f(texOrigin.x, texOrigin.y + 1) * tileSize;
    }
  }

  mMapSize = info.mMapRectNum * info.mTileSize;
  mTileSize = info.mTileSize;
}

void TileMap::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  states.texture = &mTexture;
  target.draw(mVertices, states);
}
