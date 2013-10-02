/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * isoextract.c
 * Copyright (C) harish 2009 <harish2704@gmail.com>
 *
 * callbacks.h is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * isoextract.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
Reference :libisofs iso_read.c
Thanks   libisofs

*/


#include <stdio.h>
#include <stdlib.h>

#include <libisofs/libisofs.h>

#include "callbacks.h"
/*
 * Little test program that extracts a file form a given ISO image.
 * Outputs file contents to stdout!
 */




static void create_file ( IsoFileSource *file, char *base_path )
{

	char *name;
	int ret,res;
	GString *current_path=g_string_new ( base_path );
	char tt[96];   // it is a dirty fix because sizeof(struct stat) evaluates 88 or somthing but sizeof(struct stat) realy needed is 96
	struct stat *info=tt;

	name = iso_file_source_get_name ( file );

	ret = iso_file_source_open ( file );
	if ( ret < 0 )
	{
		printf ( "Can't open file %d\n", ret );
	}
	ret = iso_file_source_lstat ( file, info );
	if ( ret < 0 )
	{
		printf ( "Can't open file %d\n", ret );
	}
	if ( S_ISDIR ( info->st_mode ) )
	{
		g_string_append_printf ( current_path,"/%s",name );
		mkdir ( current_path->str, 0755 );
		IsoFileSource *sub_file;
		while ( ( ret = iso_file_source_readdir ( file, &sub_file ) ) == 1 )
		{
			create_file ( sub_file,current_path->str );

		}
	}
	else
	{
		char buf[2048];
		g_string_append_printf ( current_path,"/%s",name );
		FILE *dest_file_stream;
		dest_file_stream = fopen ( current_path->str, "w" );

		while ( ( res = iso_file_source_read ( file, buf, 2048 ) ) > 0 )
		{
			fwrite ( buf, 1, res, dest_file_stream );
		}
		if ( res < 0 )
		{
			fprintf ( stderr, "Error reading, err = %d\n", res );
			return ;
		}
//		iso_file_source_close ( file );
		fclose ( dest_file_stream );

	}
	iso_file_source_close ( file );
	iso_file_source_unref ( file );
	free ( name );
}




int iso_extract ( char *dev, char *file_name ,char* dest_path, long track )
{
	int res;
	IsoFilesystem *fs;
	IsoFileSource *file;
	char patch[96]; // it is a dirty fix because sizeof(struct stat) evaluates 88 or somthing but sizeof(struct stat) realy needed is 96
	struct stat *info=patch;
	IsoDataSource *src;
	IsoReadOpts *opts;
	


	res = iso_init();
	if ( res < 0 )
	{
		fprintf ( stderr, "Can't init libisofs\n" );
		return 1;
	}

	res = iso_data_source_new_from_file ( dev, &src );
	if ( res < 0 )
	{
		fprintf ( stderr, "Error creating data source\n" );
		return 1;
	}

	res = iso_read_opts_new ( &opts, 0 );

	iso_read_opts_set_start_block ( opts,track );

	if ( res < 0 )
	{
		fprintf ( stderr, "Error creating read options\n" );
		return 1;
	}
	res = iso_image_filesystem_new ( src, opts, 1, &fs );
	if ( res < 0 )
	{
		fprintf ( stderr, "Error creating filesystem\n" );
		return 1;
	}
	iso_read_opts_free ( opts );

	res = fs->get_by_path ( fs, file_name, &file );
	if ( res < 0 )
	{
		fprintf ( stderr, "Can't get file, err = %d\n", res );
		return 1;
	}

	res = iso_file_source_lstat ( file, info );
	if ( res < 0 )
	{
		fprintf ( stderr, "Can't stat file, err = %d\n", res );
		return 1;
	}
	
	create_file (file,dest_path );
	
	iso_filesystem_unref ( fs );
	iso_data_source_unref ( src );
	iso_finish();

	return 0;
}
