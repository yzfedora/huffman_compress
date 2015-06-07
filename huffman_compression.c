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
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "huffman.h"



/* Options supported in the program */
static struct option long_options[] = {
	{"compress",	no_argument,	NULL,	'c'},
	{"decompress",	no_argument,	NULL,	'd'},
	{"help",	no_argument,	NULL,	'h'},
	{0,		0,		0,	  0}
};


/* Display the usage of this program */
static void help(const char *progname)
{
	fprintf(stderr,
		"A simple compression tool, by using Huffman codes\n\n"
		"Usage:    %s [option] [file]\n"
		"    -c, --compress   Create the compression file\n"
		"    -d, --decompress Decompress the compression file\n"
		"    -h, --help       Display the help\n",
		progname);
	exit(EXIT_FAILURE);
}



int main(int argc, char **argv)
{
	int opt, flags = 0, opt_cnt = 0;
	struct huffman *hfm;

	if (argc < 3)
		help(argv[0]);

	while ((opt = getopt_long(argc, argv, "cdh", long_options, NULL))
			!= -1) {
		switch (opt) {
		case 'c':
			flags |= HFM_COMPRESS;
			opt_cnt++;
			break;
		case 'd':
			flags |= HFM_DECOMPRESS;
			opt_cnt++;
			break;
		case 'h':
		default:
			help(argv[0]);
		}
	}

	if ((hfm = huffman_new(argv[optind], flags)) == NULL)
		exit(EXIT_FAILURE);

	/* Be sure the options are valid */
	if (opt_cnt != 1)
		help(argv[0]);
	else if (flags & HFM_COMPRESS)
		hfm->compress(hfm);
	else if (flags & HFM_DECOMPRESS)
		hfm->decompress(hfm);
	
	//hfm->delete(hfm);
	return 0;
}
