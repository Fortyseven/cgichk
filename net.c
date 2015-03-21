#include "cgichk.h"

struct in_addr      addr;
struct sockaddr_in  _sin;
struct hostent      *he = NULL;
int					sock = 0;
proxy_t				proxy = {NULL, 8000};

/*----------------------------------------------------------------*/
int setup_socket(char *host, int port)
{
	unsigned long start;
	unsigned long end;
	unsigned long counter;

	if (!he) // have we looked this up before?
	{
		if ((he = gethostbyname(host)) == NULL)
		{
			fprintf(stderr, "ERROR: Couldn't connect to host -> ");
			herror("gethostbyname");
			exit(0);
		}
	}

	//start   = inet_addr(host);
	inet_aton(host, &start);		// rather than inet_addr, etc.
	counter = ntohl(start);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	bcopy(he->h_addr, (char *) &_sin.sin_addr, he->h_length);

	_sin.sin_family = AF_INET;
	_sin.sin_port   = htons(port);

	if (connect(sock, (struct sockaddr *) &_sin, sizeof(_sin)) != 0)
	{
		return -1;
	}
	return 0;
}
/*----------------------------------------------------------------*/
void read_head()
{
	char *buffer;

	buffer = malloc(BUFFER_SIZE);
	bzero(buffer, BUFFER_SIZE);

	if (parms.proxy)
	{
		char *tempstr;
		
		tempstr = (char *) malloc(1024);
		
		if (strstr(parms.URL, "http://"))
		{
			snprintf(tempstr, 1024, "HEAD %s/ HTTP/1.0\n\n", parms.URL);
		}
		else
		{
			snprintf(tempstr, 1024, "HEAD http://%s/ HTTP/1.0\n\n", parms.URL);
		}
		
		if (setup_socket(proxy.host, proxy.port))
		{
			fprintf(stderr, "ERROR: Problem connecting to proxy! -> ");
			perror("proxy connect");
			exit(-2);
		}

		send(sock, tempstr, strlen(tempstr), 0);
	}
	else
	{
		send(sock, "HEAD / HTTP/1.0\n\n", 17, 0);
	}
	recv(sock, buffer, BUFFER_SIZE, 0);
	PRINT("%s", buffer);
	close(sock);
	free(buffer);
}
/*----------------------------------------------------------------*/
int http_has(char *file, int result)
{
	int  err = 0;
	char query[256] = "";

	if (parms.delay)
		waitafew(parms.delay);

	if (parms.proxy)
	{
		if (strstr(parms.URL, "http://"))
		{
			snprintf(query, 256, "GET %s/%s HTTP/1.0\nUser-Agent: %s\nHost: %s\n\n", parms.URL, file, parms.agent, domain);
		}
		else
		{
			snprintf(query, 256, "GET http://%s/%s HTTP/1.0\nUser-Agent: %s\nHost: %s\n\n", parms.URL, file, parms.agent, domain);
		}
		//fprintf(stderr, "::%s\n", query);
		if (setup_socket(proxy.host, proxy.port))
		{
			fprintf(stderr, "ERROR: Problem connecting to proxy! -> ");
			perror("proxy connect");
			exit(-2);
		}
	}
	else
	{
		if (strstr(domain, "http://"))
			snprintf(query, 256, "GET %s%s/%s HTTP/1.0\nUser-Agent: %s\nHost: %s\n\n", domain, dir, file, parms.agent, domain);
		else
			snprintf(query, 256, "GET http://%s%s/%s HTTP/1.0\nUser-Agent: %s\nHost: %s\n\n", domain, dir, file, parms.agent, domain);
			
		if (setup_socket(domain, parms.port))
		{
			fprintf(stderr, "ERROR: Problem connecting to host! -> ");
			perror("connect");
			exit(-1);
		}
	}

	if (parms.sdebug)
			puts(query);

	bzero(httpbuff, parms.buffer_length);
	send(sock, query, strlen(query), 0);
	{
		int x, c;
		x = 0;
		while(recv(sock, &c, 1, 0) == 1)
		{
			httpbuff[x] = c;
			x++;
			if (x == parms.buffer_length)
			{
				break;
			}
		};
	}

	bzero(query, 256);
	memcpy(query, httpbuff+9, 3);  // Skip "HTTP/1.0 " and grab the 3 digits

	err = atoi(query);

	if (err == HTTP_OK || err == HTTP_MOVED)
    {
		if (parms.no_false_200_search == FALSE)
		{
			if (strnstr(httpbuff, "file not found") ||
				strnstr(httpbuff, "error 404") ||
				strnstr(httpbuff, "document has moved") ||
				strnstr(httpbuff, "page you have requested") ) // Wise?
			{
				err = HTTP_NOEXIST;
			}

			// If user spcified an additional string to search for, check for that too.
			if (parms.alt_fake_404_string)
			{
				if (strnstr(httpbuff, parms.alt_fake_404_string))
				{
					err = HTTP_NOEXIST;
				}
			}
		}
	}

	// Time for some debuggin...
	switch(parms.sdebug)
	{
		case DEBUG_RESULT:
			PRINT("[returned: %d]\n", err);
			break;
		case DEBUG_FOUND:
			if ((err != HTTP_NOEXIST) && (err != 0))
			{
				PRINT("\n\n ------------------------\n %s \n ------------------------\n", httpbuff);
				PRINT("Press any key to continue....\n");
				getchar();
			}
			break;
		case DEBUG_ALL:
			PRINT("\n\n ------------------------\n %s \n ------------------------\n", httpbuff);
			PRINT("Press any key to continue....\n");
			getchar();
			break;
		default:
			break;
	}

	if ((err != HTTP_NOEXIST) && (err != 0))
	{
		/* Is this right? */
		if (parms.ignore)
		{
			if (parms.ignore == err)
				return 0;
		}

		if (result)
		{
			PRINT("\007\r             \r\t%s found! (%d)\n", file, err);
		}
		
		switch(err)
		{
			case  HTTP_MOVED:
			case  HTTP_FORBID:
				if (parms.ignore403)
					return 0;			//Fall through
			case HTTP_GATEWAY:
			case HTTP_SERVERR:
			case    HTTP_AUTH: 			//return 0;
			default:
					return err;
		}
	}
	else
	{
		return 0;
	}
}

