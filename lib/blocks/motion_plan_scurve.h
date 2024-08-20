#ifndef MOTION_PLAN_SCURVE_H
#define MOTION_PLAN_SCURVE_H

#include "generic_block.h"

/**
 * @brief Creates a scurve motion profile.
 * This class outputs values to plot a scurve motion profile.
 * @param tick Update the motion calculations.
 * @param current_pos Current position over time.
 * @param current_vel Instantaneous velocity.
 * @param current_accel Instantaneous acceleration.
 * @param current_jerk Instantaneous jerk.
 * @param current_time Time elapsed.
 */
class MotionPlanScurve : public Block {
    BLOCK_INPUT(float, position);       // input for the target position
    BLOCK_INPUT(float, max_vel);        // input for the maximum desired velocity
    BLOCK_INPUT(float, max_accel);      // input for the maximum desired acceleration
    BLOCK_INPUT(float, max_jerk);       // input for the maximum desired jerk (acceleration of acceleration)
    BLOCK_INPUT(float, dt);             // input for the timestep for instantaneous values
    BLOCK_OUTPUT(float, current_pos);   // output for the current position
    BLOCK_OUTPUT(float, current_vel);   // output for the current velocity
    BLOCK_OUTPUT(float, current_accel); // output for the current acceleration
    BLOCK_OUTPUT(float, current_jerk);  // output for the current jerk
    BLOCK_OUTPUT(float, current_time);  // output for the current time elapsed

    enum VelocityPhase {READY, ACCEL, CONSTANT, DECEL};     // class that keeps track of what part of the velocity phase
    enum AccelPhase {INCR, HOLD, DECR};               // class that keeps track of what part of the acceleration phase

    private:
        float position_tracking;                // keeps accounted how far has been traveled
        float velocity;                         // the current velocity
        float acceleration;                     // the current acceleration
        float jerk;                             // the current jerk
        float time;                             // keeps accounted how much time has elapsed
        float accel_distance;                   // logs how far it took to meet maximum velocity
        float accel_step;                       // logs the final step of acceleration to reach max_vel, used in deceleration.
        float jerk_distance;                    // logs how much velocity it took to meet maximum acceleration
        float jerk_step;                        // logs the final step of jerk to reach max_accel
        VelocityPhase v_phase = READY;          // keeps track of what velocity phase
        AccelPhase a_phase = INCR;              // keeps track of acceleration phase

    public:
        /**
         * @brief Trapezoidal motion plan constructor.
         * @param position The target position. Assumes starting position is 0.
         * @param max_vel The maximum velocity desired.
         * @param max_accel The maximum acceleration desired.
         * @param max_jerk The maximum jerk desired.
         * @param dt The timestep for each instantaneous value. 
         */
        MotionPlanScurve(const float& position,
                        const float& max_vel,
                        const float& max_accel,
                        const float& max_jerk,
                        const float& dt)
            : position_(position),
            max_vel_(max_vel),
            max_accel_(max_accel),
            max_jerk_(max_jerk),
            dt_(dt) {}

        /**
         * @brief Updates the instantaneous position, velocity, acceleration, and jerk values.
         * Updates the instantaneous position, velocity, acceleration, and jerk values per timestep inputted.
         */
        void tick() override;
};

void MotionPlanScurve::tick() {
    // time step
    time += dt_;

    switch(v_phase) {
        case READY: // Resets the internal counter variables, and moves into acceleration phase.
            position_tracking = 0;
            velocity = 0;
            acceleration = 0;
            jerk = 0;
            time = 0;
            accel_distance = 0;
            accel_step = 0;
            v_phase = ACCEL;
            a_phase = INCR;

            break;
        
        case ACCEL: // increases velocity based on max acceleration
            // set acceleration based on max jerk
            switch(a_phase) {
                case INCR: // brings to max acceleration up by max jerk
                    // sets jerk
                    jerk = max_jerk_;

                    // checks if the jerk will overshoot the maximum acceleration
                    if (acceleration + (jerk * dt_) >= max_accel_) {
                        // if it does overshoot, lowers jerk to meet maximum acceleration
                        jerk = (max_accel_ - acceleration) / dt_;
                        
                        // logs the final jerk step
                        jerk_step = jerk;

                        // sets the next tick call to move to the next phase
                        a_phase = HOLD;

                        // uses velocity as reference for decreasing acceleration
                        jerk_distance = velocity + (jerk * dt_);
                    }

                    // updates current acceleration
                    acceleration += jerk * dt_;

                    // catch triangular cases where a phase with zero jerk is not needed and can skip to decreasing acceleration
                    if (velocity + (acceleration * dt_) >= max_vel_/2) {
                        a_phase = DECR;
                    }

                    break;

                case HOLD: // holds the jerk at zero until it needs to apply jerk to remove acceleration.
                    acceleration = max_accel_;
                    if (max_vel_ - (velocity + (acceleration * dt_)) <= jerk_distance) {
                        a_phase = DECR;
                    }

                    break;

                case DECR:
                    // sets jerk to the negative max since we need to get rid of the acceleration
                    jerk = -max_jerk_;

                    //makes sure to use jerk_step in the first step
                    if (jerk_step != 0) {
                        jerk = -jerk_step;
                        jerk_step = 0;
                    }

                    // updates current acceleration
                    acceleration += jerk * dt_;

                    // tries to ease into max velocity when acceleration hits zero (close enough)
                    if ( acceleration <= 0) {
                        acceleration += -jerk * dt_;
                    }

                    break;
            }

            // checks if the acceleration will overshoot the maximum velocity
            if ( velocity + (acceleration * dt_) >= max_vel_) {
                // if it does overshoot, lowers acceleration to meet maximum velocity
                acceleration = (max_vel_ - velocity) / dt_;
                
                //logs the final acceleration step
                accel_step = acceleration;

                // sets the next tick call to move to the next phase
                v_phase = CONSTANT;
                a_phase = INCR;
            }

            // updates current velocity and position
            velocity += acceleration * dt_;
            position_tracking += velocity * dt_;

            // updates the distance it took to accelerate, so we can decelerate the same
            accel_distance = position_tracking;

            // catch triangular cases where no constant speed is needed and can skip to deceleration
            if (position_tracking >= position_/2) {
                v_phase = DECEL;
                a_phase = INCR;
            }

            break;
        
        case CONSTANT: // holds a constant velocity
            // acceleration is now zero
            acceleration = 0;

            // holds the current velocity
            position_tracking += velocity * dt_;

            // breaks out when the distance left to travel is less than accel_distance
            if (position_ - position_tracking <= accel_distance) {
                v_phase = DECEL;
            }

            break;
        
        case DECEL: // brings velocity back down according to negative max acceleration
            switch(a_phase) {
                case INCR: // brings to negative max acceleration by negative max jerk
                    // sets jerk
                    jerk = -max_jerk_;

                    // checks if the jerk will overshoot the maximum acceleration
                    if (acceleration + (jerk * dt_) <= -max_accel_) {
                        // if it does overshoot, lowers jerk to meet maximum acceleration
                        jerk = (-max_accel_ - acceleration) / dt_;
                        
                        // logs the final jerk step
                        jerk_step = jerk;

                        // sets the next tick call to move to the next phase
                        a_phase = HOLD;
                    }

                    // updates current acceleration
                    acceleration += jerk * dt_;

                    // catch triangular cases where a phase with zero jerk is not needed and can skip to decreasing acceleration
                    if (velocity + (acceleration * dt_) <= max_vel_/2) {
                        a_phase = DECR;
                    }

                    break;

                case HOLD: // holds the jerk at zero until it needs to apply jerk to remove acceleration.
                    acceleration = -max_accel_;
                    if (velocity + (acceleration * dt_) <= jerk_distance) {
                        a_phase = DECR;
                    }

                    break;

                case DECR:
                    // sets jerk to the positive max since we need to get rid of the negative acceleration
                    jerk = max_jerk_;

                    //makes sure to use jerk_step in the first step
                    if (jerk_step != 0) {
                        jerk = jerk_step;
                        jerk_step = 0;
                    }

                    // updates current acceleration
                    acceleration += jerk * dt_;

                    // tries to ease into max velocity when acceleration hits zero (close enough)
                    if ( acceleration >= 0) {
                        acceleration += -jerk * dt_;
                    }

                    break;
            }

            //makes sure to use accel_step in the first step of deceleration
            if (accel_step != 0) {
                acceleration = -accel_step;
                accel_step = 0;
            }

            // updates current velocity and position, doesn't let drop to 0
            velocity += acceleration * dt_;
            if (velocity <= 0) {
                velocity -= acceleration * dt_;
            }

            position_tracking += velocity * dt_;

            // breaks out when target position is reached
            if (position_ <= position_tracking) {
                v_phase = READY;
            }

            break;

        default:
            v_phase = READY;
    }

    // output values
    current_pos_ = position_tracking;
    current_vel_ = velocity;
    current_accel_ = acceleration;
    current_jerk_ = jerk;
    current_time_ = time;
}

#endif MOTION_PLAN_SCURVE_H