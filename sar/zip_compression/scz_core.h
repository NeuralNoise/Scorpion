/************************************************************************
 * SCZ_CORE.c - Core routines for SCZ compression methods.		*
 ************************************************************************/

#ifndef SCZ_CORE_H
#define SCZ_CORE_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SCZ_MAX_BUF  16777215
#define SCZFREELSTSZ 2048
#define nil 0
extern int sczbuflen;

typedef struct scz_item;		/* Data structure for holding buffer items. */

typedef struct scz_amalgam;	/* Data structure for holding markers and phrases. */

typedef struct scz_item2;		/* Data structure for holding buffer items with index. */

typedef struct scz_block_alloc;		/* List for holding references to blocks of allocated memory. */


typedef struct scz_item *new_scz_item();


typedef struct scz_item2 *new_scz_item2();


void sczfree( struct scz_item *tmppt );



void sczfree2( struct scz_item2 *tmppt );


void scz_cleanup();	/* Call after last SCZ call to free temporarily allocated */			/*  memory which will all be on the free-lists now. */



/*-----------------------*/
/* Add item to a buffer. */
/*-----------------------*/
void scz_add_item( struct scz_item **hd, struct scz_item **tl, unsigned char ch );

#endif
