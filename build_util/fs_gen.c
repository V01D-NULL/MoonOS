#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_NFILES     64
#define MAGIC          0xBF
#define FILE_NAME_SIZE 128

const char *imagePath = "../initrd.img";
const char *srcFSPath;


struct initrd_header
{
	unsigned char magic;
	char          name [ FILE_NAME_SIZE ];
	unsigned int  offset;
	unsigned int  length;
};

int main ( int argc, char **argv )
{
	int   i;
	int   sz_initrd_header;
	char* dstFile;
	char  srcFile [ FILE_NAME_SIZE ];

	int   contentSize;
	int   headerSize;
	int   fileSize;

	srcFSPath = argv[ 1 ];
	// printf( "Source path: %s\n", srcFSPath );

	int nHeaders = argc - 2;  // number of files/dirs

	struct initrd_header headers[ MAX_NFILES ];

	sz_initrd_header = sizeof( struct initrd_header );
	// printf( "Size of header: %d\n", sz_initrd_header );

	unsigned int offset = sz_initrd_header * MAX_NFILES + sizeof( int );

	// Prepare header for each file
	// for ( i = 0; i < nHeaders; i += 1 )
	for ( i = 0; i < MAX_NFILES; i += 1 )
	{
		if ( i < nHeaders )
		{
			dstFile = argv[ 2 + i ];

			strcpy( srcFile, srcFSPath );
			strcat( srcFile, dstFile );

			printf( "Writing file %s to %s at 0x%x\n", srcFile, dstFile, offset );

			// Set name
			strcpy( headers[ i ].name, dstFile );

			// Set offset
			headers[ i ].offset = offset;

			// Set length
			FILE *stream = fopen( srcFile, "r" );

			if ( stream == 0 )
			{
				printf( "Error: file not found: %s\n", srcFile );

				return 1;
			}

			fseek( stream, 0, SEEK_END );

			headers[ i ].length = ftell( stream );

			fclose( stream );

			// Set magic number
			headers[ i ].magic = MAGIC;

			//
			offset += headers[ i ].length;
		}

		// Unused, init with zeros
		else
		{
			memset( headers[ i ].name, 0, FILE_NAME_SIZE );  // not sure why not always all filled w zeros
			headers[ i ].offset = 0;
			headers[ i ].length = 0;
			headers[ i ].magic  = MAGIC;
		}
	}


	// Begin writing image
	FILE *wstream = fopen( imagePath, "w" );


	// Write number of files
	fwrite( &nHeaders, sizeof( int ), 1, wstream );

	// Write file headers
	fwrite( headers, sz_initrd_header, MAX_NFILES, wstream );

	// Write file contents
	contentSize = 0;
	for ( i = 0; i < nHeaders; i += 1 )
	{
		dstFile = argv[ 2 + i ];

		strcpy( srcFile, srcFSPath );
		strcat( srcFile, dstFile );

		FILE *stream = fopen( srcFile, "r" );

		// printf("file %s, bytes %d\n", dstFile, headers[ i ].length );

		unsigned char *buf = ( unsigned char * ) malloc( headers[ i ].length );

		fread( buf, 1, headers[ i ].length, stream );

		fwrite( buf, 1, headers[ i ].length, wstream );

		fclose( stream );

		free( buf );

		contentSize += headers[ i ].length;
	}

	fclose( wstream );


	// Report
	headerSize = sz_initrd_header * MAX_NFILES;
	fileSize = sizeof( int ) + headerSize + contentSize;

	printf( "Image is %d bytes long:\n", fileSize );
	printf( "  nFiles        uses %lu bytes\n", sizeof( int ) );
	printf( "  file headers  use  %d bytes\n", headerSize );
	printf( "  file contents use  %d bytes\n", contentSize );


	return 0;
}