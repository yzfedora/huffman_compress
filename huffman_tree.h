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
#ifndef _HUFFMAN_TREE_H
#define _HUFFMAN_TREE_H
#include <unistd.h>
#include <malloc.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <curses.h>
#include <sys/ioctl.h>

/* The INVALID node in the parent is used to indicate it is a internal node. */
#define INVALID_PTR	((struct huffman_node *)-1)
#define INVALID_VAL	((unsigned short)-1)
#define HFM_LEAF_MAX	256

struct huffman_node {
	unsigned short val;
	unsigned short code;
	unsigned short bits;
	struct huffman_node 	*parent,
				*left,
				*right;
	unsigned int freq;
};

/* Huffman_tree object used to construct the huffman tree */
struct huffman_tree {
	char h_file[PATH_MAX + NAME_MAX];
	/* Pointer to the root of huffman tree */
	struct huffman_node *h_ptr;
	/* The first 256 elements in the h_node array is used to store the
	 * leaf node, and the rest of 255 internal nodes of their parent is
	 * INVALID, there are most 511 nodes to consists the huffman tree */
#define HFM_NODES_MAX	511
	struct huffman_node h_node[HFM_NODES_MAX];

	int (*get_frequency)(struct huffman_tree *);
	void (*construct_huffman_tree)(struct huffman_tree *);
	void (*construct_huffman_table)(struct huffman_tree *);
	void (*delete)(struct huffman_tree *);
	void (*print)(struct huffman_tree *);
};


/* Interfaces of operate huffman_tree */
struct huffman_tree *huffman_tree_new(char *file);
int get_frequency(struct huffman_tree *hfm);
void construct_huffman_tree(struct huffman_tree *hfm);
inline void construct_huffman_table(struct huffman_tree *htree);
inline void huffman_tree_delete(struct huffman_tree *hfm);
void print_huffman_tree(struct huffman_tree *htree);
#endif
