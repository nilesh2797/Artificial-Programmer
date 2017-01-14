/* Brainfuck Interpreter
 * By Stefan "MuteX" Schindler <stefan@boxbox.org>
 * Last change: 2007-05-31
 *
 * Released under the terms and conditions of the
 * GNU General Public License, version 2. See the
 * file LICENSE for more information. If the file
 * is missing, please contact the author or go to
 * http://www.gnu.org/licenses/gpl.txt to read it
 * online.
 */

#define MAX_LOOPS 32
#define CELL_BLOCK_SIZE 128
#define READ_BUFFER_SIZE 1024

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

bool parsecmdline( int argc, char **argv );
void printcells();

bool debugmode;
bool extendedmode;
char *filename;
FILE *sourcefile;
char *readbuffer;
int bytesread;
char *code;
char *cell;
unsigned long codepos, codesize, codebuffersize;
unsigned long cellpos, cellcount, lastcell;
unsigned long ignoreloops;
unsigned long looppos[MAX_LOOPS];
unsigned long currentloop;
bool error;


int main( int argc, char **argv )
{
	srand( time( NULL ) );

	if( !parsecmdline( argc, argv ) )
		return 1;

	code = NULL;
	codesize = codebuffersize = codepos = 0;
	cellpos = lastcell = cellcount = 0;
	error = false;
	currentloop = 0;

	cell = (char*)calloc( CELL_BLOCK_SIZE, 1 );
	cellcount = CELL_BLOCK_SIZE;

	sourcefile = fopen( filename, "r" );
	if( !sourcefile ) {
		printf( "Fatal error: Couldn't open '%s'.\n", argv[1] );
		return 1;
	}

	/* Read code from file */
	readbuffer = (char*)malloc( READ_BUFFER_SIZE );

	while( !feof( sourcefile ) )
	{
		bytesread = fread( readbuffer, 1, READ_BUFFER_SIZE, sourcefile );
		codesize += bytesread;
		code = (char*)realloc( code, codesize );
		memcpy( code + codesize - bytesread, readbuffer, bytesread );
	}

	free( readbuffer );
	fclose( sourcefile );


	/* Interpret code */
	while( codepos < codesize ) {
		switch( code[codepos] ) {
			case '+':
				cell[cellpos]++;
				break;
			case '-':
				cell[cellpos]--;
				break;
			case '.':
				putchar( cell[cellpos] );
				break;
			case ',':
				cell[cellpos] = getchar();
				break;

			case '<':
				if( cellpos == 0 ) {
					printf( "Runtime error:%lu: Cell index out of range (<0).\n", codepos );
					error = true;
				}
				else
					cellpos--;

				break;

			case '>':
				cellpos++;
				/* Cell position out of range, resize */
				if( cellpos >= cellcount ) {
					unsigned long nullpos;

					cellcount += CELL_BLOCK_SIZE;
					cell = (char*)realloc( cell, cellcount );

					for( nullpos = cellpos; nullpos < cellcount; nullpos++ )
						cell[nullpos] = 0;
				}

				if( cellpos > lastcell )
					lastcell = cellpos;

				break;

			case '[':
				/* Cell value is zero, skip loop */
				if( !cell[cellpos] ) {
					ignoreloops = 1;

					while( ignoreloops ) {
						codepos++;
						if( codepos >= codesize ) { /* Check if code pointer is out of range */
							printf( "Runtime error:%lu: No matching ']'!\n", codepos );
							error = true;
							break;
						}

						if( code[codepos] == ']' )
							ignoreloops--;
						else if( code[codepos] == '[' )
							ignoreloops++;
					}

					break;
				}

				/* Cell value is NOT zero, jump into loop */
				if( currentloop >= MAX_LOOPS ) {
					printf( "Runtime error:%lu: Loop limit exceeded (%d)!\n", codepos, MAX_LOOPS );
					error = true;
					break;
				}

				looppos[currentloop] = codepos;
				currentloop++;
				break;

			case ']':
				if( currentloop == 0 ) {
					printf( "Runtime error:%lu: No matching '['!\n", codepos );
					error = true;
					break;
				}

				if( cell[cellpos] != 0 )
					codepos = looppos[currentloop-1];
				else
					currentloop--;

				break;

			default:
				if( !extendedmode ) /* When in extended mode, interpret extended instructions */
					break;

				switch( code[codepos] ) {
					case '@':
						error = true;
						break;

					case '#':
						while( code[codepos] != 13 && code[codepos] != 10 && codepos < codesize )
							codepos++;
						while( ( code[codepos] == 13 || code[codepos] == 0x10 ) && codepos < codesize )
							codepos++;

						break;

					case '_':
						cellpos = 0;
						break;

					case '|':
						cell[cellpos] = 0;
						break;

					case '?':
						{
							char input;
							printf( "\nBREAK BREAK BREAK BREAK BREAK BREAK BREAK\n" );
							printcells();
							printf( "Current cell: %d\n", (int)cellpos );
							printf( "Enter to continue, 'q'+Enter to interrupt: " );

							input = getchar();
							if( input == 'q' ) {
								printf( "Interrupted.\n" );
								error = true;
							}
						}
						
						break;

					case '%':
						printf( "%d", (int)cell[cellpos] );
						break;

					case '/':
						if( cell[cellpos] == 0 )
							break;

						codepos++;
						while( code[codepos] != ':' && codepos < codesize ) {
							codepos++;
						}
						
						if( codepos >= codesize ) {
							printf( "Runtime error:%lu: No matching ':'!\n", codepos );
							error = true;
						}

						break;

					case '~':
						cell[cellpos] = rand() % 256;
						break;
				}
				break;
		}

		if( error )
			break;

		codepos++;
	}

	if( debugmode )
		printcells();

	free( code );
	free( cell );

	return 0;
}

bool parsecmdline( int argc, char **argv )
{
	int argpos;

	if( argc <= 1 ) {
		printf( "bfinterpreter [-d][-e] <file>\n\n" );
		printf( "\t<file>       - Brainfuck source file\n" );
		printf( "\t-d           - enable debug mode (shows cell contents when the\n" );
		printf( "\t               Brainfuck program ends)\n" );
		printf( "\t-e           - enable extended mode\n" );
		printf( "\nSee README for details.\n" );
		return false;
	}

	filename = NULL;
	extendedmode = false;
	debugmode = false;

	for( argpos = 1; argpos < argc; argpos++ ) {
		if( !strcmp( argv[argpos], "-d" ) )
			debugmode = true;
		else if( !strcmp( argv[argpos], "-e" ) )
			extendedmode = true;
		else {
			if( argv[argpos][0] == '-' ) {
				printf( "Unknown option: '%s'.\n", argv[argpos] );
				return false;
			}

			filename = argv[argpos];
		}
	}

	if( filename == NULL ) {
		printf( "No source filename given!\n" );
		return false;
	}

	return true;
}

void printcells()
{
	char linebreak;
	unsigned long tempcellpos;
	linebreak = 0;

	for( tempcellpos = 0; tempcellpos <= lastcell; tempcellpos++ ) {
		linebreak++;
		printf( "% 4d: %03d ### ", (int)tempcellpos, (int)cell[tempcellpos] );
		if( linebreak % 4 == 0 )
			printf( "\n" );
	}

	if( linebreak % 4 )
		printf( "\n" );
}
