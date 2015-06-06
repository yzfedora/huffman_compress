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
#include "huffman.h"
#include "huffman_tree.h"

/************************************************************************
 * 	Implementations of Huffman compress and decompress		*
 *									*
 ************************************************************************/
struct huffman *huffman_new(char *file, int flags)
{
	struct huffman *hfm;
	char buf[BUFSZ];

	/* Force to allocate the memory */
	while ((hfm = (struct huffman *)malloc(sizeof(*hfm))) == NULL);
	bzero(hfm, sizeof(*hfm));

	strcpy(hfm->file, file);
	hfm->flags = flags;
	hfm->htree = huffman_tree_new(hfm->file);
	hfm->compress = huffman_compress;
	hfm->decompress = huffman_decompress;
	hfm->delete = huffman_delete;
	
	/* Open the file which would be compressed */
	if ((hfm->rfd = open(hfm->file, O_RDONLY)) == -1)
		goto open_failure;
	
	strcpy(buf, hfm->file);
	if (flags & HFM_COMPRESS) {
		strcat(buf, ".hcp");
	} else if (flags & HFM_DECOMPRESS) {
		if (strncmp(file + strlen(file) - 4, ".hcp", 4) != 0) {
			fprintf(stderr, "The file \"%s\" may not a huffman"
					" compressed file.\n", hfm->file);
			exit(EXIT_FAILURE);
		}
		*(buf + strlen(buf) - 4) = 0;
	}

	/* Default file permisson is ((S_IRWXU & ~S_IXISR) | S_IRGRP)*/
	if ((hfm->wfd = open(buf, O_WRONLY | O_CREAT, 0640)) == -1)
		goto open_failure;


	return hfm;
open_failure:
	perror(NULL);
	hfm->delete(hfm);
	return NULL;
}



/* Write n bytes data to the fd, absolutely */
static int writen(const int fd, const void *buf, unsigned int len)
{
	unsigned int n;

	/*printf("Write %d bytes to file descriptor %d\n", len, fd);*/
	while (len > 0) {
		n = write(fd, buf, len);
		if (n == -1)
			return -1;

		len -= n;
		buf += n;
	}
	return 0;
}

/* Write frequency and file length in bytes to the end of compressed file */
static int write_additional_data(struct huffman *hfm)
{
	int i;
	struct huffman_node *node;

	/* Get length of the file, and recover the offset of the file */
	hfm->size = lseek(hfm->rfd, 0, SEEK_END);
	lseek(hfm->rfd, 0, SEEK_SET);

	node = hfm->htree->h_node;
	/* Write file length to the end of file for decompress, 4bytes */
	writen(hfm->wfd, &hfm->size, sizeof(hfm->size));
	/* Write frequency to the compressed file, for rebuild huffman tree */
	for (i = 0; i < HFM_LEAF_MAX; i++)
		writen(hfm->wfd, &node[i].freq, sizeof(node[i].freq));
	return 0;
}

static int read_additional_data(struct huffman *hfm)
{
	int i;
	
	read(hfm->rfd, &hfm->size, sizeof(hfm->size));
	for (i = 0; i < HFM_LEAF_MAX; i++) {
		read(hfm->rfd, &hfm->htree->h_node[i].freq,
				sizeof(hfm->htree->h_node[0].freq));
	}
	
	return 0;
}

/* Compress the file using huffman codes */
int huffman_compress(struct huffman *hfm)
{
	int n;
	unsigned int bits, code, i = 8, cnt = 1;
	char buf[BUFSZ], tmp[BUFSZ];
	char *p1, *p2 = tmp;
	/* using the filename to construct a huffman_tree object */
	struct huffman_tree *htree;
	struct huffman_node *node;

	htree = hfm->htree;
	node = htree->h_node;
	/* Construct the huffman tree */
	htree->get_frequency(htree);
	htree->construct_huffman_tree(htree);
	/* Construct the huffman table */
	htree->construct_huffman_table(htree);
	/*htree->print(htree);*/

	write_additional_data(hfm);
	while ((n = read(hfm->rfd, buf, BUFSZ)) > 0) {
		for (p1 = buf; p1 < buf + n; p1++) {
			bits = node[(int)*p1].bits;
			code = node[(int)*p1].code;
compress_bitwise:
			if (i >= bits) {
				i -= bits;
				*p2 |= code << i;
			} else {
				bits -= i;
				*p2 |= code >> bits;
				code = code << (8 - bits) >> (8 - bits);
				i = 8;
				/* Buffer is full */
				if (cnt == BUFSZ) {
					writen(hfm->wfd, tmp, BUFSZ);
					p2 = tmp;
					cnt = 1;
				} else {
					p2++;
					cnt++;
				}

				if (bits > 0)
					goto compress_bitwise;
			}
		}
	}

	/* Write remaining compressed data to the file */
	writen(hfm->wfd, tmp, cnt);
	return 0;
}


/* Decompress the file using huffman codes */
int huffman_decompress(struct huffman *hfm)
{
	int n;
	unsigned int bitwise, count = 0, cnt = 1;
	char buf[BUFSZ], tmp[BUFSZ];
	char *p1, *p2 = tmp;
	/* using the filename to construct a huffman_tree object */
	struct huffman_tree *htree;
	struct huffman_node *ptr;

	read_additional_data(hfm);
	htree = hfm->htree;
	
	htree->construct_huffman_tree(htree);
	htree->construct_huffman_table(htree);

	/*htree->print(htree);*/
	ptr = htree->h_ptr;
	while ((n = read(hfm->rfd, buf, BUFSZ)) > 0) {
		bitwise = 0x80;
		for (p1 = buf; p1 < buf + n; p1++) {
			bitwise = 0x80;
			while (1) {
				if (*p1 & bitwise) {
					ptr = ptr->right;
				} else {
					ptr = ptr->left;
				}
				if (ptr->val != (unsigned short)INVALID_VAL) {
					*p2 = (char)ptr->val;
					if (cnt == BUFSZ) {
						writen(hfm->wfd, tmp, cnt);
						p2 = tmp;
						cnt = 1;
					} else {
						cnt++, p2++, count++;
					}
					if (count == hfm->size) {
						writen(hfm->wfd, tmp, cnt);
						goto decompress_finish;
					}
					ptr = htree->h_ptr;
				}
				if (!(bitwise >>= 1))
					break;
			}
		}
	}
decompress_finish:
	/* The compressed file include the 0x0a, indicates the end of file,
	 * but the decompressed file is contains 0x0a also, so I truncate
	 * the length of file to orignal size. */
	if (ftruncate(hfm->wfd, hfm->size) == -1)
		return -1;
	return 0;
}

inline void huffman_delete(struct huffman *hfm)
{
	hfm->htree->delete(hfm->htree);
	free(hfm);
}
