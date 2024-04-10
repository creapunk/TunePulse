#include <inttypes.h>

enum MOTOR_TYPE : uint32_t // uint32_t
{
    // DC connection patterns
    M_DC = 0x1 << 0,      // DC motor base type
    M_DC_ABxx = 0x1 << 1, // AB__ connection
    M_DC_xxAB = 0x1 << 2, // __AB connection
    M_DC_AxxB = 0x1 << 3, // A__B connection
    M_DC_xABx = 0x1 << 4, // _AB_ connection
    M_DC_AxBx = 0x1 << 5, // A_B_ connection
    M_DC_xAxB = 0x1 << 6, // _A_B connection
    M_DC_AABB = 0x1 << 7, // AABB connection
    M_DC_ABAB = 0x1 << 8, // ABAB connection

    // Stepper connection patterns
    M_STEPPER = 0x1 << 15,      // Stepper motor base type
    M_STEPPER_AABB = 0x1 << 16, // AABB connection
    M_STEPPER_ABAB = 0x1 << 17, // ABAB connection
    M_STEPPER_ABBA = 0x1 << 18, // ABBA connection

    // 3-phase connection patterns
    M_3PHASE = 0x1 << 23,      // 3-phase motor base type
    M_3PHASE_ABCx = 0x1 << 24, // ABC_ connection
    M_3PHASE_ABxC = 0x1 << 25, // AB_C connection
    M_3PHASE_AxBC = 0x1 << 26, // A_BC connection
    M_3PHASE_xABC = 0x1 << 27, // _ABC connection
};