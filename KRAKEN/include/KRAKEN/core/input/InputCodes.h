#pragma once
#include "KRAKEN/Defines.h"
///
/// All Keycodes are taken from GLFW.
/// Credits go to GLFW.
/// 

/// The unknown key
#define KRAKEN_KEY_UNKNOWN            -1

/// Printable keys
#define KRAKEN_KEY_SPACE              32
#define KRAKEN_KEY_APOSTROPHE         39  /* ' */
#define KRAKEN_KEY_COMMA              44  /* , */
#define KRAKEN_KEY_MINUS              45  /* - */
#define KRAKEN_KEY_PERIOD             46  /* . */
#define KRAKEN_KEY_SLASH              47  /* / */
#define KRAKEN_KEY_0                  48
#define KRAKEN_KEY_1                  49
#define KRAKEN_KEY_2                  50
#define KRAKEN_KEY_3                  51
#define KRAKEN_KEY_4                  52
#define KRAKEN_KEY_5                  53
#define KRAKEN_KEY_6                  54
#define KRAKEN_KEY_7                  55
#define KRAKEN_KEY_8                  56
#define KRAKEN_KEY_9                  57
#define KRAKEN_KEY_SEMICOLON          59  /* ; */
#define KRAKEN_KEY_EQUAL              61  /* = */
#define KRAKEN_KEY_A                  65
#define KRAKEN_KEY_B                  66
#define KRAKEN_KEY_C                  67
#define KRAKEN_KEY_D                  68
#define KRAKEN_KEY_E                  69
#define KRAKEN_KEY_F                  70
#define KRAKEN_KEY_G                  71
#define KRAKEN_KEY_H                  72
#define KRAKEN_KEY_I                  73
#define KRAKEN_KEY_J                  74
#define KRAKEN_KEY_K                  75
#define KRAKEN_KEY_L                  76
#define KRAKEN_KEY_M                  77
#define KRAKEN_KEY_N                  78
#define KRAKEN_KEY_O                  79
#define KRAKEN_KEY_P                  80
#define KRAKEN_KEY_Q                  81
#define KRAKEN_KEY_R                  82
#define KRAKEN_KEY_S                  83
#define KRAKEN_KEY_T                  84
#define KRAKEN_KEY_U                  85
#define KRAKEN_KEY_V                  86
#define KRAKEN_KEY_W                  87
#define KRAKEN_KEY_X                  88
#define KRAKEN_KEY_Y                  89
#define KRAKEN_KEY_Z                  90
#define KRAKEN_KEY_LEFT_BRACKET       91  /* [ */
#define KRAKEN_KEY_BACKSLASH          92  /* \ */
#define KRAKEN_KEY_RIGHT_BRACKET      93  /* ] */
#define KRAKEN_KEY_GRAVE_ACCENT       96  /* ` */
#define KRAKEN_KEY_WORLD_1            161 /* non-US #1 */
#define KRAKEN_KEY_WORLD_2            162 /* non-US #2 */

/// Function keys
#define KRAKEN_KEY_ESCAPE             256
#define KRAKEN_KEY_ENTER              257
#define KRAKEN_KEY_TAB                258
#define KRAKEN_KEY_BACKSPACE          259
#define KRAKEN_KEY_INSERT             260
#define KRAKEN_KEY_DELETE             261
#define KRAKEN_KEY_RIGHT              262
#define KRAKEN_KEY_LEFT               263
#define KRAKEN_KEY_DOWN               264
#define KRAKEN_KEY_UP                 265
#define KRAKEN_KEY_PAGE_UP            266
#define KRAKEN_KEY_PAGE_DOWN          267
#define KRAKEN_KEY_HOME               268
#define KRAKEN_KEY_END                269
#define KRAKEN_KEY_CAPS_LOCK          280
#define KRAKEN_KEY_SCROLL_LOCK        281
#define KRAKEN_KEY_NUM_LOCK           282
#define KRAKEN_KEY_PRINT_SCREEN       283
#define KRAKEN_KEY_PAUSE              284
#define KRAKEN_KEY_F1                 290
#define KRAKEN_KEY_F2                 291
#define KRAKEN_KEY_F3                 292
#define KRAKEN_KEY_F4                 293
#define KRAKEN_KEY_F5                 294
#define KRAKEN_KEY_F6                 295
#define KRAKEN_KEY_F7                 296
#define KRAKEN_KEY_F8                 297
#define KRAKEN_KEY_F9                 298
#define KRAKEN_KEY_F10                299
#define KRAKEN_KEY_F11                300
#define KRAKEN_KEY_F12                301
#define KRAKEN_KEY_F13                302
#define KRAKEN_KEY_F14                303
#define KRAKEN_KEY_F15                304
#define KRAKEN_KEY_F16                305
#define KRAKEN_KEY_F17                306
#define KRAKEN_KEY_F18                307
#define KRAKEN_KEY_F19                308
#define KRAKEN_KEY_F20                309
#define KRAKEN_KEY_F21                310
#define KRAKEN_KEY_F22                311
#define KRAKEN_KEY_F23                312
#define KRAKEN_KEY_F24                313
#define KRAKEN_KEY_F25                314
#define KRAKEN_KEY_KP_0               320
#define KRAKEN_KEY_KP_1               321
#define KRAKEN_KEY_KP_2               322
#define KRAKEN_KEY_KP_3               323
#define KRAKEN_KEY_KP_4               324
#define KRAKEN_KEY_KP_5               325
#define KRAKEN_KEY_KP_6               326
#define KRAKEN_KEY_KP_7               327
#define KRAKEN_KEY_KP_8               328
#define KRAKEN_KEY_KP_9               329
#define KRAKEN_KEY_KP_DECIMAL         330
#define KRAKEN_KEY_KP_DIVIDE          331
#define KRAKEN_KEY_KP_MULTIPLY        332
#define KRAKEN_KEY_KP_SUBTRACT        333
#define KRAKEN_KEY_KP_ADD             334
#define KRAKEN_KEY_KP_ENTER           335
#define KRAKEN_KEY_KP_EQUAL           336
#define KRAKEN_KEY_LEFT_SHIFT         340
#define KRAKEN_KEY_LEFT_CONTROL       341
#define KRAKEN_KEY_LEFT_ALT           342
#define KRAKEN_KEY_LEFT_SUPER         343
#define KRAKEN_KEY_RIGHT_SHIFT        344
#define KRAKEN_KEY_RIGHT_CONTROL      345
#define KRAKEN_KEY_RIGHT_ALT          346
#define KRAKEN_KEY_RIGHT_SUPER        347
#define KRAKEN_KEY_MENU               348

#define KRAKEN_MOUSE_BUTTON_1         0
#define KRAKEN_MOUSE_BUTTON_2         1
#define KRAKEN_MOUSE_BUTTON_3         2
#define KRAKEN_MOUSE_BUTTON_4         3
#define KRAKEN_MOUSE_BUTTON_5         4
#define KRAKEN_MOUSE_BUTTON_6         5
#define KRAKEN_MOUSE_BUTTON_7         6
#define KRAKEN_MOUSE_BUTTON_8         7
#define KRAKEN_MOUSE_BUTTON_LAST      KRAKEN_MOUSE_BUTTON_8
#define KRAKEN_MOUSE_BUTTON_LEFT      KRAKEN_MOUSE_BUTTON_1
#define KRAKEN_MOUSE_BUTTON_RIGHT     KRAKEN_MOUSE_BUTTON_2
#define KRAKEN_MOUSE_BUTTON_MIDDLE    KRAKEN_MOUSE_BUTTON_3

#define KRAKEN_JOYSTICK_1             0
#define KRAKEN_JOYSTICK_2             1
#define KRAKEN_JOYSTICK_3             2
#define KRAKEN_JOYSTICK_4             3
#define KRAKEN_JOYSTICK_5             4
#define KRAKEN_JOYSTICK_6             5
#define KRAKEN_JOYSTICK_7             6
#define KRAKEN_JOYSTICK_8             7
#define KRAKEN_JOYSTICK_9             8
#define KRAKEN_JOYSTICK_10            9
#define KRAKEN_JOYSTICK_11            10
#define KRAKEN_JOYSTICK_12            11
#define KRAKEN_JOYSTICK_13            12
#define KRAKEN_JOYSTICK_14            13
#define KRAKEN_JOYSTICK_15            14
#define KRAKEN_JOYSTICK_16            15
#define KRAKEN_JOYSTICK_LAST          KRAKEN_JOYSTICK_16

#define KRAKEN_GAMEPAD_BUTTON_A               0
#define KRAKEN_GAMEPAD_BUTTON_B               1
#define KRAKEN_GAMEPAD_BUTTON_X               2
#define KRAKEN_GAMEPAD_BUTTON_Y               3
#define KRAKEN_GAMEPAD_BUTTON_LEFT_BUMPER     4
#define KRAKEN_GAMEPAD_BUTTON_RIGHT_BUMPER    5
#define KRAKEN_GAMEPAD_BUTTON_BACK            6
#define KRAKEN_GAMEPAD_BUTTON_START           7
#define KRAKEN_GAMEPAD_BUTTON_GUIDE           8
#define KRAKEN_GAMEPAD_BUTTON_LEFT_THUMB      9
#define KRAKEN_GAMEPAD_BUTTON_RIGHT_THUMB     10
#define KRAKEN_GAMEPAD_BUTTON_DPAD_UP         11
#define KRAKEN_GAMEPAD_BUTTON_DPAD_RIGHT      12
#define KRAKEN_GAMEPAD_BUTTON_DPAD_DOWN       13
#define KRAKEN_GAMEPAD_BUTTON_DPAD_LEFT       14
#define KRAKEN_GAMEPAD_BUTTON_LAST            KRAKEN_GAMEPAD_BUTTON_DPAD_LEFT

#define KRAKEN_GAMEPAD_BUTTON_CROSS       KRAKEN_GAMEPAD_BUTTON_A
#define KRAKEN_GAMEPAD_BUTTON_CIRCLE      KRAKEN_GAMEPAD_BUTTON_B
#define KRAKEN_GAMEPAD_BUTTON_SQUARE      KRAKEN_GAMEPAD_BUTTON_X
#define KRAKEN_GAMEPAD_BUTTON_TRIANGLE    KRAKEN_GAMEPAD_BUTTON_Y

#define KRAKEN_GAMEPAD_AXIS_LEFT_X        0
#define KRAKEN_GAMEPAD_AXIS_LEFT_Y        1
#define KRAKEN_GAMEPAD_AXIS_RIGHT_X       2
#define KRAKEN_GAMEPAD_AXIS_RIGHT_Y       3
#define KRAKEN_GAMEPAD_AXIS_LEFT_TRIGGER  4
#define KRAKEN_GAMEPAD_AXIS_RIGHT_TRIGGER 5
#define KRAKEN_GAMEPAD_AXIS_LAST          KRAKEN_GAMEPAD_AXIS_RIGHT_TRIGGER
