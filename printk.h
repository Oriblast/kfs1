#ifndef PRINTK_H
#define PRINTK_H

/* Formats : %c %s %d %i %u %x %X %p %%, avec largeur et remplissage par 0 (ex. %08x) */
int printk(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

#endif
