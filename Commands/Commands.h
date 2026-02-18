#pragma once

namespace comm
{
  constexpr char DELIMITER = ':';//commands will look like "LEFT:SPEED:12.3"
  constexpr char END = '\n';

  constexpr char BEAT = 'B';//GUI sends this every now and again to make sure we're connected and not stalling

  constexpr const char* LEFT = "LFT";
  constexpr const char* RIGHT = "RGHT";
  constexpr const char* SPEED = "SPD";
  constexpr const char* IR = "IR";
  constexpr const char* ULTRA_SONIC = "US";

  constexpr const char* DIRECTION = "DRCT";
  constexpr const char* W = "W";
  constexpr const char* D = "D";
  constexpr const char* A = "A";
  constexpr const char* S = "S";
  constexpr const char* STOP = "STP";
  constexpr const char* START = "SRT";

  constexpr const char* MANUAL = "MNL";
  constexpr const char* TRACK = "TRCK";
  constexpr const char* FOLLOW = "FLLW";

  constexpr const char* EVENT = "EVNT";

  constexpr const char* REMOVED_MESSAGE = "Obstacle removed";
  constexpr const char* OBSTACLE_MESSAGE = "Stopped for obstacle";
}
