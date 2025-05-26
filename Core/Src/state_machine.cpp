#include "state_machine.h"

State_Machine::State_Machine(Data *data) {
    this->data = data;
}

State State_Machine::update() {
    // Run through all the the switch conditions

    switch (this->state) {
        case INITIALIZING:
            switch_to_system_checks();
            break;
        case SYSTEM_CHECKS:
            switch_to_localizing();
            break;
        case LOCALIZING:
            switch_to_ready_for_flight();
            break;
        case READY_FOR_FLIGHT:
            switch_to_accelerating();
            break;
        case ACCELERATING:
            switch_to_ascending();
            switch_to_landed(); // Something is VERY wrong ¯\_(ツ)_/¯
            break;
        case ASCENDING:
            switch_to_descending();
            switch_to_accelerating();
            break;
        case DESENDING:
            switch_to_under_drouge();
            switch_to_landed();
            break;
        case UNDER_DROUGE:
            switch_to_under_main();
            switch_to_landed();
            break;
        case UNDER_MAIN:
            switch_to_landed();
            break;
        case LANDED:
            break;
    }

    return this->state;
}

State State_Machine::get_state() {
    return this->state;
}

void State_Machine::reset() {
    this->state = INITIALIZING;
}

void switch_to_system_checks() {
}

void switch_to_localizing() {
}

void switch_to_ready_for_flight() {
}

void switch_to_accelerating() {
}

void switch_to_ascending() {
}

void switch_to_descending() {
}

void switch_to_under_drouge() {
}

void switch_to_under_main() {
}

void switch_to_landed() {
}