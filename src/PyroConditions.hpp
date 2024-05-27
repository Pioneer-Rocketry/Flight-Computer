#ifndef PyroConditions_hpp
#define PyroConditions_hpp

#include "Data.hpp"
#include "State_Machine.h"

#define MainChutesTargetAlt 1000

bool ApogeeChutes(Data* data) {
  if (data->state == State::descent) {
    return true;
  }

  return false;
}

bool MainChute(Data* data) {
  if (data->state == State::parachute_descent && data->filted_alt < MainChutesTargetAlt) {
    return true;
  }

  return false;
}

bool TestCondition(Data* data) {
  if (data->time > 5.0) {
    return true;
  }

  return false;
}


#define StageMinAlt 100
#define StageMaxAlt 200
#define StageMaxAngle 10

bool Stage(Data* data) {
  // If state is unpowered flight, and between min and max altitudes
  // Also if angle at anytime was +- StageMaxAngle

  // if (data->angleX > StageMaxAngle || data->angleX < -StageMaxAngle 
  //     || data->angleY > StageMaxAngle || data->angleY < -StageMaxAngle
  //     || data->angleZ > StageMaxAngle || data->angleZ < -StageMaxAngle) {
  //   data->allowStage = 1; // Dont allow staging    
  // }
  
  if (data->state == State::unpowered_flight
      && data->filted_alt > StageMinAlt 
      && data->filted_alt < StageMaxAlt
      && data->allowStage == 0) {
    return true;
  }

  return false;
}

#endif