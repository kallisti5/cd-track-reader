/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * cd_reader.c
 * Copyright (C) harish 2009 <harish2704@gmail.com>
 *
 * callbacks.h is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cd_reader.c is distributed in the hope that it will be useful, but
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
#include <string.h>
#include <limits.h>
#include <libisofs/libisofs.h>
#define GTK_ENABLE_BROKEN
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"


extern CdApp *main_app;



static void create_tree ( IsoFileSource *dir, GtkTreeStore * tree_store,GtkTreeIter parent )
{

	GtkTreeIter  child;
	int ret;
	IsoFileSource *file;
	char tt[96];   // it is a dirty fix because sizeof(struct stat) evaluates 88 or somthing but sizeof(struct stat) realy needed is 96
	struct stat *info=tt;
	ret = iso_file_source_open ( dir );
	if ( ret < 0 )
	{
		printf ( "Can't open dir %d\n", ret );
		show_error("Can't open dir ");
	}
	while ( ( ret = iso_file_source_readdir ( dir, &file ) ) == 1 )
	{


//		struct stat *info;
		char *name;
		iso_file_source_lstat ( file, info );


		name = iso_file_source_get_name ( file );
		//printf ( " %s", name );
		if ( parent.stamp )
			gtk_tree_store_append ( tree_store,&child ,&parent );
		else
			gtk_tree_store_append ( tree_store,&child ,NULL );
		gtk_tree_store_set ( tree_store, &child,0, name, -1 );

		free ( name );
		/*		if ( S_ISLNK ( info.st_mode ) )
				{
					char buf[PATH_MAX];
					iso_file_source_readlink ( file, buf, PATH_MAX );
					printf ( " -> %s\n", buf );
				}
				printf ( "\n" );
		*/


		ret = iso_file_source_lstat ( file, info );
		if ( ret < 0 )
		{
			break;
		}
		if ( S_ISDIR ( info->st_mode ) )
		{

			create_tree ( file,tree_store ,child );
		}
		if ( file )
			iso_file_source_unref ( file );
	}
	iso_file_source_close ( dir );
	if ( ret < 0 )
	{
		printf ( "Can't print dir\n" );
		show_error("Can't print dir ");
	}

}

GtkTreeStore * read_cd ( char *dev,int block )
{
	TrackData *cd_data=main_app->cd_data;
	int result;
	IsoImageFilesystem *fs;
	IsoDataSource *src;
	IsoFileSource *root;
	IsoReadOpts *ropts;


	iso_init();
	iso_set_msgs_severities ( "NEVER", "ALL", "" );

	result = iso_data_source_new_from_file ( dev, &src );
	if ( result < 0 )
	{
		printf ( "Error creating data source\n" );
		show_error("Error creating data source ");
		return NULL;
	}

	result = iso_read_opts_new ( &ropts, 0 );



	iso_read_opts_set_start_block ( ropts,cd_data[ block].track_start );

	if ( result < 0 )
	{
		fprintf ( stderr, "Error creating read options\n" );
		show_error("Error creating read options ");
		return NULL;
	}
	result = iso_image_filesystem_new ( src, ropts, 1, &fs );
	iso_read_opts_free ( ropts );
	if ( result < 0 )
	{
		printf ( "Error creating filesystem\n" );
		show_error("Error creating filesystem");
		return NULL;
	}
	GtkListStore  *store;
	GtkTreeIter    iter;
	GString *string=g_string_new("");
	store = gtk_list_store_new ( 2, G_TYPE_STRING ,G_TYPE_STRING );


	g_string_printf(string,"%ld * 2kB",cd_data[ block].track_start);
	gtk_list_store_append ( store, &iter );
	gtk_list_store_set ( store, &iter, 0, "Track Start",1,string->str,-1 );

	g_string_printf(string,"%ld * 2kB",cd_data[ block].track_end);
	gtk_list_store_append ( store, &iter );
	gtk_list_store_set ( store, &iter, 0, "Track End",1,string->str,-1 );

	g_string_printf(string,"%ld * 2kB",cd_data[ block].track_size);
	gtk_list_store_append ( store, &iter );
	gtk_list_store_set ( store, &iter, 0, "Track Size",1,string->str,-1 );

	gtk_list_store_append ( store, &iter );
	gtk_list_store_set ( store, &iter, 0, "Vol. id: ",1,iso_image_fs_get_volume_id ( fs ),-1 );
	gtk_list_store_append ( store, &iter );
	gtk_list_store_set ( store, &iter, 0, "Publisher ",1,iso_image_fs_get_publisher_id ( fs ),-1 );
	gtk_list_store_append ( store, &iter );
	gtk_list_store_set ( store, &iter, 0, "Data preparer ",1,iso_image_fs_get_data_preparer_id ( fs ),-1 );
	gtk_list_store_append ( store, &iter );
	gtk_list_store_set ( store, &iter, 0, "System ",1,iso_image_fs_get_system_id ( fs ),-1 );
	gtk_list_store_append ( store, &iter );
	gtk_list_store_set ( store, &iter, 0, "Application ",1,iso_image_fs_get_application_id ( fs ),-1 );
	gtk_list_store_append ( store, &iter );
	gtk_list_store_set ( store, &iter, 0, "Copyright ",1,iso_image_fs_get_copyright_file_id ( fs ),-1 );
	gtk_list_store_append ( store, &iter );
	gtk_list_store_set ( store, &iter, 0, "Abstract ",1,iso_image_fs_get_abstract_file_id ( fs ),-1 );
	gtk_list_store_append ( store, &iter );
	gtk_list_store_set ( store, &iter, 0, "Biblio ",1,iso_image_fs_get_biblio_file_id ( fs ),-1 );
	cd_data[ block].track_info=store;


	result = fs->get_root ( fs, &root );
	if ( result < 0 )
	{
		printf ( "Can't get root %d\n", result );
		show_error("Can't get root ");
		return NULL;
	}
	GtkTreeIter root_iter;
	GtkTreeStore * tree_store  ;
	tree_store  = gtk_tree_store_new ( 1,G_TYPE_STRING );
	root_iter.stamp=0;

	create_tree ( root, tree_store,root_iter );
	iso_file_source_unref ( root );

	fs->close ( fs );
	iso_filesystem_unref ( ( IsoFilesystem* ) fs );
	iso_data_source_unref ( src );
	iso_finish();
	return tree_store;
}

