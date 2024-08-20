#ifndef MOTION_PLAN_TRAPEZOIDAL_H
#define MOTION_PLAN_TRAPEZOIDAL_H

#include "generic_block.h"

/**
 * @brief Creates a Triangular motion profile.
 * This class outputs values to plot a triangular motion profile.
 * @param tick Update the motion calculations.
 * @param current_pos Current position over time.
 * @param current_vel Instantaneous velocity.
 * @param current_accel Instantaneous acceleration.
 * @param current_jerk Instantaneous jerk.
 * @param current_time Time elapsed.
 */
class MotionPlanTrapezoidal : public Block {
    BLOCK_INPUT(float, position);       // input for the target position
    BLOCK_INPUT(float, max_vel);        // input for the maximum desired velocity
    BLOCK_INPUT(float, max_accel);      // input for the maximum desired acceleration
    BLOCK_INPUT(float, dt);             // input for the timestep for instantaneous values
    BLOCK_OUTPUT(float, current_pos);   // output for the current position
    BLOCK_OUTPUT(float, current_vel);   // output for the current velocity
    BLOCK_OUTPUT(float, current_accel); // output for the current acceleration
    BLOCK_OUTPUT(float, current_jerk);  // output for the current jerk
    BLOCK_OUTPUT(float, current_time);  // output for the current time elapsed

    enum Phase {READY, ACCEL, CONSTANT, DECEL}; // class that keeps track of what part of the profile it is currently in

    private:
        float position_tracking;        // keeps accounted how far has been traveled
        float velocity;                 // the current velocity
        float acceleration;             // the current acceleration
        float jerk;                     // the current jerk
        float time;                     // keeps accounted how much time has elapsed
        float accel_distance;           // logs how far it took to meet maximum velocity
        float accel_step;               // logs the final step of acceleration to reach max_vel, used in deceleration.
        Phase current_phase = READY;    // keeps track of what part of the profile it is currently in

    public:
        /**
         * @brief Trapezoidal motion plan constructor.
         * @param position The target position. Assumes starting position is 0.
         * @param max_vel The maximum velocity desired.
         * @param max_accel The maximum acceleration desired.
         * @param dt The timestep for each instantaneous value. 
         */
        MotionPlanTrapezoidal(const float& position,
                        const float& max_vel,
                        const float& max_accel,
                        const float& dt)
            : position_(position),
            max_vel_(max_vel),
            max_accel_(max_accel),
            dt_(dt) {}

        /**
         * @brief Updates the instantaneous position, velocity, acceleration, and jerk values.
         * Updates the instantaneous position, velocity, acceleration, and jerk values per timestep inputted.
         */
        void tick() override;
};

void MotionPlanTrapezoidal::tick() {
    // time step
    time += dt_;

    switch(current_phase) {
        case READY: // Resets the internal counter variables, and moves into acceleration phase.
            position_tracking = 0;
            velocity = 0;
            acceleration = 0;
            jerk = 0;
            time = 0;
            accel_distance = 0;
            accel_step = 0;
            current_phase = ACCEL;

            break;
        
        case ACCEL: // increases velocity based on max acceleration
            // set acceleration to max
            acceleration = max_accel_;

            // checks if the acceleration will overshoot the maximum velocity
            if ( velocity + (acceleration * dt_) >= max_vel_) {
                // if it does overshoot, lowers acceleration to meet maximum velocity
                acceleration = (max_vel_ - velocity) / dt_;
                
                //logs the final acceleration step
                accel_step = acceleration;

                // sets the next tick call to move to the next phase
                current_phase = CONSTANT;
            }

            // updates current velocity and position
            velocity += acceleration * dt_;
            position_tracking += velocity * dt_;

            // updates the distance it took to accelerate, so we can decelerate the same
            accel_distance = position_tracking;

            // catch triangular cases where no constant speed is needed and can skip to deceleration
            if (position_tracking >= position_/2) {
                current_phase = DECEL;
            }

            break;
        
        case CONSTANT: // holds a constant velocity
            // holds the current velocity
            position_tracking += velocity * dt_;

            // breaks out when the distance left to travel is less than accel_distance
            if (position_ - position_tracking <= accel_distance) {
                current_phase = DECEL;
            }

            break;
        
        case DECEL: // brings velocity back down according to negative max acceleration
            // set deceleration to negative max acceleration
            acceleration = -max_accel_;

            //makes sure to use accel_step in the first step of deceleration
            if (accel_step != 0) {
                acceleration = -accel_step;
                accel_step = 0;
            }

            // updates current velocity and position
            velocity += acceleration * dt_;
            position_tracking += velocity * dt_;

            // breaks out when target position is reached or velocity hits zero (close enough)
            if (position_ <= position_tracking || velocity <= 0) {
                current_phase = READY;
            }

            break;

        default:
            current_phase = READY;
    }

    // output values
    current_pos_ = position_tracking;
    current_vel_ = velocity;
    current_accel_ = acceleration;
    current_jerk_ = jerk;
    current_time_ = time;
}





#endif MOTION_PLAN_TRAPEZOIDAL_H