#include "StateVariables.h"

namespace state{
  uint8_t mode = MANUAL;

  float leftSpeedPercentage = 0;
  float rightSpeedPercentage = 0;

  float targetLeftRPM = 0;
  float targetRightRPM = 0;

  unsigned int leftThreshold = 74;
  unsigned int rightThreshold = 140;
  float maxDistance = 15.0;

  bool stopped = true;
};