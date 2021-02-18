#pragma once
#include "Yggdrasil/Defines.h"
///
/// All Keycodes are taken from GLFW.
/// Credits go to GLFW.
/// 

/// The unknown key
#define YGGDRASIL_KEY_UNKNOWN            -1

/// Printable keys
#define YGGDRASIL_KEY_SPACE              32
#define YGGDRASIL_KEY_APOSTROPHE         39  /* ' */
#define YGGDRASIL_KEY_COMMA              44  /* , */
#define YGGDRASIL_KEY_MINUS              45  /* - */
#define YGGDRASIL_KEY_PERIOD             46  /* . */
#define YGGDRASIL_KEY_SLASH              47  /* / */
#define YGGDRASIL_KEY_0                  48
#define YGGDRASIL_KEY_1                  49
#define YGGDRASIL_KEY_2                  50
#define YGGDRASIL_KEY_3                  51
#define YGGDRASIL_KEY_4                  52
#define YGGDRASIL_KEY_5                  53
#define YGGDRASIL_KEY_6                  54
#define YGGDRASIL_KEY_7                  55
#define YGGDRASIL_KEY_8                  56
#define YGGDRASIL_KEY_9                  57
#define YGGDRASIL_KEY_SEMICOLON          59  /* ; */
#define YGGDRASIL_KEY_EQUAL              61  /* = */
#define YGGDRASIL_KEY_A                  65
#define YGGDRASIL_KEY_B                  66
#define YGGDRASIL_KEY_C                  67
#define YGGDRASIL_KEY_D                  68
#define YGGDRASIL_KEY_E                  69
#define YGGDRASIL_KEY_F                  70
#define YGGDRASIL_KEY_G                  71
#define YGGDRASIL_KEY_H                  72
#define YGGDRASIL_KEY_I                  73
#define YGGDRASIL_KEY_J                  74
#define YGGDRASIL_KEY_K                  75
#define YGGDRASIL_KEY_L                  76
#define YGGDRASIL_KEY_M                  77
#define YGGDRASIL_KEY_N                  78
#define YGGDRASIL_KEY_O                  79
#define YGGDRASIL_KEY_P                  80
#define YGGDRASIL_KEY_Q                  81
#define YGGDRASIL_KEY_R                  82
#define YGGDRASIL_KEY_S                  83
#define YGGDRASIL_KEY_T                  84
#define YGGDRASIL_KEY_U                  85
#define YGGDRASIL_KEY_V                  86
#define YGGDRASIL_KEY_W                  87
#define YGGDRASIL_KEY_X                  88
#define YGGDRASIL_KEY_Y                  89
#define YGGDRASIL_KEY_Z                  90
#define YGGDRASIL_KEY_LEFT_BRACKET       91  /* [ */
#define YGGDRASIL_KEY_BACKSLASH          92  /* \ */
#define YGGDRASIL_KEY_RIGHT_BRACKET      93  /* ] */
#define YGGDRASIL_KEY_GRAVE_ACCENT       96  /* ` */
#define YGGDRASIL_KEY_WORLD_1            161 /* non-US #1 */
#define YGGDRASIL_KEY_WORLD_2            162 /* non-US #2 */

/// Function keys
#define YGGDRASIL_KEY_ESCAPE             256
#define YGGDRASIL_KEY_ENTER              257
#define YGGDRASIL_KEY_TAB                258
#define YGGDRASIL_KEY_BACKSPACE          259
#define YGGDRASIL_KEY_INSERT             260
#define YGGDRASIL_KEY_DELETE             261
#define YGGDRASIL_KEY_RIGHT              262
#define YGGDRASIL_KEY_LEFT               263
#define YGGDRASIL_KEY_DOWN               264
#define YGGDRASIL_KEY_UP                 265
#define YGGDRASIL_KEY_PAGE_UP            266
#define YGGDRASIL_KEY_PAGE_DOWN          267
#define YGGDRASIL_KEY_HOME               268
#define YGGDRASIL_KEY_END                269
#define YGGDRASIL_KEY_CAPS_LOCK          280
#define YGGDRASIL_KEY_SCROLL_LOCK        281
#define YGGDRASIL_KEY_NUM_LOCK           282
#define YGGDRASIL_KEY_PRINT_SCREEN       283
#define YGGDRASIL_KEY_PAUSE              284
#define YGGDRASIL_KEY_F1                 290
#define YGGDRASIL_KEY_F2                 291
#define YGGDRASIL_KEY_F3                 292
#define YGGDRASIL_KEY_F4                 293
#define YGGDRASIL_KEY_F5                 294
#define YGGDRASIL_KEY_F6                 295
#define YGGDRASIL_KEY_F7                 296
#define YGGDRASIL_KEY_F8                 297
#define YGGDRASIL_KEY_F9                 298
#define YGGDRASIL_KEY_F10                299
#define YGGDRASIL_KEY_F11                300
#define YGGDRASIL_KEY_F12                301
#define YGGDRASIL_KEY_F13                302
#define YGGDRASIL_KEY_F14                303
#define YGGDRASIL_KEY_F15                304
#define YGGDRASIL_KEY_F16                305
#define YGGDRASIL_KEY_F17                306
#define YGGDRASIL_KEY_F18                307
#define YGGDRASIL_KEY_F19                308
#define YGGDRASIL_KEY_F20                309
#define YGGDRASIL_KEY_F21                310
#define YGGDRASIL_KEY_F22                311
#define YGGDRASIL_KEY_F23                312
#define YGGDRASIL_KEY_F24                313
#define YGGDRASIL_KEY_F25                314
#define YGGDRASIL_KEY_KP_0               320
#define YGGDRASIL_KEY_KP_1               321
#define YGGDRASIL_KEY_KP_2               322
#define YGGDRASIL_KEY_KP_3               323
#define YGGDRASIL_KEY_KP_4               324
#define YGGDRASIL_KEY_KP_5               325
#define YGGDRASIL_KEY_KP_6               326
#define YGGDRASIL_KEY_KP_7               327
#define YGGDRASIL_KEY_KP_8               328
#define YGGDRASIL_KEY_KP_9               329
#define YGGDRASIL_KEY_KP_DECIMAL         330
#define YGGDRASIL_KEY_KP_DIVIDE          331
#define YGGDRASIL_KEY_KP_MULTIPLY        332
#define YGGDRASIL_KEY_KP_SUBTRACT        333
#define YGGDRASIL_KEY_KP_ADD             334
#define YGGDRASIL_KEY_KP_ENTER           335
#define YGGDRASIL_KEY_KP_EQUAL           336
#define YGGDRASIL_KEY_LEFT_SHIFT         340
#define YGGDRASIL_KEY_LEFT_CONTROL       341
#define YGGDRASIL_KEY_LEFT_ALT           342
#define YGGDRASIL_KEY_LEFT_SUPER         343
#define YGGDRASIL_KEY_RIGHT_SHIFT        344
#define YGGDRASIL_KEY_RIGHT_CONTROL      345
#define YGGDRASIL_KEY_RIGHT_ALT          346
#define YGGDRASIL_KEY_RIGHT_SUPER        347
#define YGGDRASIL_KEY_MENU               348

#define YGGDRASIL_MOUSE_BUTTON_1         0
#define YGGDRASIL_MOUSE_BUTTON_2         1
#define YGGDRASIL_MOUSE_BUTTON_3         2
#define YGGDRASIL_MOUSE_BUTTON_4         3
#define YGGDRASIL_MOUSE_BUTTON_5         4
#define YGGDRASIL_MOUSE_BUTTON_6         5
#define YGGDRASIL_MOUSE_BUTTON_7         6
#define YGGDRASIL_MOUSE_BUTTON_8         7
#define YGGDRASIL_MOUSE_BUTTON_LAST      YGGDRASIL_MOUSE_BUTTON_8
#define YGGDRASIL_MOUSE_BUTTON_LEFT      YGGDRASIL_MOUSE_BUTTON_1
#define YGGDRASIL_MOUSE_BUTTON_RIGHT     YGGDRASIL_MOUSE_BUTTON_2
#define YGGDRASIL_MOUSE_BUTTON_MIDDLE    YGGDRASIL_MOUSE_BUTTON_3

#define YGGDRASIL_JOYSTICK_1             0
#define YGGDRASIL_JOYSTICK_2             1
#define YGGDRASIL_JOYSTICK_3             2
#define YGGDRASIL_JOYSTICK_4             3
#define YGGDRASIL_JOYSTICK_5             4
#define YGGDRASIL_JOYSTICK_6             5
#define YGGDRASIL_JOYSTICK_7             6
#define YGGDRASIL_JOYSTICK_8             7
#define YGGDRASIL_JOYSTICK_9             8
#define YGGDRASIL_JOYSTICK_10            9
#define YGGDRASIL_JOYSTICK_11            10
#define YGGDRASIL_JOYSTICK_12            11
#define YGGDRASIL_JOYSTICK_13            12
#define YGGDRASIL_JOYSTICK_14            13
#define YGGDRASIL_JOYSTICK_15            14
#define YGGDRASIL_JOYSTICK_16            15
#define YGGDRASIL_JOYSTICK_LAST          YGGDRASIL_JOYSTICK_16

#define YGGDRASIL_GAMEPAD_BUTTON_A               0
#define YGGDRASIL_GAMEPAD_BUTTON_B               1
#define YGGDRASIL_GAMEPAD_BUTTON_X               2
#define YGGDRASIL_GAMEPAD_BUTTON_Y               3
#define YGGDRASIL_GAMEPAD_BUTTON_LEFT_BUMPER     4
#define YGGDRASIL_GAMEPAD_BUTTON_RIGHT_BUMPER    5
#define YGGDRASIL_GAMEPAD_BUTTON_BACK            6
#define YGGDRASIL_GAMEPAD_BUTTON_START           7
#define YGGDRASIL_GAMEPAD_BUTTON_GUIDE           8
#define YGGDRASIL_GAMEPAD_BUTTON_LEFT_THUMB      9
#define YGGDRASIL_GAMEPAD_BUTTON_RIGHT_THUMB     10
#define YGGDRASIL_GAMEPAD_BUTTON_DPAD_UP         11
#define YGGDRASIL_GAMEPAD_BUTTON_DPAD_RIGHT      12
#define YGGDRASIL_GAMEPAD_BUTTON_DPAD_DOWN       13
#define YGGDRASIL_GAMEPAD_BUTTON_DPAD_LEFT       14
#define YGGDRASIL_GAMEPAD_BUTTON_LAST            YGGDRASIL_GAMEPAD_BUTTON_DPAD_LEFT

#define YGGDRASIL_GAMEPAD_BUTTON_CROSS       YGGDRASIL_GAMEPAD_BUTTON_A
#define YGGDRASIL_GAMEPAD_BUTTON_CIRCLE      YGGDRASIL_GAMEPAD_BUTTON_B
#define YGGDRASIL_GAMEPAD_BUTTON_SQUARE      YGGDRASIL_GAMEPAD_BUTTON_X
#define YGGDRASIL_GAMEPAD_BUTTON_TRIANGLE    YGGDRASIL_GAMEPAD_BUTTON_Y

#define YGGDRASIL_GAMEPAD_AXIS_LEFT_X        0
#define YGGDRASIL_GAMEPAD_AXIS_LEFT_Y        1
#define YGGDRASIL_GAMEPAD_AXIS_RIGHT_X       2
#define YGGDRASIL_GAMEPAD_AXIS_RIGHT_Y       3
#define YGGDRASIL_GAMEPAD_AXIS_LEFT_TRIGGER  4
#define YGGDRASIL_GAMEPAD_AXIS_RIGHT_TRIGGER 5
#define YGGDRASIL_GAMEPAD_AXIS_LAST          YGGDRASIL_GAMEPAD_AXIS_RIGHT_TRIGGER
