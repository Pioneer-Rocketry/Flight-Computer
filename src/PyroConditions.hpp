#ifndef PyroConditions_hpp
#define PyroConditions_hpp

#include "Data.hpp"
#include "State_Machine.h"

#define MainChutesTargetAlt 1000

bool ApogeeChutes(Data* data) {
  if (data->state.value == State::descent) {
    return true;
  }

  return false;
}

bool MainChute(Data* data) {
  if (data->state.value == State::parachute_descent && data->filted_alt.value < MainChutesTargetAlt) {
    return true;
  }

  return false;
}

bool TestCondition(Data* data) {
  if (data->time.value > 5.0) {
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

  if (data->angleX.value > StageMaxAngle || data->angleX.value < -StageMaxAngle 
      || data->angleY.value > StageMaxAngle || data->angleY.value < -StageMaxAngle
      || data->angleZ.value > StageMaxAngle || data->angleZ.value < -StageMaxAngle) {
    data->allowStage.value = 1; // Dont allow staging    
  }
  
  if (data->state.value == State::unpowered_flight
      && data->filted_alt.value > StageMinAlt 
      && data->filted_alt.value < StageMaxAlt
      && data->allowStage.value == 0) {
    return true;
  }
}

#endif