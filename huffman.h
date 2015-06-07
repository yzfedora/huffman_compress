/* This is a compress tools written by using OOP and Huffman Algorithm.
 *   		Copyright (C) 2015  Yang Zhang <yzfedora@gmail.com>
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/
#ifndef _HUFFMAN_H
#define _HUFFMAN_H
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#define HFM_COMPRESS	0x1
#define HFM_DECOMPRESS	0x2

#define BUFSZ 4096

struct huffman {
	char file[PATH_MAX + NAME_MAX + 1];
	int flags;	/* to indicate which operations */
	int rfd;	/* refer to input file */
	int wfd;	/* refer to compressed file, or decompressed file */
	unsigned int size;/* original size of file */
	struct huffman_tree *htree;

	int (*compress)(struct huffman *);
	int (*decompress)(struct huffman *);
	void (*delete)(struct huffman *);
};

/* Interfaces of huffman object */
struct huffman *huffman_new(char *file, int flags);
int huffman_compress(struct huffman *hfm);
int huffman_decompress(struct huffman *hfm);
inline void huffman_delete(struct huffman *);
#endif
