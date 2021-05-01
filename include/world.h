#ifndef WORLD_H
#define WORLD_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/NonCopyable.hpp>

#include <list>
#include <map>
#include <memory>
#include <vector>

namespace sf {
class Event;
class Time;
class Sprite;
} // namespace sf

enum class HEADING;
enum class OBJECT_TYPE;
class PhysicalWorld;
class Player;
class Entity;
class TileMap;
class PhysicalBody;

class World : public sf::Drawable, private sf::NonCopyable {
public:
  explicit World(size_t currentLevel);
  ~World() final;

  void update(sf::Time dt);

  bool failed() const;
  bool success() const;

private:
  static const sf::Vector2f mBulletSize;

  std::unique_ptr<PhysicalWorld> mPhysicalWorld;

  std::unique_ptr<Player> mPlayer;
  std::list<std::unique_ptr<Entity>> mEntities;

  sf::View mView;
  std::unique_ptr<const TileMap> mTileMap;
  std::unique_ptr<sf::Sprite> mBackground;

  void onSpawnBullet(HEADING heading, OBJECT_TYPE type,
                     const sf::Vector2f &position);

  using PhysicalBodyVector = std::vector<std::unique_ptr<PhysicalBody>>;
  using PhysicalBodyMap = std::map<const OBJECT_TYPE, PhysicalBodyVector>;

  void initPhysics(size_t currentLevel);

  void updateView();
  void updateSoundListener();

  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;
};

#endif // WORLD_H
