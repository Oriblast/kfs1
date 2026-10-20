#ifndef TYPES_H
#define TYPES_H

/* Types à taille fixe (pas de <stdint.h> : aucune lib de l'hôte) */
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef signed char    int8_t;
typedef signed short   int16_t;
typedef signed int     int32_t;

typedef uint32_t size_t;

#define NULL ((void*)0)

#endif
