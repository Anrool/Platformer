#define LOG_TAG "AnimationParser"

#include "animationParser.h"
#include "animation.h"
#include "animationManager.h"
#include "animationType.h"
#include "core.h"
#include "objectType.h"
#include "resourceManager.h"
#include "utils.h"

#include "tinyxml.h"

#include <SFML/Graphics/Texture.hpp>

#include <set>

std::unique_ptr<const AnimationParser> AnimationParser::mInstance;

const std::map<std::string, ANIMATION_TYPE>
    AnimationParser::mAnimationNameTypeMap = {
        {"Stand", ANIMATION_TYPE::STAND},
        {"Walk", ANIMATION_TYPE::WALK},
        {"Jump", ANIMATION_TYPE::JUMP},
        {"Shoot", ANIMATION_TYPE::SHOOT},
        {"Crawl", ANIMATION_TYPE::CRAWL},
        {"Spin", ANIMATION_TYPE::SPIN},
        {"Hurt", ANIMATION_TYPE::HURT},
        {"Dead", ANIMATION_TYPE::DEAD},
        {"Fall", ANIMATION_TYPE::FALL},
        {"Climb", ANIMATION_TYPE::CLIMB},
        {"Idle", ANIMATION_TYPE::IDLE},

        {"RunnerWalk", ANIMATION_TYPE::RUNNER},
        {"Archer", ANIMATION_TYPE::ARCHER},

        {"AlliedBullet", ANIMATION_TYPE::ALLIED_BULLET},
        {"EnemyBullet", ANIMATION_TYPE::ENEMY_BULLET},
        {"ExplodedBullet", ANIMATION_TYPE::EXPLODED_BULLET},

        {"Transporter", ANIMATION_TYPE::PLATFORM},

        {"RavenSpeak", ANIMATION_TYPE::RAVEN_SPEAK},
        {"RavenLook", ANIMATION_TYPE::RAVEN_LOOK},
        {"RavenTurn", ANIMATION_TYPE::RAVEN_TURN},
        {"RavenFly", ANIMATION_TYPE::RAVEN_FLY},
};

const AnimationParser::ObjectPair AnimationParser::mFileToObjectTypeMap[] = {
    {"Stitch", OBJECT_TYPE::PLAYER},

    {"Runner", OBJECT_TYPE::RUNNER},
    {"Archer", OBJECT_TYPE::ARCHER},

    {"AlliedBullet", OBJECT_TYPE::ALLIED_BULLET},
    {"EnemyBullet", OBJECT_TYPE::ENEMY_BULLET},
    {"ExplodedBullet", OBJECT_TYPE::EXPLODED_BULLET},

    {"Transporter", OBJECT_TYPE::VERTICAL_PLATFORM},
    {"Transporter", OBJECT_TYPE::HORIZONTAL_PLATFORM},
    {"Raven", OBJECT_TYPE::RAVEN},
};
const std::string AnimationParser::mObjFileExtension = ".xml";
const std::string AnimationParser::mSpritesElemName = "sprites";
const std::string AnimationParser::mImageAttributeName = "image";
const std::string AnimationParser::mAnimationElemName = "animation";
const std::string AnimationParser::mTitleAttributeName = "title";
const std::string AnimationParser::mDelayAttributeName = "delay";
const std::string AnimationParser::mCutAttributeName = "cut";

void AnimationParser::createInstance() {
  /*const auto& instance = */ getInstance();
}

std::unique_ptr<AnimationManager>
AnimationParser::createManagerFor(OBJECT_TYPE type) {
  CHECK(type != OBJECT_TYPE::NONE);

  const auto &managerDataMap = getInstance().mManagerDataMap;
  const auto it = managerDataMap.find(type);

  CHECK(it != managerDataMap.cend());

  auto animationManager = makeUnique<AnimationManager>(it->second.mTextureName);

  for (const auto &data : it->second.mAnimationDataArray) {
    auto animation =
        makeUnique<Animation>(data.mFrames, data.mDuration, data.mLoop);
    animationManager->addAnimation(data.mType, std::move(animation));
  }

  return animationManager;
}

const AnimationParser &AnimationParser::getInstance() {
  if (mInstance == nullptr) {
    mInstance.reset(new (std::nothrow) AnimationParser{});

    NOT_NULL(mInstance);
  }

  return *mInstance;
}

AnimationParser::AnimationParser() : mManagerDataMap() {
  for (const auto &elem : mFileToObjectTypeMap) {
    parseFile(elem);
  }
}

void AnimationParser::parseFile(const ObjectPair &objPair) {
  CHECK(mManagerDataMap.find(objPair.second) == mManagerDataMap.cend());

  TiXmlDocument doc;

  CHECK(doc.LoadFile(ENTITIES_DIR + objPair.first + mObjFileExtension));

  TiXmlElement *const spritesElem = doc.FirstChildElement(mSpritesElemName);

  NOT_NULL(spritesElem);

  const std::string *textureName = spritesElem->Attribute(mImageAttributeName);

  NOT_NULL(textureName);

  const auto &texture = ResourceManager::getTexture(*textureName);
  const auto textureSize = static_cast<sf::Vector2i>(texture.getSize());

  const auto insertResult =
      mManagerDataMap.emplace(objPair.second, ManagerData{});

  CHECK(insertResult.second);

  auto &managerData = insertResult.first->second;
  managerData.mTextureName = *textureName;
  auto &dataArray = managerData.mAnimationDataArray;

  std::set<ANIMATION_TYPE> uniqueAnimationTypes;

  for (TiXmlElement *animationElem = spritesElem->FirstChildElement();
       animationElem != nullptr;
       animationElem = animationElem->NextSiblingElement()) {
    CHECK(animationElem->ValueStr() == mAnimationElemName);

    const std::string *const name =
        animationElem->Attribute(mTitleAttributeName);

    NOT_NULL(name);

    const auto animationType = animationNameToType(*name);

    CHECK(uniqueAnimationTypes.emplace(animationType).second);

    int frameDurationMs;

    CHECK(animationElem->QueryValueAttribute(
              mDelayAttributeName, &frameDurationMs) == TIXML_SUCCESS);
    CHECK(frameDurationMs > 0);

    const auto frames = makeShared<FrameArray>();

    for (const TiXmlElement *cutElem = animationElem->FirstChildElement();
         cutElem != nullptr; cutElem = cutElem->NextSiblingElement()) {
      CHECK(cutElem->ValueStr() == mCutAttributeName);

      frames->emplace_back();
      auto &newFrame = frames->back();

      CHECK(cutElem->QueryValueAttribute("x", &newFrame.left) == TIXML_SUCCESS);
      CHECK(cutElem->QueryValueAttribute("y", &newFrame.top) == TIXML_SUCCESS);
      CHECK(cutElem->QueryValueAttribute("w", &newFrame.width) ==
            TIXML_SUCCESS);
      CHECK(cutElem->QueryValueAttribute("h", &newFrame.height) ==
            TIXML_SUCCESS);
      CHECK(newFrame.width > 0 && newFrame.height > 0);
      CHECK(newFrame.left + newFrame.width <= textureSize.x);
      CHECK(newFrame.top + newFrame.height <= textureSize.y);
    }

    dataArray.emplace_back(animationType, frames,
                           sf::milliseconds(frameDurationMs), true /* loop */);
  }
}

ANIMATION_TYPE
AnimationParser::animationNameToType(const std::string &name) const {
  CHECK(!name.empty());

  const auto it = mAnimationNameTypeMap.find(name);

  CHECK(it != mAnimationNameTypeMap.cend());

  return it->second;
}

AnimationParser::AnimationData::AnimationData(
    ANIMATION_TYPE type, const std::shared_ptr<const FrameArray> &frames,
    sf::Time duration, bool loop)
    : mType(type), mFrames(frames), mDuration(duration), mLoop(loop) {}
