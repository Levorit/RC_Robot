\# Robot Control System Documentation



\## 1. Source Path Tree

Assuming your main Arduino file is named `RobotControl`, the standard project directory structure is as follows:



```text

RobotControl/

│

├── RobotControl.ino    # Main Arduino sketch containing the logic

└── README.md           # This documentation file

```



\---



\## 2. Pin Configuration



\### Stepper Motor Pins

| Component Pin | Arduino Pin | Description |

| :--- | :--- | :--- |

| IN1 | 8 | Stepper motor coil 1 |

| IN3 | 10 | Stepper motor coil 2 |

| IN2 | 11 | Stepper motor coil 3 |

| IN4 | 12 | Stepper motor coil 4 |



\### DC Motor Driver Pins (PWM Supported)

| Motor Position | Direction | Arduino Pin | Pin Type |

| :--- | :--- | :--- | :--- |

| Front Left | Forward (`onSolIleri`) | 3 | PWM |

| Front Left | Backward (`onSolGeri`) | 5 | PWM |

| Front Right | Forward (`onSagIleri`) | 6 | PWM |

| Front Right | Backward (`onSagGeri`) | 9 | PWM |

| Rear Left\* | Forward (`arkaSolIleri`) | 2 | Digital Output |

| Rear Left\* | Backward (`arkaSolGeri`) | 4 | Digital Output |



\*\\\*Note: Rear motor pins are defined as digital outputs. For full PWM control on rear motors, they should be connected in parallel to the front motor PWM pins or assigned to available PWM pins.\*



\### Joystick \& Speed Configuration

| Component / Variable | Value / Pin | Description |

| :--- | :--- | :--- |

| X-Axis (`joyXPin`) | A0 | Analog Input |

| Y-Axis (`joyYPin`) | A1 | Analog Input |

| Center Point | 512 | Default joystick center value |

| Deadzone | 100 | Buffer zone to prevent accidental movement |

| Target Speed Limit | 191 | Maximum speed limit (75% of full power) |

| Acceleration Rate | 0.82 | Rate of speed increase/decrease (approx. 3.5s to max) |



\---



\## 3. Flow Chart



Below is the logical flow of the system's main execution loop using Mermaid syntax.



```mermaid

graph TD

&#x20;   A\[Start: setup] --> B\[Configure Pins as Output]

&#x20;   B --> C\[Set Stepper Speed to 100 RPM]

&#x20;   C --> D\[Main Loop: loop]

&#x20;   

&#x20;   D --> E\[Stepper Motor: Execute 1 Step]

&#x20;   E --> F\[Read Joystick: X and Y Values]

&#x20;   F --> G{Is Joystick in Deadzone?}

&#x20;   

&#x20;   G -- No --> H\[Determine Active Direction 1-4]

&#x20;   H --> I\[Accelerate: currentSpeed += 0.82]

&#x20;   

&#x20;   G -- Yes --> J\[Decelerate: currentSpeed -= 0.82]

&#x20;   

&#x20;   I --> K\[Clamp Speed: Max 191]

&#x20;   J --> L\[Clamp Speed: Min 0]

&#x20;   

&#x20;   K --> M{currentSpeed > 0?}

&#x20;   L --> M

&#x20;   

&#x20;   M -- Yes --> N\[motoruGorevlendir: Apply Direction and Speed]

&#x20;   M -- No --> O\[stopAll: Halt All Motors]

&#x20;   

&#x20;   N --> P\[Delay 5ms]

&#x20;   O --> P

&#x20;   P --> D

```



\---



\## 4. Function Brief and Parameter Explanation



\### `setup()`

\* \*\*Brief:\*\* Initializes the system environment. Configures the DC motor pins as outputs, sets the stepper motor speed to 100 RPM, and initializes serial communication at 9600 baud.

\* \*\*Parameters:\*\* None.



\### `loop()`

\* \*\*Brief:\*\* The main execution cycle of the program. It performs tasks in four steps: advances the stepper motor by a single step (to prevent blocking code), reads joystick inputs, calculates smooth acceleration/deceleration using a floating-point speed variable, and delegates the final speed and direction to the motor driving functions.

\* \*\*Parameters:\*\* None.



\### `motoruGorevlendir(int yon, int hiz)`

\* \*\*Brief:\*\* Acts as the routing function that translates a directional state into specific motor commands. It uses a `switch-case` structure to determine which side of the robot should move forward or backward based on the active or last known direction.

\* \*\*Parameters:\*\*

&#x20;   \* `yon` \*(int)\*: The active direction determined by the joystick (`1` = Forward, `2` = Backward, `3` = Right, `4` = Left).

&#x20;   \* `hiz` \*(int)\*: The calculated speed value (0 to 191) to be applied to the motors.



\### `hareket(int sI, int sG, int rI, int rG)`

\* \*\*Brief:\*\* Executes the hardware-level commands to drive the motors. It writes the PWM values to the front motors and applies digital logic (HIGH/LOW) to the rear motors depending on whether a forward or backward speed value is greater than zero.

\* \*\*Parameters:\*\*

&#x20;   \* `sI` \*(int)\*: PWM value for Front Left Forward (`onSolIleri`).

&#x20;   \* `sG` \*(int)\*: PWM value for Front Left Backward (`onSolGeri`).

&#x20;   \* `rI` \*(int)\*: PWM value for Front Right Forward (`onSagIleri`).

&#x20;   \* `rG` \*(int)\*: PWM value for Front Right Backward (`onSagGeri`).



\### `stopAll()`

\* \*\*Brief:\*\* Immediately halts all movement by setting all front motor PWM values to `0` and all rear motor digital states to `LOW`.

\* \*\*Parameters:\*\* None.

