#define LOG_TAG "PhysicalWord"

#include "physicalWorld.h"
#include "core.h"
#include "entity.h"
#include "levelParser.h"
#include "objectType.h"
#include "physicalBody.h"
#include "player.h"
#include "soundPlayer.h"
#include "utils.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <Box2D/Box2D.h>

namespace {
b2Vec2 getPosition(OBJECT_TYPE type, const sf::FloatRect &frame) {
  switch (type) {
  case OBJECT_TYPE::SLOPE_LEFT:
  case OBJECT_TYPE::ALLIED_BULLET:
  case OBJECT_TYPE::ENEMY_BULLET:
    return {frame.left, frame.top};
  case OBJECT_TYPE::SLOPE_RIGHT:
    return {frame.left + frame.width, frame.top};
  default:
    return {frame.left + frame.width / 2.f, frame.top + frame.height / 2.f};
  }
}

sf::Color getColor(b2BodyType type) {
  switch (type) {
  case b2_dynamicBody:
    return sf::Color::Blue;
  case b2_kinematicBody:
    return sf::Color::Cyan;
  default:
    return sf::Color::Green;
  }
}

b2BodyType getBodyType(OBJECT_TYPE type) {
  switch (type) {
  case OBJECT_TYPE::PLAYER:
  case OBJECT_TYPE::RUNNER:
  case OBJECT_TYPE::ARCHER:
  case OBJECT_TYPE::ALLIED_BULLET:
  case OBJECT_TYPE::ENEMY_BULLET:
  case OBJECT_TYPE::EXPLODED_BULLET:
  case OBJECT_TYPE::PLAYER_SENSOR:
    return b2_dynamicBody;
  case OBJECT_TYPE::HORIZONTAL_PLATFORM:
  case OBJECT_TYPE::VERTICAL_PLATFORM:
    return b2_kinematicBody;
  case OBJECT_TYPE::SOLID:
  case OBJECT_TYPE::SLOPE_LEFT:
  case OBJECT_TYPE::SLOPE_RIGHT:
  case OBJECT_TYPE::HAZARD:
  case OBJECT_TYPE::FINISH:
  case OBJECT_TYPE::SOLID_ON_LADDER:
  case OBJECT_TYPE::LADDER:
    return b2_staticBody;
  default:
    CHECK(false);
  }
}

bool isEnemy(OBJECT_TYPE type) {
  return type == OBJECT_TYPE::RUNNER || type == OBJECT_TYPE::ARCHER;
}

bool isPlatform(OBJECT_TYPE type) {
  return type == OBJECT_TYPE::HORIZONTAL_PLATFORM ||
         type == OBJECT_TYPE::VERTICAL_PLATFORM;
}

bool isSlope(OBJECT_TYPE type) {
  return type == OBJECT_TYPE::SLOPE_LEFT || type == OBJECT_TYPE::SLOPE_RIGHT;
}

bool isGround(OBJECT_TYPE type) {
  return type == OBJECT_TYPE::SOLID || isSlope(type) || isPlatform(type) ||
         type == OBJECT_TYPE::SOLID_ON_LADDER;
}

bool findFixtureType(b2Contact *const contact, OBJECT_TYPE wantedType,
                     const b2Fixture **const wanted,
                     const b2Fixture **const other) {
  NOT_NULL(contact);
  CHECK(wantedType != OBJECT_TYPE::NONE);
  NOT_NULL(wanted);
  NOT_NULL(other);

  const auto fixtureA = contact->GetFixtureA();
  const auto fixtureB = contact->GetFixtureB();

  const auto typeA = getFixtureUserData(fixtureA)->getType();
  const auto typeB = getFixtureUserData(fixtureB)->getType();

  if (typeA == wantedType) {
    *wanted = fixtureA;
    *other = fixtureB;
    return true;
  } else if (typeB == wantedType) {
    *wanted = fixtureB;
    *other = fixtureA;
    return true;
  }

  return false;
}

bool isSensor(OBJECT_TYPE type) {
  return type == OBJECT_TYPE::HAZARD || type == OBJECT_TYPE::FINISH ||
         type == OBJECT_TYPE::LADDER || type == OBJECT_TYPE::PLAYER_SENSOR;
}

bool findColliderType(OBJECT_TYPE typeA, OBJECT_TYPE typeB,
                      OBJECT_TYPE targetType, OBJECT_TYPE &resultType) {
  if (typeA == targetType) {
    resultType = typeB;
    return true;
  } else if (typeB == targetType) {
    resultType = typeA;
    return true;
  }

  return false;
}

bool shouldCollideWithPlayer(OBJECT_TYPE type) {
  CHECK(type != OBJECT_TYPE::NONE);
  // should not collide with itself
  CHECK(type != OBJECT_TYPE::PLAYER);

  switch (type) {
  case OBJECT_TYPE::ARCHER:
  case OBJECT_TYPE::RUNNER:
  case OBJECT_TYPE::ENEMY_BULLET:
  case OBJECT_TYPE::EXPLODED_BULLET:
  case OBJECT_TYPE::LADDER:
  case OBJECT_TYPE::FINISH:
  case OBJECT_TYPE::HAZARD:
    return true;
  default:
    return false;
  }
}
} // unnamed namespace

const int32 PhysicalWorld::mVelocityIterations = 8;
const int32 PhysicalWorld::mPositionIterations = 3;

const float PhysicalWorld::mSolidOnLadderHeight = 5.f;

class PhysicalWorld::CustomContactFilter : public b2ContactFilter,
                                           private sf::NonCopyable {
public:
  bool ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB) final {
    const auto typeA = getFixtureUserData(fixtureA)->getType();
    const auto typeB = getFixtureUserData(fixtureB)->getType();

    CHECK(typeA != OBJECT_TYPE::NONE);
    CHECK(typeB != OBJECT_TYPE::NONE);

    if (isGround(typeA) || isGround(typeB)) {
      return true;
    }

    auto otherType = OBJECT_TYPE::NONE;

    if (findColliderType(typeA, typeB, OBJECT_TYPE::PLAYER, otherType)) {
      switch (otherType) {
        // used only in PreSolve
      case OBJECT_TYPE::ARCHER:
      case OBJECT_TYPE::RUNNER:
      case OBJECT_TYPE::ENEMY_BULLET:
      // used only in BeginContact
      case OBJECT_TYPE::FINISH:
      case OBJECT_TYPE::HAZARD:
      // used in BeginContact/EndContact
      case OBJECT_TYPE::LADDER:
        return true;
      default:
        return false;
      }
    } else if (findColliderType(typeA, typeB, OBJECT_TYPE::ALLIED_BULLET,
                                otherType)) {
      return isEnemy(otherType);
    }

    return false;
  }
};

class PhysicalWorld::CustomContactListener : public b2ContactListener,
                                             private sf::NonCopyable {
public:
  explicit CustomContactListener()
      : mPlayerContactNum(0), mLadderListener(), mFinished(false) {}

  bool playerOnGround() const {
    CHECK(mPlayerContactNum >= 0);

    return mPlayerContactNum > 0;
  }

  bool checkLadderCollisionChange(bool &isOnLadder,
                                  const b2Fixture **const ladder) {
    return mLadderListener.checkLadderCollisionChange(isOnLadder, ladder);
  }

  bool finished() const { return mFinished; }

  void BeginContact(b2Contact *contact) final {
    const b2Fixture *wanted = nullptr;
    const b2Fixture *other = nullptr;

    if (findFixtureType(contact, OBJECT_TYPE::PLAYER_SENSOR, &wanted, &other)) {
      const auto otherType = getFixtureUserData(other)->getType();

      CHECK(isGround(otherType));

      mPlayerContactNum++;

      if (isPlatform(otherType)) {
        const b2Fixture *const playerFixture = wanted->GetNext();

        NOT_NULL(playerFixture);

        Player *const player = dynamic_cast<Player *>(
            getFixtureUserData(playerFixture)->getEntity());
        player->setParentBody(other->GetBody());
      }
    } else if (findFixtureType(contact, OBJECT_TYPE::PLAYER, &wanted, &other)) {
      const auto otherType = getFixtureUserData(other)->getType();

      switch (otherType) {
      case OBJECT_TYPE::LADDER: {
        mLadderListener.onLadderCollision(true, other);
        break;
      }
      case OBJECT_TYPE::SOLID_ON_LADDER: {
        mLadderListener.onSolidCollision(true);
        break;
      }
      case OBJECT_TYPE::HAZARD: {
        Entity *const player = getFixtureUserData(wanted)->getEntity();

        if (!player->isKilled()) {
          player->kill();
        }

        break;
      }
      case OBJECT_TYPE::FINISH: {
        mFinished = true;

        Entity *const player = getFixtureUserData(wanted)->getEntity();
        SoundPlayer::play("Finish.wav", toSFMLCoords(player->getPosition()));
        break;
      }
      default: {
        CHECK(shouldCollideWithPlayer(otherType) || isGround(otherType));

        break;
      }
      }
    }
  }

  void EndContact(b2Contact *contact) final {
    const b2Fixture *wanted = nullptr;
    const b2Fixture *other = nullptr;

    if (findFixtureType(contact, OBJECT_TYPE::PLAYER_SENSOR, &wanted, &other)) {
      const auto otherType = getFixtureUserData(other)->getType();

      CHECK(isGround(otherType));

      mPlayerContactNum--;

      if (isPlatform(otherType)) {
        const b2Fixture *const playerFixture = wanted->GetNext();

        NOT_NULL(playerFixture);

        Player *const player = dynamic_cast<Player *>(
            getFixtureUserData(playerFixture)->getEntity());
        player->setParentBody(nullptr);
      }
    } else if (findFixtureType(contact, OBJECT_TYPE::PLAYER, &wanted, &other)) {
      const OBJECT_TYPE otherType = getFixtureUserData(other)->getType();

      switch (otherType) {
      case OBJECT_TYPE::LADDER: {
        mLadderListener.onLadderCollision(false, other);
        break;
      }
      case OBJECT_TYPE::SOLID_ON_LADDER: {
        mLadderListener.onSolidCollision(false);
        break;
      }
      default: {
        CHECK(shouldCollideWithPlayer(otherType) || isGround(otherType));

        break;
      }
      }
    }
  }

  void PreSolve(b2Contact *contact,
                const b2Manifold * /* oldManifold */) final {
    const b2Fixture *wanted = nullptr;
    const b2Fixture *other = nullptr;

    if (findFixtureType(contact, OBJECT_TYPE::ENEMY_BULLET, &wanted, &other)) {
      const UserData *const otherUserData = getFixtureUserData(other);
      const auto otherType = otherUserData->getType();

      if (otherType == OBJECT_TYPE::PLAYER) {
        Entity *const player = otherUserData->getEntity();

        if (!player->isKilled()) {
          player->damage(1);
        }
      } else {
        CHECK(isGround(otherType));
      }

      Entity *const enemyBullet = getFixtureUserData(wanted)->getEntity();

      if (!enemyBullet->isKilled()) {
        enemyBullet->kill();
      }

      contact->SetEnabled(false);
    } else if (findFixtureType(contact, OBJECT_TYPE::PLAYER, &wanted, &other)) {
      const auto otherType = getFixtureUserData(other)->getType();

      CHECK(shouldCollideWithPlayer(otherType) || isGround(otherType));

      if (isEnemy(otherType)) {
        const auto enemyPos = other->GetBody()->GetPosition();
        const auto playerPos = wanted->GetBody()->GetPosition();
        const auto side = playerPos.x < enemyPos.x
                              ? Player::COLLISION_SIDE::RIGHT
                              : Player::COLLISION_SIDE::LEFT;

        Player *const player =
            dynamic_cast<Player *>(getFixtureUserData(wanted)->getEntity());
        player->onCollisionWithEnemy(side, 1);

        contact->SetEnabled(false);
      }
    } else if (findFixtureType(contact, OBJECT_TYPE::ALLIED_BULLET, &wanted,
                               &other)) {
      const auto otherUserData = getFixtureUserData(other);
      const auto otherType = otherUserData->getType();

      if (isEnemy(otherType)) {
        Entity *const enemy = otherUserData->getEntity();
        enemy->damage(1);
      } else {
        CHECK(isGround(otherType));
      }

      Entity *const alliedBullet = getFixtureUserData(wanted)->getEntity();

      if (!alliedBullet->isKilled()) {
        alliedBullet->kill();
      }

      contact->SetEnabled(false);
    } else {
      const auto typeA =
          (getFixtureUserData(contact->GetFixtureA()))->getType();
      const auto typeB =
          (getFixtureUserData(contact->GetFixtureB()))->getType();

      CHECK(isGround(typeA) || isGround(typeB));
    }
  }

private:
  class LadderListener : private sf::NonCopyable {
  public:
    LadderListener()
        : mLadderFixture(nullptr), mCollideWithLadder(false),
          mCollideWithSolid(false), mWasOnLadder(false) {}

    void onLadderCollision(bool enable, const b2Fixture *const ladderFixture) {
      NOT_NULL(ladderFixture);

      mCollideWithLadder = enable;
      mLadderFixture = ladderFixture;
    }

    void onSolidCollision(bool enable) { mCollideWithSolid = enable; }

    // should be performed after b2World step() function
    bool checkLadderCollisionChange(bool &isOnLadder,
                                    const b2Fixture **const ladder) {
      NOT_NULL(ladder);

      if (mLadderFixture != nullptr && (beginContact() || endContact())) {
        isOnLadder = mWasOnLadder = mCollideWithLadder;
        *ladder = mLadderFixture;
        mLadderFixture = nullptr;
        return true;
      }

      return false;
    }

  private:
    bool beginContact() const {
      return mCollideWithLadder && !mCollideWithSolid;
    }

    bool endContact() const { return mWasOnLadder && !mCollideWithLadder; }

    const b2Fixture *mLadderFixture;
    bool mCollideWithLadder;
    bool mCollideWithSolid;
    bool mWasOnLadder;
  };

  int mPlayerContactNum;
  LadderListener mLadderListener;
  bool mFinished;
};

PhysicalWorld::PhysicalWorld(const LevelParser &levelParser,
                             PhysicalBodyMap &bodyMap)
    : mContactListener(makeUnique<CustomContactListener>()),
      mContactFilter(makeUnique<CustomContactFilter>()),
      mWorld(makeUnique<b2World>(b2Vec2{0.f, GRAVITY})),
      mNonEntityUserDataArray(), mFixtureSizeMap(), mLadderSolidMap(),
      mPlayerSensorOffset(), mPlayerCallback(nullptr), mVertices(sf::Lines) {
  mWorld->SetContactListener(mContactListener.get());
  mWorld->SetContactFilter(mContactFilter.get());

  initEntityBodies(levelParser, bodyMap);
  initNonEntityBodies(levelParser);
}

PhysicalWorld::~PhysicalWorld() {}

b2Body *PhysicalWorld::createBody(const sf::FloatRect &object,
                                  OBJECT_TYPE type) {
  CHECK(type != OBJECT_TYPE::NONE);

  const auto posInPixels = getPosition(type, object);
  const b2Vec2 sizeInPixels = {object.width, object.height};

  const auto posInMeters = toB2Coords(posInPixels);
  const auto sizeInMeters = toMeters(sizeInPixels);

  b2BodyDef bodyDef;
  bodyDef.position = posInMeters;
  bodyDef.type = getBodyType(type);
  bodyDef.fixedRotation = true;

  b2PolygonShape polygon;

  if (isSlope(type)) {
    b2Vec2 vertices[TRIANGLE_VERTEX_NUM];

    createTriangle(vertices, type == OBJECT_TYPE::SLOPE_LEFT, sizeInMeters);
    polygon.Set(vertices, static_cast<int32>(arraySize(vertices)));
  } else {
    b2Vec2 vertices[RECTANGLE_VERTEX_NUM];

    createRectangle(vertices, sizeInMeters);
    polygon.Set(vertices, static_cast<int32>(arraySize(vertices)));
  }

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &polygon;
  fixtureDef.isSensor = isSensor(type);
  fixtureDef.friction = 0.f;

  b2Body *const body = mWorld->CreateBody(&bodyDef);
  const b2Fixture *const fixture = body->CreateFixture(&fixtureDef);

  CHECK(mFixtureSizeMap.emplace(fixture, sizeInPixels).second);

  return body;
}

void PhysicalWorld::createFootSensorFor(b2Body *const body,
                                        const b2Vec2 &parentSizeInPixels) {
  NOT_NULL(body);
  CHECK(mPlayerSensorOffset.x == 0.f && mPlayerSensorOffset.y == 0.f);

  const b2Vec2 sizeFactor = {1.1f, 4.f};
  const b2Vec2 sensorSizeInPixels = {parentSizeInPixels.x / sizeFactor.x,
                                     parentSizeInPixels.y / sizeFactor.y};

  const b2Vec2 sensorSizeInMeters = toMeters(sensorSizeInPixels);
  const b2Vec2 sensorCenter = {0.f, parentSizeInPixels.y / 2.f};

  b2PolygonShape polygon;
  polygon.SetAsBox(sensorSizeInMeters.x / 2.f, sensorSizeInMeters.y / 2.f,
                   toB2Coords(sensorCenter), 0.f /* angle */);

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &polygon;
  fixtureDef.isSensor = true;
  fixtureDef.friction = 0.f;

  const b2Fixture *const fixture = body->CreateFixture(&fixtureDef);

  CHECK(mFixtureSizeMap.emplace(fixture, sensorSizeInPixels).second);

  mPlayerSensorOffset = {sensorCenter.x, sensorCenter.y};
}

void PhysicalWorld::destroyBody(b2Body *const body) {
  NOT_NULL(body);

  for (const b2Fixture *fixture = body->GetFixtureList(); fixture != nullptr;
       fixture = fixture->GetNext()) {
    CHECK(mFixtureSizeMap.erase(fixture) == 1);

    auto userData = getFixtureUserData(fixture);

    if (!userData->isExtended() &&
        userData->getType() != OBJECT_TYPE::PLAYER_SENSOR) {
      const auto it = std::find_if(
          mNonEntityUserDataArray.begin(), mNonEntityUserDataArray.end(),
          [userData](const std::unique_ptr<UserData> &p) {
            return p.get() == userData;
          });

      CHECK(it != mNonEntityUserDataArray.end());

      /*const auto nextIt = */ mNonEntityUserDataArray.erase(it);
    }
  }

  mWorld->DestroyBody(body);
}

void PhysicalWorld::update(sf::Time dt) {
  mWorld->Step(static_cast<float32>(dt.asSeconds()), mVelocityIterations,
               mPositionIterations);

  NOT_NULL(mPlayerCallback);

  const bool onGround = mContactListener->playerOnGround();
  mPlayerCallback->onGround(onGround);

  bool isOnLadder = false;
  const b2Fixture *ladder = nullptr;

  if (mContactListener->checkLadderCollisionChange(isOnLadder, &ladder)) {
    b2Fixture *const solidFixture = findSolidOnLadderFixture(ladder);
    solidFixture->SetSensor(isOnLadder);

    mPlayerCallback->onLadder(isOnLadder);
  }
}

void PhysicalWorld::setPlayerCallback(PlayerCallback *const callback) {
  NOT_NULL(callback);

  mPlayerCallback = callback;
}

bool PhysicalWorld::finished() const { return mContactListener->finished(); }

void PhysicalWorld::initEntityBodies(const LevelParser &parser,
                                     PhysicalBodyMap &bodyMap) {
  const auto end = OBJECT_TYPE::ENTITY_TYPES_END;

  for (auto type = OBJECT_TYPE::ENTITY_TYPES_START; type < end;
       type = static_cast<OBJECT_TYPE>(static_cast<size_t>(type) + 1)) {
    if (!parser.hasType(type)) {
      CHECK(!parser.isRequiredType(type));

      continue;
    }

    const auto insertResult = bodyMap.emplace(type, PhysicalBodyVector{});

    CHECK(insertResult.second);

    auto &bodies = insertResult.first->second;

    for (const auto &object : parser.getObjectsFor(type)) {
      auto body = makeUnique<PhysicalBody>(*this, object, type);
      bodies.push_back(std::move(body));
    }
  }
}

void PhysicalWorld::initNonEntityBodies(const LevelParser &parser) {
  const auto end = OBJECT_TYPE::NON_ENTITY_TYPES_END;

  for (auto type = OBJECT_TYPE::NON_ENTITY_TYPES_START; type < end;
       type = static_cast<OBJECT_TYPE>(static_cast<size_t>(type) + 1)) {
    if (!parser.hasType(type)) {
      CHECK(!parser.isRequiredType(type));

      continue;
    }

    for (const auto &object : parser.getObjectsFor(type)) {
      auto userData = makeUnique<UserData>(type);
      mNonEntityUserDataArray.push_back(std::move(userData));
      /*b2Body* const rawBody = */ createBody(
          object, mNonEntityUserDataArray.back().get());
    }
  }

  const auto ladderType = OBJECT_TYPE::LADDER;

  if (!parser.hasType(ladderType)) {
    return;
  }

  const auto solidType = OBJECT_TYPE::SOLID_ON_LADDER;

  for (const auto &object : parser.getObjectsFor(ladderType)) {
    auto ladderUserData = makeUnique<UserData>(ladderType);
    mNonEntityUserDataArray.push_back(std::move(ladderUserData));

    b2Body *const ladderRawBody =
        createBody(object, mNonEntityUserDataArray.back().get());
    b2Fixture *const ladderFixture = ladderRawBody->GetFixtureList();

    NOT_NULL(ladderFixture);
    IS_NULL(ladderFixture->GetNext());

    const sf::FloatRect solidObject = {object.left, object.top, object.width,
                                       mSolidOnLadderHeight};

    auto solidUserData = makeUnique<UserData>(solidType);
    mNonEntityUserDataArray.push_back(std::move(solidUserData));
    b2Body *const solidRawBody =
        createBody(solidObject, mNonEntityUserDataArray.back().get());
    b2Fixture *const solidFixture = solidRawBody->GetFixtureList();

    NOT_NULL(solidFixture);
    IS_NULL(solidFixture->GetNext());

    CHECK(mLadderSolidMap.emplace(ladderFixture, solidFixture).second);
  }
}

b2Body *PhysicalWorld::createBody(const sf::FloatRect &bounds,
                                  UserData *const userData) {
  NOT_NULL(userData);

  b2Body *const rawBody = createBody(bounds, userData->getType());
  b2Fixture *const fixture = rawBody->GetFixtureList();

  NOT_NULL(fixture);
  IS_NULL(fixture->GetNext());

  fixture->SetUserData(userData);

  return rawBody;
}

void PhysicalWorld::draw(sf::RenderTarget &target,
                         sf::RenderStates states) const {
  mVertices.clear();

  for (const b2Body *body = mWorld->GetBodyList(); body != nullptr;
       body = body->GetNext()) {
    const auto &posInMeters = body->GetPosition();
    const b2Fixture *const fixture = body->GetFixtureList();

    NOT_NULL(fixture);

    const auto &posInPixels = toSFMLCoords(posInMeters);
    const auto &sizeInPixels = findFixtureSize(fixture);
    const sf::Color color = getColor(body->GetType());
    const auto userDataType = getFixtureUserData(fixture)->getType();

    CHECK(fixture->GetNext() == nullptr ||
          userDataType == OBJECT_TYPE::PLAYER_SENSOR);

    if (userDataType == OBJECT_TYPE::PLAYER_SENSOR) {
      const b2Fixture *const playerFixture = fixture->GetNext();

      NOT_NULL(playerFixture);

      const auto &playerSizeInPixels = findFixtureSize(playerFixture);
      const auto sensorPos =
          posInPixels + b2Vec2{mPlayerSensorOffset.x, mPlayerSensorOffset.y};
      const auto sensorColor =
          mContactListener->playerOnGround() ? sf::Color::Yellow : color;

      addRectangleToDrawing(posInPixels, playerSizeInPixels, color);
      addRectangleToDrawing(sensorPos, sizeInPixels, sensorColor);
    } else if (isSlope(userDataType)) {
      addTriangleToDrawing(posInPixels, sizeInPixels, color,
                           userDataType == OBJECT_TYPE::SLOPE_LEFT);
    } else {
      addRectangleToDrawing(posInPixels, sizeInPixels, color);
    }
  }

  target.draw(mVertices, states);
}

void PhysicalWorld::addRectangleToDrawing(const b2Vec2 &pos, const b2Vec2 &size,
                                          const sf::Color &color) const {
  b2Vec2 vertices[RECTANGLE_VERTEX_NUM];

  createRectangle(vertices, size);
  createBoundaryAtPosition(pos, vertices, color);
}

void PhysicalWorld::addTriangleToDrawing(const b2Vec2 &pos, const b2Vec2 &size,
                                         const sf::Color &color,
                                         bool isSlopeLeft) const {
  b2Vec2 vertices[TRIANGLE_VERTEX_NUM];

  createTriangle(vertices, isSlopeLeft, size);
  createBoundaryAtPosition(pos, vertices, color);
}

sf::Vector2f PhysicalWorld::shiftVertexAtPosition(const b2Vec2 &point,
                                                  const b2Vec2 &pos) const {
  return {pos.x + point.x, pos.y - point.y};
}

const b2Vec2 &PhysicalWorld::findFixtureSize(const b2Fixture *fixture) const {
  CHECK(fixture);

  const auto it = mFixtureSizeMap.find(fixture);

  CHECK(it != mFixtureSizeMap.cend());

  return it->second;
}

b2Fixture *
PhysicalWorld::findSolidOnLadderFixture(const b2Fixture *const ladder) const {
  CHECK(ladder != nullptr);

  const auto it = mLadderSolidMap.find(ladder);

  CHECK(it != mLadderSolidMap.cend());

  return it->second;
}
