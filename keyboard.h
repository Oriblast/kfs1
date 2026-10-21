#ifndef KEYBOARD_H
#define KEYBOARD_H

enum {
    KEY_F1 = 0x100, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
    KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
};

enum keyboard_layout {
    LAYOUT_QWERTY,
    LAYOUT_AZERTY,
};

int keyboard_read(void);
void keyboard_set_layout(enum keyboard_layout layout);
enum keyboard_layout keyboard_get_layout(void);

#endif
