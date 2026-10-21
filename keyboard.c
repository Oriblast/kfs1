#include "keyboard.h"
#include "io.h"

#define KBD_DATA   0x60
#define KBD_STATUS 0x64
#define SC_LCTRL     0x1D
#define SC_LSHIFT    0x2A
#define SC_RSHIFT    0x36
#define SC_ALT       0x38
#define SC_CAPSLOCK  0x3A
#define SC_F1        0x3B
#define SC_F10       0x44
#define SC_F11       0x57
#define SC_F12       0x58
#define SC_EXTENDED  0xE0
#define SC_RELEASED  0x80

static const char qwerty_normal[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ',
    [0x56] = '\\',
};

static const char qwerty_shift[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ',
    [0x56] = '|',
};

static const char azerty_normal[128] = {
    0, 27, '&', '\x82', '"', '\'', '(', '-', '\x8A', '_', '\x87', '\x85', ')', '=', '\b',
    '\t', 'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '^', '$', '\n',
    0, 'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', '\x97', '\xFD',
    0, '*', 'w', 'x', 'c', 'v', 'b', 'n', ',', ';', ':', '!', 0,
    '*', 0, ' ',
    [0x56] = '<',
};

static const char azerty_shift[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\xF8', '+', '\b',
    '\t', 'A', 'Z', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 0, '\x9C', '\n',
    0, 'Q', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', '%', 0,
    0, '\xE6', 'W', 'X', 'C', 'V', 'B', 'N', '?', '.', '/', '\x15', 0,
    '*', 0, ' ',
    [0x56] = '>',
};

static const char azerty_altgr[128] = {
    [0x03] = '~', [0x04] = '#', [0x05] = '{', [0x06] = '[', [0x07] = '|',
    [0x08] = '`', [0x09] = '\\', [0x0A] = '^', [0x0B] = '@', [0x0C] = ']',
    [0x0D] = '}',
};

static enum keyboard_layout layout = LAYOUT_QWERTY;
static int shift = 0;
static int ctrl = 0;
static int alt = 0;
static int altgr = 0;
static int capslock = 0;
static int extended = 0;

static char translate(uint8_t sc) {
    const char* normal = layout == LAYOUT_AZERTY ? azerty_normal : qwerty_normal;
    const char* shifted = layout == LAYOUT_AZERTY ? azerty_shift : qwerty_shift;

    if (altgr)
        return layout == LAYOUT_AZERTY ? azerty_altgr[sc] : 0;

    char c = shift ? shifted[sc] : normal[sc];

    if (capslock) {
        if (c >= 'a' && c <= 'z')
            c -= 'a' - 'A';
        else if (c >= 'A' && c <= 'Z')
            c += 'a' - 'A';
    }
    return c;
}

int keyboard_read(void) {
    if (!(inb(KBD_STATUS) & 1))
        return 0;

    uint8_t sc = inb(KBD_DATA);

    if (sc == SC_EXTENDED) {
        extended = 1;
        return 0;
    }
    int released = sc & SC_RELEASED;
    sc &= ~SC_RELEASED;
    if (extended) {
        extended = 0;
        if (sc == SC_ALT)
            altgr = !released;
        else if (sc == SC_LCTRL)
            ctrl = !released;
        else if (sc == 0x1C && !released)
            return '\n';
        return 0;
    }
    switch (sc) {
    case SC_LSHIFT:
    case SC_RSHIFT:
        shift = !released;
        return 0;
    case SC_LCTRL:
        ctrl = !released;
        return 0;
    case SC_ALT:
        alt = !released;
        return 0;
    case SC_CAPSLOCK:
        if (!released)
            capslock = !capslock;
        return 0;
    }

    if (released)
        return 0;
    if (sc >= SC_F1 && sc <= SC_F10)
        return KEY_F1 + (sc - SC_F1);
    if (sc == SC_F11)
        return KEY_F11;
    if (sc == SC_F12)
        return KEY_F12;
    if (ctrl || alt)
        return 0;
    return (uint8_t)translate(sc);
}

void keyboard_set_layout(enum keyboard_layout new_layout) {
    layout = new_layout;
}

enum keyboard_layout keyboard_get_layout(void) {
    return layout;
}
