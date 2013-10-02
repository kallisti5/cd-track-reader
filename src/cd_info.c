/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * cd_info.c
 * Copyright (C) harish 2009 <harish2704@gmail.com>
 *
 * callbacks.h is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cd_info.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
Reference :dvd-rw-tools dvd+rw-mediainfo.c
Thanks to dvd-rw-tools

*/


#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <scsi/sg.h>
#include <linux/cdrom.h>

#include "callbacks.h"


extern CdApp *main_app;



int get_no_track ( char * dev )
{
	int sg_fd, k,i;
	unsigned char  header[32];
	sg_io_hdr_t io_hdr;
	struct cdrom_generic_command cgc;
	TrackData *trak_data_array,*track;
	union
	{
		struct request_sense	s;
		unsigned char		u[18];
	} _sense;


	if ( ( sg_fd=open ( dev,O_RDWR|O_NONBLOCK ) ) < 0 &&
	        ( sg_fd=open ( dev,O_RDONLY|O_NONBLOCK ) ) < 0 )
	{

		perror ( "\nerror opening given file name" );
		show_error("error opening given file name");
		return 1;
	}

	if ( ( ioctl ( sg_fd, SG_GET_VERSION_NUM, &k ) < 0 ) || ( k < 30000 ) )
	{
		printf ( "\n%s is not an sg device, or old sg driver\n",dev );
		show_error("device is not an sg device, or old sg driver");
		return 1;
	}

	memset ( &io_hdr, 0, sizeof ( sg_io_hdr_t ) );
	io_hdr.interface_id = 'S';
	io_hdr.cmd_len = 10;
	io_hdr.mx_sb_len = sizeof ( _sense );
	io_hdr.dxfer_len =32;
	io_hdr.cmdp = cgc.cmd;
	io_hdr.sbp = _sense.u;
	io_hdr.timeout = 20000;
	io_hdr.dxferp		= header;
	io_hdr.dxfer_len		= 32;
	io_hdr.dxfer_direction	= SG_DXFER_FROM_DEV;
	io_hdr.flags	= SG_FLAG_LUN_INHIBIT|SG_FLAG_DIRECT_IO;



	cgc.quiet = 1;
	cgc.sense = &_sense.s;
	cgc.cmd[0] = GPCMD_READ_DISC_INFO;	// READ DISC INFORMATION
	cgc.cmd[6] = 0;
	cgc.cmd[7] = 0;
	cgc.cmd[8] = 32;
	cgc.cmd[9] = 0;

	if ( ioctl ( sg_fd,SG_IO,&io_hdr ) ) return -1;

//	printf ( "\nREAD DISC INFORMATION:" );

	main_app->no_of_tracks=header[11]<<8|header[6] ;
//	printf ( "\nNO OF TRACKS:  %d", no_of_tracks );

	trak_data_array =calloc ( main_app->no_of_tracks,sizeof ( TrackData ) );

	for ( i=1;i<=main_app->no_of_tracks;i++ )
	{
		track=&trak_data_array[i-1];

		cgc.cmd[0] = GPCMD_READ_TRACK_RZONE_INFO;	// READ TRACK INFORMATION
		cgc.cmd[1] = 1;
		cgc.cmd[2] = i>>24;
		cgc.cmd[3] = i>>16;
		cgc.cmd[4] = i>>8;
		cgc.cmd[5] = i;
		cgc.cmd[8] =32;
		cgc.cmd[9] = 0;

		if ( ioctl ( sg_fd,SG_IO,&io_hdr ) ) return -1;
		track->track_no=i-1;
		track->track_start= header[8]<<24|header[9]<<16|header[10]<<8|header[11];

		track->track_size=header[24]<<24|header[25]<<16|header[26]<<8|header[27];
		if ( header[7]&2 )
			track->track_end=header[28]<<24|header[29]<<16|header[30]<<8|header[31];

	}
	main_app->cd_data=trak_data_array;
	close ( sg_fd );

	return 0;
}
