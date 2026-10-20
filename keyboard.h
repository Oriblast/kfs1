#ifndef KEYBOARD_H
#define KEYBOARD_H

/* Touches spéciales, hors de la plage des caractères (0-255) */
enum {
    KEY_F1 = 0x100, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6,
    KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
};

enum keyboard_layout {
    LAYOUT_QWERTY,
    LAYOUT_AZERTY,
};

/* Non bloquant : renvoie 0 si aucune touche, sinon un caractère (1-255) ou une touche KEY_* */
int keyboard_read(void);
void keyboard_set_layout(enum keyboard_layout layout);
enum keyboard_layout keyboard_get_layout(void);

#endif
