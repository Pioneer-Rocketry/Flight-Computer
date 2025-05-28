#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <cstdint>

#include "data.h"

enum State {
    INITIALIZING,
    SYSTEM_CHECKS,
    LOCALIZING,
    READY_FOR_FLIGHT,
    ACCELERATING,
    ASCENDING,
    DESCENDING,
    UNDER_DROUGE,
    UNDER_MAIN,
    LANDED
};

class State_Machine {
private:
    State state = INITIALIZING;

    uint8_t stages;
    uint8_t stagesRemaining;

    Data *data;

    // State Switch Conditions
    void switch_to_system_checks();
    void switch_to_localizing();
    void switch_to_ready_for_flight();
    void switch_to_accelerating();
    void switch_to_ascending();
    void switch_to_descending();
    void switch_to_under_drouge();
    void switch_to_under_main();
    void switch_to_landed();

public:
    State_Machine(Data *data);

    State update();
    State get_state();

    void reset();
};

#endif