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
#include "huffman_tree.h"

#define BUFSZ		4096

/* Prototypes of inner implementation of huffman_tree's method */
static int try_extract_min(struct huffman_tree *htree);
static struct huffman_node *extract_min(struct huffman_tree *hfm);


/* Create an object of struct huffman */
struct huffman_tree *huffman_tree_new(char *file)
{
	struct huffman_tree *htree;
	int i;

	while ((htree = (struct huffman_tree *)malloc(sizeof(*htree)))
			== NULL);
	
	/* Initialize the huffman tree object */
	bzero(htree, sizeof(*htree));
	strcpy(htree->h_file, file);
	htree->construct_huffman_tree = construct_huffman_tree;
	htree->construct_huffman_table = construct_huffman_table;
	htree->get_frequency = get_frequency;
	htree->delete = huffman_tree_delete;
	htree->print =print_huffman_tree;
	
	/* Setting value to huffman node ##*/
	for (i = 0; i < HFM_LEAF_MAX; i++)
		htree->h_node[i].val = (short)i;

	/* The node wihch great than HFM_LEAF_MAX are internal nodes */
	for (i = HFM_LEAF_MAX; i < HFM_NODES_MAX; i++) {
		htree->h_node[i].parent = INVALID_PTR;
		htree->h_node[i].val = INVALID_VAL;
	}

	return htree;
}

inline void huffman_tree_delete(struct huffman_tree *htree)
{
	free(htree);
}


/* Statistics the frequency of every bytes */
int get_frequency(struct huffman_tree *htree)
{
	int fd, n;
	unsigned char buf[BUFSZ];
	unsigned char *p;

	if ((fd = open(htree->h_file, O_RDONLY)) == -1)
		return -1;

	while ((n = read(fd, buf, BUFSZ)) > 0) {
		for (p = buf; p < buf + n; p++) {
			htree->h_node[*p].freq++;
		}
	}
	
	close(fd);
	return 0;
}


/* Return a pointer to the mininum frequncy of leaf node, or NULL if no any
 * leaf node is available. */
static struct huffman_node *extract_min(struct huffman_tree *htree)
{
	int i;
	struct huffman_node *node;

	node = NULL;
	/* Find the available node as temporary which its parent is NULL */
	for (i = 0; i < HFM_NODES_MAX; i++) {
		if ((htree->h_node[i].parent == NULL) &&
				(htree->h_node[i].parent != INVALID_PTR) &&
				(htree->h_node[i].freq > 0)) {
			node = &htree->h_node[i];
			break;
		}
	}

	/* Find the frequency of smallest node */
	for (i++; i < HFM_NODES_MAX; i++) {
		if ((htree->h_node[i].parent == NULL) &&
				(htree->h_node[i].parent != INVALID_PTR) &&
				(htree->h_node[i].freq > 0) &&
				(htree->h_node[i].freq < node->freq))
			node = &htree->h_node[i];
	}
	
	/*printf("Extract node[%ld]: val=%#x(%1$c), freq=%d\n",
			(node - htree->h_node), node->val, node->freq);*/
	return node;	
}


static int try_extract_min(struct huffman_tree *htree)
{
	int i, cnt;

	/* Find the available node as temporary which its parent is NULL */
	for (i = 0, cnt = 0; i < HFM_NODES_MAX; i++) {
		if ((htree->h_node[i].parent == NULL) &&
				(htree->h_node[i].parent != INVALID_PTR) &&
				(htree->h_node[i].freq > 0)) {
			cnt++;
		}
	}

	if (cnt < 2)
		return 0;
	return 1;
}
/* Constructing the huffman tree */
void construct_huffman_tree(struct huffman_tree *htree)
{
	int i, index;


	/* Beginning index of the internal nodes */
	index = HFM_LEAF_MAX;

	/* n leaf nodes just need to loop n-1 times, and the root of tree is
	 * the node in the last loop */
	for (i = 0; i < HFM_LEAF_MAX - 1; i++, index++) {
		if (!try_extract_min(htree))
			break;
		htree->h_node[index].left = extract_min(htree);
		htree->h_node[index].left->parent = &htree->h_node[index];
		
		htree->h_node[index].right = extract_min(htree);
		htree->h_node[index].right->parent = &htree->h_node[index];
		/* Setup parent member to NULL for indicate it's a subtree */
		htree->h_node[index].parent = NULL;
		htree->h_node[index].freq = htree->h_node[index].left->freq +
			htree->h_node[index].right->freq;
	}
	
	htree->h_ptr = &htree->h_node[--index];
}

/* Construct the Huffman Table */
static void _construct_huffman_table(struct huffman_node *node,
		unsigned int code, unsigned int h)
{
	unsigned int l, r;

	if (node == NULL)
		return;

	h++;
	l = code << 1;
	r = (code << 1) | 1;

	_construct_huffman_table(node->left, l, h);
	_construct_huffman_table(node->right, r, h);

	if (node->left == NULL && node->right == NULL) {
		node->code = code;
		node->bits = h - 1;
	}
}

inline void construct_huffman_table(struct huffman_tree *htree)
{
	_construct_huffman_table(htree->h_ptr, 0, 0);
}

/* Used to checking huffman tree whether the same  */
/*static void _traversal_huffman_tree(struct huffman_node *node)
{
	char buf[8];

	if (NULL == node)
		return;
	if (node->left == NULL && node->right == NULL) {
		sprintf(buf, "%#x", node->val);
		printf("%s", buf+2);
	}
	_traversal_huffman_tree(node->left);
	_traversal_huffman_tree(node->right);
}*/

static void _print_huffman_frequency(struct huffman_tree *htree)
{
	int i, j;
	struct huffman_node *node = htree->h_node;

	for (i = 0, j = 0; i < HFM_LEAF_MAX; i++) {

		if (node[i].freq > 0) {
			if (j % 4 == 0)
				printf("\n");

		printf("val=%#-4x code=%#-6x bits=%-2d freq=%d\n", node->val,
				node->code, node->bits, node->freq);
			j++;
		}
	}
}


/* For debug, print the frequency of every bytes */
void print_huffman_tree(struct huffman_tree *htree)
{
	/*struct winsize size;*/

	printf("Frequency: val->code(freq)\n");
	_print_huffman_frequency(htree);
	printf("\n");
	/*printf("Pre-order traverse: \n");
	_traversal_huffman_tree(htree->h_ptr);
	printf("\n");*/
	
	/* Although I want to use a good way to print huffman tree, but I
	 * realized this is not ideal. so I would to put the implementations
	 * to the source code if better ideas I have. */
	/*if (ioctl(0, TIOCGWINSZ, (char *)&size) == -1)
		return;*/
}
