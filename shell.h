#ifndef SHELL_H
#define SHELL_H

/* Affiche le prompt sur chaque écran */
void shell_init(void);

/* Traite une touche renvoyée par keyboard_read() */
void shell_handle_key(int key);

#endif
