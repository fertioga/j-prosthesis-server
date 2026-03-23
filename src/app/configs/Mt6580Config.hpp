

/** INITIAL VALUES, CAN BE CHANGE WITH BLUETOOTH INTEGRATION */
#define LIFT_THRESHOLD        1.5 // Acceleration threshold to detect a lift or upward motion
#define AIR_THRESHOLD         3.0 // Acceleration threshold indicating the device is in free motion / airborne
#define IMPACT_THRESHOLD      1.0 // Acceleration threshold to detect a strong impact or collision
#define STABLE_THRESHOLD      0.6 // Maximum acceleration variation to consider the device stable

#define STEP_COOLDOWN_MS      250 // Minimum time between detected steps to avoid double counting
#define STEP_DEBOUNCE         80  // Minimum duration a state must persist to be considered valid (debounce)

#define ACE_I2C_SPEED         100000 // I2C bus speed in Hz for accelerometer communication
#define ACE_I2C_TIMEOUT       50     // I2C communication timeout in milliseconds