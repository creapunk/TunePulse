#ifndef MOTION_EXECUTOR_H
#define MOTION_EXECUTOR_H

#include "generic_block.h"

struct MotionTask {
    float position;  // keeps accounted how far has been traveled
    float velocity;  // velocity
    float accel;     // acceleration
    float jerk;      // changing of acceleration
    float time;      // time
    bool is_accel;
    bool is_jerk;
} __attribute__((packed));

struct MotionTaskInt {
    int32_t transition;  // keeps accounted how far has been traveled
    int32_t velocity;    // velocity
    int32_t accel;       // acceleration
    int32_t jerk;        // changing of acceleration
    int32_t time;        // time
} __attribute__((packed));

class MotionExecutor {
private:
    BLOCK_INPUT(MotionTask, target);
    BLOCK_INPUT(float, dt);
    BLOCK_OUTPUT(MotionTaskInt, instant);

public:
    MotionExecutor(const MotionTask& target, const float& dt) : target_(target), dt_(dt) {}

    void tick();
};

void MotionExecutor::tick() {
    if (instant_.time < target_.time) {
        instant_.time += dt_;
        instant_.accel = (target_.is_jerk) ? target_.jerk * instant_.time : target_.accel;
        instant_.velocity = (target_.is_accel) ? instant_.accel * instant_.time : target_.velocity;
        instant_.transition = instant_.velocity * instant_.time;
    }
}

#endif