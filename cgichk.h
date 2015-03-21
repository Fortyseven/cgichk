/*
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to:
                                   The Free Software Foundation, Inc.
                                          59 Temple Place - Suite 330
                                          Boston, MA  02111-1307, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdarg.h>
#include <getopt.h>
#include <time.h>
#include <unistd.h>

#ifdef DEBUG
	#define MEMWATCH
	#define MW_STDIO
	#include "memwatch.h"
#endif

#define VERSION					"2.60"

#define CONFIGFILENAME			"cgichk.cfg"

#ifdef _WIN32_
	#define CONFIG_PATH			"c:/windows/"CONFIGFILENAME
#else
	#define CONFIG_PATH			"/etc/"CONFIGFILENAME
#endif

#define PROXY_ENV				"HTTP_PROXY"

#define DEFAULT_AGENT			"cgichk "VERSION

#define TRUE					1
#define FALSE					0

#define HTTP_PORT				80
#define BUFFER_SIZE				1024

#define HTTP_OK					200
#define HTTP_MOVED				302
#define HTTP_NOEXIST			404
#define HTTP_FORBID				403
#define HTTP_AUTH				401
#define HTTP_SERVERR			500
#define HTTP_GATEWAY			502

#define DEBUG_NONE				0
#define DEBUG_RESULT			1
#define DEBUG_FOUND				2
#define DEBUG_ALL				3

#define BEEP()						{if(!parms.quiet) printf("\007");}

#define DEFAULT_HTTP_BUFFER_LENGTH		1024	// Yup, 1k.  Expand this using
												// -b for pages with bloated pseudo-404 pages.

typedef struct {
   int			delay;
   int			whois;
   int			proxy;
   int			quiet;     //Stops beepin
   char			*config;
   char			*URL;
   char			*agent;
   char			*alt_fake_404_string;
   int			check_interest;
   int			check_frontpage;
   int			noua;
   unsigned int port;
   int			debugm;
   int			sdebug;
   int			ignore403;
   int			only_head;
   int			no_false_200_search;
   int			buffer_length;
   int			ignore;
   FILE			*OUTPUT;
} parms_t;

extern parms_t parms;

typedef struct {
	char	*host;
	int		port;
} proxy_t;

extern proxy_t	proxy;

typedef struct {
	FILE	*F;
	char	**v;
	int		vnum;
} result_t;

extern FILE *OUTPUT;       //For output redirection, defaults to stdout
extern char quiet;         //Stops beepin

extern int  sock;
extern int  numin;
extern char *httpbuff;

extern char *whois_host;

extern char domain[1024];
extern char dir[1024];

extern struct in_addr      addr;
extern struct sockaddr_in  _sin;
extern struct hostent      *he;

