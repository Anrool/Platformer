#ifndef TILEMAP_H
#define TILEMAP_H

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/NonCopyable.hpp>

#include <memory>

namespace sf {
class RenderTarget;
}

class TileMapInfo;

class TileMap : public sf::Drawable, private sf::NonCopyable {
public:
  explicit TileMap(const TileMapInfo &info);

  const sf::Vector2u &getMapSize() const;
  unsigned int getTileSize() const;

private:
  const sf::Texture &mTexture;
  sf::VertexArray mVertices;

  sf::Vector2u mMapSize;
  unsigned int mTileSize;

  void init(const TileMapInfo &info);

  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;
};

#endif // TILEMAP_H
