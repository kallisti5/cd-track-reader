/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * callbacks.h
 * Copyright (C) harish 2009 <harish2704@gmail.com>
 *
 * callbacks.h is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * callbacks.h is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>

typedef struct
{
	int track_no;
	long track_start;
	long track_end;
	long track_size;
	GtkTreeStore *file_tree;
	GtkListStore *track_info;

}TrackData;

typedef struct
{
	int no_of_tracks;
	char *dev;
	TrackData *cd_data;
	GtkWidget *window;
	GtkWidget *opendev_menu_item;
	GtkWidget *help_menu_item;
	GtkWidget *about_menu_item;
// 	GtkWidget *
	GtkWidget *track_tree;
	GtkWidget *button;
	GtkWidget *file_tree;
	GtkCellRenderer     *renderer;
	GtkWidget *menubar ,*main_menu_item,*submenu_item,*menu;
}CdApp;




void on_file_clicked_cb ( GtkTreeView       *tree_view,
                          GtkTreePath       *path,
                          GtkTreeViewColumn *column,
                          CdApp         *app );

void  on_scan_button_clicked_cb ( GtkButton *button,
                                  CdApp         *app )  ;

void  about_cb ( GtkMenuItem *menuitem,
                 CdApp         *app );

void help_cb ( GtkMenuItem *menuitem,
               CdApp         *app );

void open_dev_callbk ( GtkMenuItem *menuitem,
                       CdApp         *app );

gboolean track_tree_button_press_cb ( GtkWidget *track_list,
                                      GdkEventButton *event,
                                      CdApp         *app );


void show_error ( char *msg );

int get_no_track ( char * dev );

int iso_extract ( char *dev, char *file_name ,char* dest_path, long track );

GtkTreeStore * read_cd ( char *dev,int block );




