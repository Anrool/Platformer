#ifndef ANIMATIONPARSER_H
#define ANIMATIONPARSER_H

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>

#include <map>
#include <memory>
#include <vector>

enum class OBJECT_TYPE;
class AnimationManager;
enum class ANIMATION_TYPE;

class AnimationParser : private sf::NonCopyable {
public:
  static void createInstance();

  static std::unique_ptr<AnimationManager> createManagerFor(OBJECT_TYPE type);

private:
  using ObjectPair = std::pair<std::string, OBJECT_TYPE>;
  using FrameArray = std::vector<sf::IntRect>;

  static std::unique_ptr<const AnimationParser> mInstance;

  static const std::map<std::string, ANIMATION_TYPE> mAnimationNameTypeMap;

  static const ObjectPair mFileToObjectTypeMap[];
  static const std::string mObjFileExtension;
  static const std::string mSpritesElemName;
  static const std::string mImageAttributeName;
  static const std::string mAnimationElemName;
  static const std::string mTitleAttributeName;
  static const std::string mDelayAttributeName;
  static const std::string mCutAttributeName;

  struct AnimationData {
    ANIMATION_TYPE mType;
    const std::shared_ptr<const FrameArray> mFrames;
    sf::Time mDuration;
    bool mLoop;

    explicit AnimationData(ANIMATION_TYPE type,
                           const std::shared_ptr<const FrameArray> &frames,
                           sf::Time duration, bool loop);
  };

  struct ManagerData {
    std::string mTextureName;
    std::vector<AnimationData> mAnimationDataArray;
  };

  std::map<const OBJECT_TYPE, ManagerData> mManagerDataMap;

  static const AnimationParser &getInstance();

  AnimationParser();

  void parseFile(const ObjectPair &objPair);

  ANIMATION_TYPE animationNameToType(const std::string &name) const;
};

#endif // ANIMATIONPARSER_H
