/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) harish 2009 <harish2704@gmail.com>
 *
 * callbacks.h is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * main.c is distributed in the hope that it will be useful, but
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
#include <stdlib.h>
#include <gtk/gtk.h>

#include "callbacks.h"



CdApp *main_app;


int main ( int   argc,
           char *argv[] )
{
	CdApp app;
	main_app=&app;
	GtkWidget *scrolled_win_t, *scrolled_win;
	GtkWidget *box2,*box1 ,*bigbox;
	GtkCellRenderer     *renderer;
	GtkWidget *menubar ,*main_menu_item,*menu;


	gtk_init ( &argc, &argv );

//Initialisation
	app.window= gtk_window_new ( GTK_WINDOW_TOPLEVEL );
	bigbox=gtk_vbox_new ( FALSE, 10 );
	box1 = gtk_hbox_new ( FALSE, 10 );
	scrolled_win = gtk_scrolled_window_new ( NULL, NULL );
	app.track_tree= gtk_tree_view_new ();
	renderer = gtk_cell_renderer_text_new ();
	app.file_tree= gtk_tree_view_new ();
	box2 = gtk_hbox_new ( FALSE, 10 );
	renderer = gtk_cell_renderer_text_new ();
	scrolled_win_t = gtk_scrolled_window_new ( NULL, NULL );
	menubar=gtk_menu_bar_new();

	char deflt_dev[]="/dev/scd0";
	app.dev =deflt_dev;
//packing
	gtk_container_add ( GTK_CONTAINER ( app.window ), bigbox );
	gtk_box_pack_start ( GTK_BOX ( bigbox ), menubar, FALSE, FALSE, 5 );
	gtk_box_pack_start ( GTK_BOX ( bigbox ), box1, TRUE, TRUE, 10 );
	gtk_box_pack_start ( GTK_BOX ( bigbox ), box2, FALSE, FALSE, 10 );
	gtk_box_pack_start ( GTK_BOX ( box1 ), scrolled_win_t,  TRUE, TRUE, 10 );
	gtk_box_pack_start ( GTK_BOX ( box1 ), scrolled_win,  TRUE, TRUE, 10 );
	gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW ( scrolled_win ),app.file_tree );
	gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW ( scrolled_win_t ),app.track_tree );

//window
	gtk_container_set_border_width ( GTK_CONTAINER ( app.window ), 5 );
	g_signal_connect ( G_OBJECT ( app.window ), "delete_event",
	                   G_CALLBACK ( gtk_main_quit ), NULL );
	gtk_window_set_icon_name ( GTK_WINDOW ( app.window ),"cd-track-reader" );
//scrolled winow for file tree
	gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrolled_win ),
	                                 GTK_POLICY_AUTOMATIC,
	                                 GTK_POLICY_AUTOMATIC );
	gtk_widget_set_size_request ( scrolled_win, 300, 100 );

//scrolled winow for track list
	gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrolled_win_t ),
	                                 GTK_POLICY_AUTOMATIC,
	                                 GTK_POLICY_AUTOMATIC );
	gtk_widget_set_size_request ( scrolled_win_t, 300, 100 );
//track list
	gtk_tree_view_insert_column_with_attributes ( GTK_TREE_VIEW ( app.track_tree ),
	        -1,
	        "Tracks",
	        renderer,
	        "text", 0,
	        NULL );
	g_signal_connect ( app.track_tree, "button-press-event",G_CALLBACK ( track_tree_button_press_cb ), &app );

//file tree
	gtk_tree_view_insert_column_with_attributes ( GTK_TREE_VIEW ( app.file_tree ),
	        -1,
	        "Files",
	        renderer,
	        "text", 0,
	        NULL );
	g_signal_connect ( G_OBJECT ( app.file_tree ), "row-activated",
	                   G_CALLBACK ( on_file_clicked_cb ), &app );


//button
	app.button = gtk_button_new_with_label ( "Scan Tracks" );
	g_signal_connect ( G_OBJECT ( app.button ), "clicked",
	                   G_CALLBACK ( on_scan_button_clicked_cb ), &app );
	gtk_box_pack_start ( GTK_BOX ( box2 ), app.button,  TRUE, TRUE, 10 );
	gtk_widget_show ( app.button );






	menu = gtk_menu_new ();
	main_menu_item=gtk_menu_item_new_with_label ( "File" );
	app.opendev_menu_item=gtk_menu_item_new_with_label ( "Open Device" );
	gtk_menu_shell_append ( GTK_MENU_SHELL ( menu ), app.opendev_menu_item );
	g_signal_connect ( app.opendev_menu_item, "activate",
	                   G_CALLBACK ( open_dev_callbk ),
	                   &app );
	gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( main_menu_item ), menu );
	gtk_menu_shell_append ( GTK_MENU_SHELL ( menubar ), main_menu_item );

//Main Menu
	menu = gtk_menu_new ();
	main_menu_item=gtk_menu_item_new_with_label ( "Help" );
//Submenu Help
	app.help_menu_item=gtk_menu_item_new_with_label ( "Help" );
	gtk_menu_shell_append ( GTK_MENU_SHELL ( menu ), app.help_menu_item );
	g_signal_connect ( app.help_menu_item, "activate",
	                   G_CALLBACK ( help_cb ),
	                   &app );
	gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( main_menu_item ), menu );
//Submenu About
	app.about_menu_item=gtk_menu_item_new_with_label ( "About" );
	gtk_menu_shell_append ( GTK_MENU_SHELL ( menu ), app.about_menu_item );
	g_signal_connect ( app.about_menu_item, "activate",
	                   G_CALLBACK ( about_cb ),
	                   &app );
	gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( main_menu_item ), menu );

	gtk_menu_shell_append ( GTK_MENU_SHELL ( menubar ), main_menu_item );


	gtk_widget_show_all ( app.window );


	gtk_main ();

	return 0;
}
