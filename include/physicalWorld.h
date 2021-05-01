#ifndef PHYSICALWORLD_H
#define PHYSICALWORLD_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/NonCopyable.hpp>

#include <list>
#include <map>
#include <memory>

struct PhysicalBody;
enum class OBJECT_TYPE;
class LevelParser;
class b2Body;
class b2Vec2;
struct PlayerCallback;
class b2World;
class UserData;
class b2Fixture;

namespace sf {
class Time;
class Color;
} // namespace sf

class PhysicalWorld : public sf::Drawable, private sf::NonCopyable {
public:
  using PhysicalBodyVector = std::vector<std::unique_ptr<PhysicalBody>>;
  using PhysicalBodyMap = std::map<const OBJECT_TYPE, PhysicalBodyVector>;

  explicit PhysicalWorld(const LevelParser &levelParser,
                         PhysicalBodyMap &bodyMap);
  ~PhysicalWorld() final;

  b2Body *createBody(const sf::FloatRect &object, OBJECT_TYPE type);

  void createFootSensorFor(b2Body *const body,
                           const b2Vec2 &parentSizeInPixels);

  void destroyBody(b2Body *const body);

  void setPlayerCallback(PlayerCallback *const callback);

  void update(sf::Time dt);

  bool finished() const;

private:
  class CustomContactListener;
  class CustomContactFilter;

  static const int mVelocityIterations;
  static const int mPositionIterations;

  static const float mSolidOnLadderHeight;

  std::unique_ptr<CustomContactListener> mContactListener;
  std::unique_ptr<CustomContactFilter> mContactFilter;
  std::unique_ptr<b2World> mWorld;

  // user data for non-entity bodies
  std::list<std::unique_ptr<UserData>> mNonEntityUserDataArray;

  std::map<const b2Fixture *, const b2Vec2> mFixtureSizeMap;

  std::map<const b2Fixture *, b2Fixture *> mLadderSolidMap;

  sf::Vector2f mPlayerSensorOffset;
  PlayerCallback *mPlayerCallback;

  mutable sf::VertexArray mVertices;

  void initEntityBodies(const LevelParser &parser, PhysicalBodyMap &bodyMap);

  void initNonEntityBodies(const LevelParser &parser);

  b2Body *createBody(const sf::FloatRect &bounds, UserData *const userData);

  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;

  void addRectangleToDrawing(const b2Vec2 &pos, const b2Vec2 &size,
                             const sf::Color &color) const;

  void addTriangleToDrawing(const b2Vec2 &pos, const b2Vec2 &size,
                            const sf::Color &color, bool isSlopeLeft) const;

  template <size_t size>
  void createBoundaryAtPosition(const b2Vec2 &pos,
                                const b2Vec2 (&vertices)[size],
                                const sf::Color &color) const {
    sf::Vertex vertexArray[size];

    for (size_t i = 0; i < size; i++) {
      vertexArray[i] = {shiftVertexAtPosition(vertices[i], pos), color};
    }

    mVertices.append(vertexArray[0]);

    for (size_t i = 1; i < size; ++i) {
      mVertices.append(vertexArray[i]);
      mVertices.append(vertexArray[i]);
    }

    mVertices.append(vertexArray[0]);
  }

  sf::Vector2f shiftVertexAtPosition(const b2Vec2 &point,
                                     const b2Vec2 &pos) const;

  const b2Vec2 &findFixtureSize(const b2Fixture *fixture) const;

  b2Fixture *findSolidOnLadderFixture(const b2Fixture *const ladder) const;
};

#endif // PHYSICALWORLD_H
