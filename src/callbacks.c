/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * callbacks.c
 * Copyright (C) harish 2009 <harish2704@gmail.com>
 *
 * callbacks.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * callbacks.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#define GTK_ENABLE_BROKEN
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <fcntl.h>
#include <gtk/gtk.h>

#include "callbacks.h"


extern CdApp *main_app;



void show_error ( char *msg )
{
	GtkWidget *dialog= dialog = gtk_message_dialog_new ( NULL,
	                            GTK_DIALOG_DESTROY_WITH_PARENT,
	                            GTK_MESSAGE_ERROR,
	                            GTK_BUTTONS_CLOSE,
	                            msg );
	gtk_dialog_run ( GTK_DIALOG ( dialog ) );
	gtk_widget_destroy ( dialog );
}


void open_dev_callbk ( GtkMenuItem *menuitem,
                       CdApp         *app )

{
	GtkWidget *action_area;
	GtkWidget *entry;
	entry=gtk_entry_new( );
	gtk_entry_set_text ( entry,
	                     app->dev );
	GtkWidget *dialog= gtk_dialog_new_with_buttons ( "Enter Device File name",
	                   GTK_WINDOW ( main_app->window ),
	                   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
	                   GTK_STOCK_OK,
	                   GTK_RESPONSE_ACCEPT,
	                   NULL );
	action_area= gtk_dialog_get_content_area ( GTK_DIALOG ( dialog ) );
	gtk_container_add ( GTK_CONTAINER ( action_area ), entry );

	gtk_widget_show ( entry );

	if ( GTK_RESPONSE_ACCEPT==gtk_dialog_run ( GTK_DIALOG ( dialog ) ) )
	{
		char *entry_text;
		entry_text = gtk_entry_get_text ( GTK_ENTRY ( entry ) );
		if ( entry_text )
		{
			GString *dev_name =g_string_new ( entry_text );
			main_app->dev=dev_name->str;
			gtk_tree_view_set_model ( GTK_TREE_VIEW ( app->track_tree ),NULL );
		}

	}

	gtk_widget_destroy ( dialog );
}


void about_cb ( GtkMenuItem *menuitem,
                CdApp         *app )

{

	const char *authors[] =
	{
		"Harish.k <harish2704@gmail.com>", NULL
	};
	const char *documenters [] =
	{
		"No Documentation",
		NULL
	};

	gtk_show_about_dialog ( GTK_WINDOW ( app->window ),
	                        "version", VERSION,
	                        "copyright", "Copy Left",
	                        "comments", "Read & extract data from any session of multisession disks",
	                        "authors", authors,
	                        "documenters", documenters,
	                        "translator-credits", "translator-credits",
	                        "logo-icon-name", "cd-track-reader",
// 			       "license", license_text,
//			       "wrap-license", TRUE,
	                        NULL );


}

void                help_cb ( GtkMenuItem *menuitem,
                              CdApp         *app )
{
	const char *help_txt="Creating multi session disks may not be always succsessful.Especially on DVDs. Failure of last session makes the disk completely unmountable or takes long time for mounting.\n\tThis application scans tracks & reads the specified track only\n\tIn multi-session disks,last track will be empty(open session) so second last track is the last session you had written .So reading second last track is the same effect of mounting the disk\n\n First press \"scan tracks\"button\n Then double-click on a track for reading the track\nThen double-click on a file name or directory tree for extracting the specified file";

	GtkWidget *dialog=  gtk_message_dialog_new ( NULL,
	                    GTK_DIALOG_DESTROY_WITH_PARENT,
	                    GTK_MESSAGE_INFO,
	                    GTK_BUTTONS_CLOSE,
	                    help_txt );
	gtk_dialog_run ( GTK_DIALOG ( dialog ) );
	gtk_widget_destroy ( dialog );

}





void read_track ( GtkTreeView       *tree_view,
                  GtkTreePath        *path,
                  GtkTreeView         *file_tree )
{

	TrackData *cd_data=main_app->cd_data;
	int track_no= ( *gtk_tree_path_get_indices ( path ) );
	if ( cd_data[ track_no].file_tree!=NULL )
	{
		gtk_tree_view_set_model ( file_tree,GTK_TREE_MODEL ( cd_data[ track_no].file_tree ) );
		gtk_widget_show ( GTK_WIDGET ( file_tree ) );
	}
	else
	{
		GtkTreeStore *file_tree_store=read_cd ( main_app->dev, track_no );
		cd_data[ track_no].file_tree=file_tree_store;
		gtk_tree_view_set_model ( file_tree,GTK_TREE_MODEL ( file_tree_store ) );
		g_object_set_data ( G_OBJECT ( gtk_tree_view_get_model ( file_tree ) ),
		                    "track",
		                    & ( cd_data[ track_no] ) );
		gtk_widget_show ( GTK_WIDGET ( file_tree ) );

	}
}

void on_file_clicked_cb ( GtkTreeView       *tree_view,
                          GtkTreePath       *path,
                          GtkTreeViewColumn *column,
                          CdApp         *app )
{

	GString *abs_file_path =g_string_new ( "" );
	GtkWidget *dialog;
	GtkTreeIter   iter;
	GtkTreeModel *model;
	GtkTreePath  *parent =path;
	model = gtk_tree_view_get_model ( tree_view );
	TrackData *currnt_track=g_object_get_data ( G_OBJECT ( model ), "track" );

	do
	{
		gchar *name;
		gtk_tree_model_get_iter ( model, &iter, parent );
		gtk_tree_model_get ( model, &iter, 0, &name, -1 );
		g_string_prepend ( abs_file_path,name );
		g_string_prepend ( abs_file_path,"/" );
		gtk_tree_path_up ( parent );
	}
	while ( gtk_tree_path_get_depth ( parent ) );

	dialog = gtk_file_chooser_dialog_new ( "select folder to save file",
	                                       NULL,
	                                       GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER,
	                                       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	                                       GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
	                                       NULL );

	if ( gtk_dialog_run ( GTK_DIALOG ( dialog ) ) == GTK_RESPONSE_ACCEPT )
	{
		char *filename;
		filename = gtk_file_chooser_get_filename ( GTK_FILE_CHOOSER ( dialog ) );
		iso_extract ( app->dev,abs_file_path->str ,filename,currnt_track->track_start );
		g_free ( filename );
	}
	gtk_widget_destroy ( dialog );



}


void  on_scan_button_clicked_cb ( GtkButton *button,
                                  CdApp         *app )
{

	if ( !gtk_tree_view_get_model ( GTK_TREE_VIEW ( app->track_tree ) ) )
	{
		GtkListStore  *store;
		GtkTreeIter    iter;
		store = gtk_list_store_new ( 1, G_TYPE_STRING );
		int i;
		if ( get_no_track ( app->dev ) )
			return ;

		for ( i=1;i<=app->no_of_tracks;i++ )
		{

			char name[10];
			sprintf ( name,"Track: %d",i );
			gtk_list_store_append ( store, &iter );
			gtk_list_store_set ( store, &iter, 0, name,-1 );
		}

		gtk_tree_view_set_model ( GTK_TREE_VIEW ( app->track_tree ), GTK_TREE_MODEL ( store ) );
		g_object_unref ( store );
		gtk_widget_show ( app->track_tree );
	}
}



void
get_trak_info_popup_clicked_cb ( GtkWidget *menuitem, TrackData *track )
{



	GtkWidget * action_area,*info_tree;
	GtkTreeViewColumn   *col;
	GtkCellRenderer     *renderer;
	int track_no= track->track_no;
	TrackData *cd_data=main_app->cd_data;
//  Track Info Dialog

	GtkWidget *dialog= gtk_dialog_new_with_buttons ( "Track Info",
	                   GTK_WINDOW ( main_app->window ),
	                   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
	                   GTK_STOCK_OK,
	                   GTK_RESPONSE_ACCEPT,
	                   NULL );
	action_area= gtk_dialog_get_content_area ( GTK_DIALOG ( dialog ) );
	info_tree = gtk_tree_view_new ();
	gtk_container_add ( GTK_CONTAINER ( action_area ), info_tree );

	/* --- Column #1 --- */

	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title ( col, "Property" );
	gtk_tree_view_append_column ( GTK_TREE_VIEW ( info_tree ), col );
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start ( col, renderer, TRUE );
	gtk_tree_view_column_add_attribute ( col, renderer, "text", 0 );


	/* --- Column #2 --- */

	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title ( col, "Value" );
	gtk_tree_view_append_column ( GTK_TREE_VIEW ( info_tree ), col );
	renderer = gtk_cell_renderer_text_new();
	g_object_set ( G_OBJECT ( renderer ),"editable",TRUE,NULL );
	gtk_tree_view_column_pack_start ( col, renderer, TRUE );
	gtk_tree_view_column_add_attribute ( col, renderer, "text", 1 );


	gtk_tree_view_set_model ( GTK_TREE_VIEW ( info_tree ), GTK_TREE_MODEL ( cd_data[ track_no].track_info ) );
	gtk_widget_show ( info_tree );

	gtk_dialog_run ( GTK_DIALOG ( dialog ) );

	gtk_widget_destroy ( dialog );

}



void
view_popup_menu ( GtkWidget *track_list, GdkEventButton *event,TrackData *track )
{
	GtkWidget *menu, *menuitem;

	menu = gtk_menu_new();

	menuitem = gtk_menu_item_new_with_label ( "Get Track Information" );

	g_signal_connect ( menuitem, "activate",
	                   G_CALLBACK ( get_trak_info_popup_clicked_cb ), track );

	gtk_menu_shell_append ( GTK_MENU_SHELL ( menu ), menuitem );

	gtk_widget_show_all ( menu );

	gtk_menu_popup ( GTK_MENU ( menu ), NULL, NULL, NULL, NULL,
	                 ( event != NULL ) ? event->button : 0,
	                 gdk_event_get_time ( ( GdkEvent* ) event ) );
}


gboolean track_tree_button_press_cb ( GtkWidget *track_list, GdkEventButton *event, CdApp         *app )
{
	GtkTreeSelection *selection;
	GtkTreePath *path;
	TrackData *cd_data=main_app->cd_data;
	selection = gtk_tree_view_get_selection ( GTK_TREE_VIEW ( track_list ) );
	if ( gtk_tree_view_get_path_at_pos ( GTK_TREE_VIEW ( track_list ),
	                                     ( gint ) event->x,
	                                     ( gint ) event->y,
	                                     &path, NULL, NULL, NULL ) )
	{
		gtk_tree_selection_unselect_all ( selection );
		gtk_tree_selection_select_path ( selection, path );
	}
	else
		return FALSE;

	if ( event->type == GDK_2BUTTON_PRESS  &&  event->button == 1 )  // Double click => Read that track
	{

		read_track ( GTK_TREE_VIEW ( track_list ),
		             path,
		             GTK_TREE_VIEW ( app->file_tree ) );

	}
	else if ( event->type == GDK_BUTTON_PRESS  &&  event->button == 3 ) // Right click => get track information
	{
		read_track ( GTK_TREE_VIEW ( track_list ),
		             path,
		             GTK_TREE_VIEW ( app->file_tree ) );
		int track_no= ( *gtk_tree_path_get_indices ( path ) );
		view_popup_menu ( track_list, event, &cd_data[ track_no] );
		return TRUE;
	}
	gtk_tree_path_free ( path );
	return TRUE;
}

