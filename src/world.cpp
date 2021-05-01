#define LOG_TAG "World"

#include "world.h"
#include "animationManager.h"
#include "archer.h"
#include "bullet.h"
#include "core.h"
#include "levelParser.h"
#include "objectType.h"
#include "physicalBody.h"
#include "physicalWorld.h"
#include "platform.h"
#include "player.h"
#include "resourceManager.h"
#include "runner.h"
#include "soundPlayer.h"
#include "tileMap.h"
#include "utils.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <algorithm>

const sf::Vector2f World::mBulletSize = {10.f, 10.f};

World::World(size_t currentLevel)
    : mPhysicalWorld(), mPlayer(), mEntities(),
      mView(ResourceManager::getWindow().getDefaultView()), mTileMap(),
      mBackground(makeUnique<sf::Sprite>(
          ResourceManager::getLevelTexture(currentLevel))) {
  initPhysics(currentLevel);
}

World::~World() {}

void World::update(sf::Time dt) {
  for (const auto &entity : mEntities) {
    entity->handleRealtimeInput();
  }

  mPlayer->handleRealtimeInput();

  mPhysicalWorld->update(dt);

  const auto &playerPosition = mPlayer->getPosition();

  for (const auto &entity : mEntities) {
    if (entity->getType() == OBJECT_TYPE::ARCHER) {
      const auto &entityPosition = entity->getPosition();
      const auto newHeading =
          entityPosition.x < playerPosition.x ? HEADING::RIGHT : HEADING::LEFT;
      entity->setHeading(newHeading);
    }

    entity->update(dt);
  }

  mPlayer->update(dt);

  mEntities.remove_if([](const std::unique_ptr<Entity> &entity) {
    return entity->isDestroyed();
  });

  updateView();
  updateSoundListener();
}

bool World::failed() const {
  CHECK(mPlayer != nullptr);

  return mPlayer->isDestroyed();
}

bool World::success() const { return mPhysicalWorld->finished(); }

void World::onSpawnBullet(HEADING heading, OBJECT_TYPE type,
                          const sf::Vector2f &position) {
  CHECK(type == OBJECT_TYPE::ALLIED_BULLET ||
        type == OBJECT_TYPE::ENEMY_BULLET);

  const sf::FloatRect bounds = {position, mBulletSize};
  auto body = makeUnique<PhysicalBody>(*mPhysicalWorld, bounds, type);
  auto bullet = makeUnique<Bullet>(heading, std::move(body));

  mEntities.push_back(std::move(bullet));

  const std::string soundFile =
      type == OBJECT_TYPE::ALLIED_BULLET ? "PlayerShot.wav" : "EnemyShot.wav";
  SoundPlayer::play(soundFile, position);
}

void World::initPhysics(size_t currentLevel) {
  const auto &levelParser = ResourceManager::getLevelParser(currentLevel);
  mTileMap = makeUnique<TileMap>(levelParser.getTileMapInfo());

  PhysicalBodyMap entityBodyMap;
  mPhysicalWorld = makeUnique<PhysicalWorld>(levelParser, entityBodyMap);

  const Shooter::BulletSpawnCallback shooterCallback =
      [this](HEADING heading, OBJECT_TYPE type, const sf::Vector2f &position) {
        onSpawnBullet(heading, type, position);
      };

  for (auto &pair : entityBodyMap) {
    auto &bodies = pair.second;

    switch (pair.first) {
    case OBJECT_TYPE::PLAYER: {
      CHECK(bodies.size() == 1u);

      mPlayer = makeUnique<Player>(std::move(bodies.back()));
      mPlayer->setBulletSpawnCallback(shooterCallback);
      mPhysicalWorld->setPlayerCallback(mPlayer.get());
      break;
    }
    case OBJECT_TYPE::HORIZONTAL_PLATFORM:
    case OBJECT_TYPE::VERTICAL_PLATFORM: {
      for (auto &body : bodies) {
        auto platform = makeUnique<Platform>(std::move(body));

        mEntities.push_back(std::move(platform));
      }

      break;
    }
    case OBJECT_TYPE::RUNNER: {
      for (auto &body : bodies) {
        auto runner = makeUnique<Runner>(std::move(body));

        mEntities.push_back(std::move(runner));
      }

      break;
    }
    case OBJECT_TYPE::ARCHER: {
      for (auto &body : bodies) {
        auto archer = makeUnique<Archer>(std::move(body));
        archer->setBulletSpawnCallback(shooterCallback);

        mEntities.push_back(std::move(archer));
      }

      break;
    }
    default: {
      CHECK(false);
    }
    }
  }
}

void World::updateView() {
  const auto mapSize = static_cast<sf::Vector2f>(mTileMap->getMapSize());
  const auto viewhalfSize = mView.getSize() / 2.f;
  const auto &playerPos = mPlayer->getPosition();

  // restrict visible area
  const sf::Vector2f newViewCenter = {
      std::min(std::max(playerPos.x, viewhalfSize.x),
               mapSize.x - viewhalfSize.x),
      std::min(std::max(playerPos.y, viewhalfSize.y),
               mapSize.y - viewhalfSize.y)};

  mView.setCenter(newViewCenter);
}

void World::updateSoundListener() {
  SoundPlayer::setListenerPosition(mPlayer->getPosition());
}

void World::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  target.setView(mView);

  target.draw(*mBackground, states);
  target.draw(*mTileMap, states);

  for (const auto &entity : mEntities) {
    target.draw(*entity, states);
  }

  target.draw(*mPlayer, states);

  // target.draw(*mPhysicalWorld, states);
}
