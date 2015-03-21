/*
  cgichk - CGI vulnerability and naughty curiousity scanner
  ---------------------------------------------------------

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
					
						(So there!  Phht!)
*/

#include "cgichk.h"

/*----------------------------------------------------------------*/
parms_t		parms = {
	0, 0, 0, 0, NULL, NULL, NULL, NULL, 0, 0, 0,
	HTTP_PORT, 0, 0, 0, 0, 0, DEFAULT_HTTP_BUFFER_LENGTH, 0, NULL
};

int			numin;
char		*httpbuff = NULL;

char		domain[1024] = "",
			dir[1024] = "";

result_t	*r = NULL;

/*----------------------------------------------------------------*/
void check_dir_only()
{
	int		count = 0;
	int		result = 0;
	char	*temp = 0;
	int		x = 0;

	read_config(r, "dir_only");

	temp = (char *)malloc(256);

	for (x = 0; x < r->vnum; x++)
	{
		int res;

		snprintf(temp, 256, "%s/", r->v[count]);
		CPRINT("                                                         \r");
		CPRINT("  Looking for /%s\r", r->v[count]);

		if (result = http_has(temp,0))
		{
			CPRINT("                                                       \r");
			PRINT("    Found /%s (%d)                        \n", r->v[count], result);
			BEEP();
		}
		count++;
	}
	free(temp);
	PRINT("                                               \n");
}
/*----------------------------------------------------------------*/
void check_hole(char *hole)
{
	int result = 0;

	CPRINT("                                                       \r");
	CPRINT("  Looking for %s\r", hole);

	if (result = http_has(hole,0))
	{
		CPRINT("                                                   \r");
		PRINT("  Found %s (%d)\n",hole, result);
		BEEP();
	}
  /*close(sock);*/
}
/*----------------------------------------------------------------*/
void check_specific()
{
	int count = 0;
	int x;

	read_config(r, "fixed_files");

	for(x = 0; x < r->vnum; x++)
		check_hole(r->v[x]);

	if (parms.check_frontpage)
	{
		read_config(r, "frontpage");
		for (x = 0; x < r->vnum; x++)
		{
			check_hole(r->v[x]);
		}
	}

	PRINT("                                                            \n"); // clean up
}
/*----------------------------------------------------------------*/
void check_interest()
{
	char		temp[256];
	int			result;
	result_t	*f;
	int			x=0, y=0;

	read_config(r, "interest_dir");

	// Duplicate config handle for our own sadistic purposes
	f       = (result_t*) malloc(sizeof(result_t));
	f->F    = r->F;
	f->vnum = 0;
	f->v    = NULL;

	read_config(f, "interest_file");

	for(y = 0; y < r->vnum; y++)
	{
		bzero(temp, 256); snprintf(temp, 256, "/%s/", r->v[y]);

		CPRINT("  Looking for /%s                          \r", r->v[y]);

		if (result = http_has(temp,0))        // See if the directory exists, so no wasted time
		{
			PRINT("    Found %s (%d)                  \n", temp, result);
			BEEP();
			if (result == HTTP_OK && parms.check_interest)
			{
				for (x = 0; x < f->vnum; x++) // Search for each file in that dir
				{
					bzero(temp, 256);
					snprintf(temp, 256, "/%s/%s", r->v[y], f->v[x]);
					CPRINT("                                         \r");
					CPRINT("    Looking for /%s                       \r", temp);

					if (result = http_has(temp,0))
					{
						PRINT("     ---> /%s (%d)\n", temp, result);
						BEEP();
					}
					x++;
				}
				PRINT("                                \n"); // clean up
			}
		}
		x = 0;
		y++;
	}
	PRINT("                                          \n"); // clean up
	free_strings(f);
	free(f);
}
/*----------------------------------------------------------------*/
void show_version()
{
	printf("cgichk "VERSION" - "__DATE__"\n\n");
	printf("Based on 'CGI Checker 1.35' by su1d sh3ll\n");
	printf("Cooked and rewritten by Fortyseven.\n\n");
	exit(0);
}
/*----------------------------------------------------------------*/
void show_help(char *name)
{
	printf("cgichk "VERSION"\n");
	printf("usage : %s host [-p n] [-D n] [-P s] [-A s] [-b n] [-g n] [-B s] [-qwhdHVifWC] [-o output]\n\n", name);
		puts("        -p n pauses n seconds between HTTP requests");
		puts("        -q   silences alert beeping");
    	puts("        -w   retrieves whois (com/net/org) information on target");
    	puts("        -o   dumps data to file (requires filename argument)");
    	puts("        -h   help");
    	puts("        -d   debugging info (shows results for !404 items)");
    	puts("        -D n super debugging level (shows whatever extra I want :))");
    	puts("        -V   version number and compile date");
    	puts("        -i   turns ON descending into 'interest' directories for");
    	puts("             exploitable files");
    	puts("        -f   turns ON checking for FrontPage explotables");
    	puts("        -W   specify alternate whois server");
    	puts("        -C   specify alternate config file");
		puts("        -3   ignore 403's (forbidden) and 302's (moved)");
		puts("        -P s specify proxy host, or taken from $HTTP_PROXY");
	  printf("        -A s alternate agent string, [%s]\n", DEFAULT_AGENT);
	  printf("        -b n specify how much data to pull from page [%d bytes]\n", DEFAULT_HTTP_BUFFER_LENGTH);
	    puts("        -g n treat a specific result code as a 404.");
		puts("        -B s give cgichk an additional string to use to find pseudo-404's.");
		puts("        -H   only retrieve HTTP header, then quit.");
		puts("        -I   do NOT search inside 200 pages for 404-type strings.");
		puts("");
	exit(0);
}
/*----------------------------------------------------------------*/
void get_commandline(int argc, char *argv[])
{
	int	c = 0;

	while ((c = getopt(argc, argv, "3A:P:C:W:D:iIfb:dhVg:qwp:o:B:H"))>=0)
	{
		switch(c)
		{
			case 'C':
				parms.config = (char*) strdup(optarg);
				break;
			case 'W':
				whois_host = (char*) strdup(optarg);
				break;
			case 'f':
				parms.check_frontpage = TRUE;
				break;
			case 'i':
				parms.check_interest = TRUE;
				break;
			case 'd':
				parms.debugm = TRUE;
				break;
			case 'D':
				parms.sdebug = atoi(optarg);
				break;
			case 'h':
				show_help(argv[0]);
				break;
			case 'V':
				show_version();
				break;
			case 'q':
				parms.quiet = TRUE;
				break;
			case 'o':
				parms.OUTPUT = fopen(optarg, "wt");
				if (parms.OUTPUT == NULL)
				{
					puts("Error opening output file.");
					exit(-1);
				}
				break;
			case '3':
				parms.ignore403 = TRUE;
				break;
			case 'p':
				parms.delay = atoi(optarg);
				break;
			case 'w':
				parms.whois = TRUE;
				break;
			case 'P': 				// Proxy
				parms.proxy = TRUE;
				proxy.host = (char *)malloc(strlen(optarg));
				breakup_url(optarg, proxy.host, strlen(optarg), &proxy.port, 8000, NULL, 0);
				break;
			case 'A':				// Agent
				parms.agent = (char *) strdup(optarg);
				break;
			case 'b': 				// http buffer length
				parms.buffer_length = atoi(optarg);
				break;
			case 'g':
				parms.ignore		= atoi(optarg);
				break;
			case 'B':
				parms.alt_fake_404_string = (char *) strdup(optarg);
				break;
			case 'H':
				parms.only_head = TRUE;
				break;
			case 'I':
				parms.no_false_200_search = TRUE;
				break;
			default:
				/*unf*/
				break;
		}
	}

	//if (parms.config == NULL) parms.config = CONFIG_PATH;

	if (argv[optind] == NULL)
	{
		show_help(argv[0]);
	}

	parms.URL = (char*) strdup(argv[optind]);

	breakup_url(parms.URL, domain, 1024, &parms.port, 80, dir, 1024);
}
/*----------------------------------------------------------------*/
void mainloop()
{
	PRINT("------------------------------------------------------------\n");
	PRINT("URL:\n\n");
 	PRINT(parms.URL);
	PRINT("\n\nHEADER:\n\n");
	read_head();
	PRINT("------------------------------------------------------------\n");
	
	if (parms.only_head)
		return;
		
	PRINT("DIRECTORIES:\n\n");
	check_dir_only();
	PRINT("INTEREST:\n\n");
	check_interest();
	PRINT("SPECIFIC:\n\n");
	check_specific();
	PRINT("------------------------------------------------------------\n");
}
/*----------------------------------------------------------------*/
/*----------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	parms.OUTPUT = stdout;

	get_commandline(argc, argv);

	httpbuff = (char *) malloc(parms.buffer_length);
	if (httpbuff == NULL)
	{
		printf("Error allocating %d bytes for http buffer.\n", parms.buffer_length);
		exit(-1);
	}
	
	r = (result_t *)open_config(parms.config);

	if (!parms.agent)
	{
		parms.agent = DEFAULT_AGENT;
	}

	if (parms.whois)
	{
		PerformWhois(domain);
	}

	if (!proxy.host)			//look for proxy info in environment
	{
		char *env = NULL;
		
		if (env = getenv(PROXY_ENV))
		{
			proxy.host = (char *) malloc(strlen(env));
			breakup_url(env, proxy.host, strlen(env), &proxy.port, 8000, NULL, 0);
			parms.proxy = TRUE;
		}
	}

	setup_socket(domain, parms.port);
	
	mainloop();

	close_config(r);

	if (parms.OUTPUT != stdout)
		fclose(parms.OUTPUT);

	if (parms.URL)
		free(parms.URL);

	return 0;
}
