#include "cgichk.h"

/*----------------------------------------------------------------*/
// I'm going to keep the old code here...just in case.
// You know: for kids.
void waitafew(unsigned int del)
{
 /*
  clock_t  prevtime = clock();

  while(1)
    {
      if ((clock() - prevtime) >= (CLOCKS_PER_SEC * del)) return;
    }
*/
	sleep(del);
}
/*----------------------------------------------------------------*/
void PRINT(char *arg, ...)
{
	va_list	v;
	char 	*buff;

	buff = (char*)malloc(8192);

	va_start(v, arg);
	vsprintf(buff, arg, v);
	va_end(v);

	fprintf(parms.OUTPUT, "%s", buff);
	fflush(parms.OUTPUT);
	free(buff);
}
/*----------------------------------------------------------------*/
void CPRINT(char *arg, ...)
{
	va_list v;
	char 	*buff;

	if (parms.OUTPUT != stdout)
		return;

	buff = (char*)malloc(8192);

	va_start(v, arg);
	vsprintf(buff, arg, v);
	va_end(v);

	printf("%s", buff);
	fflush(parms.OUTPUT);
	free(buff);
}

/*----------------------------------------------------------------*/

// _url  = string containing full/partial URL
// _host = string to put host info in
// hl    = max length of _host string
// _port = integer to recieve port #
// defport = default port #
// _path = string containing file path info
// pl    = length of _path string

void breakup_url(	char *_url,
					char *_host,	int hl,
					int  *_port, 	int defport,
					char *_path, 	int pl)
{
	char 	*u, *t;
	int		hasport = 0,
			haspath = 0;
	char	*end;

	// Check for null's
	//if (_url == NULL || _host == NULL || _port == NULL  || _path == NULL)
	//	return;

	memset(_host, 0, hl);
	memset(_path, 0, pl);

	u 		= _url;
	end 	= _url + strlen(_url);

	*_port = defport;

	if ( t = (char *)strstr(u, "://") )				// skip past http bit
		u = t + 3;

	// parse hostname and port

	if (t = (char *)index(u, ':'))						// does host have a port #?
		hasport = 1;
	
	if (t = (char *)index(u, '/'))						// does host have path?
		haspath = 1;

	if (hasport)
		t = (char *)index(u, ':');
	else
		t = (char *)index(u, '/');

	if (t == NULL) // found neither :port nor /path
	{
		if (_host)
			strcpy(_host, u);
		return;
	}

	if (_host)
		strncpy(_host, u, (t-u));

	u = t;

	// have hostname at this point

	if (hasport)
	{
		if ((t = (char *)index(u, '/')) == NULL)	// check for path seperator
			t = end;								// if none, grab to end of url
		else
			*t = 0;
			
		u++;
		
		if (_port)
			*_port = atoi(u);
			
		u = t;
		// FIXME: Convert ascii to int and store in 'port'
	}

	if (u == end)
		return;						// had just host/port, shows over

	if (*t == 0)
		*t = '/';					// *snicker*
									// have port at this point
	if (_path)
		strcpy(_path, u);
}

/*----------------------------------------------------------------
// FIXME: This needs to be rewritten, or at least have the port number
//        be found in the right place.

void OLD_breakup_url(char *url, char *base, char *dir, unsigned int *port)
{
  char *a, *b, *p;

  if ((!url) || (!base) || (!dir) || (!port)) return;

  a = (char*)strstr(url, "://");
  if (a)
    {
      a+=3;                  // skip http://
    }
  else
    {
      a = url;
    }
                                 //FIXME: ports normally come after the
                                 //       domain name in URL's, not after dir
  p = (char*)strchr(a, ':');     // look for port number
  if (p)
    {
      *port = atoi(p+1);
    }
  else
    {
      *port = 80;
    }

  b = (char*)strchr(a, '/');     // look for first directory slash, if any
  if (b)                         // has it
    {
      memcpy(base, a, b-a);
      a = b;
      if (p)
        {
          memcpy(dir, a, p - a);
        }
      else
        {
          memcpy(dir, a, strlen(a));
        }
    }
  else                // doesn't have it
    {
      if (p)
        {
          memcpy(base, a, p - a);
        }
      else
        {
          memcpy(base, a, strlen(a));
        }
    }
}
*/
/*----------------------------------------------------------------*/
char *lowerstr(char *str)
{
	int	x;

	for (x = 0; x < strlen(str); x++)
	{
		str[x] = tolower(str[x]);
	}
	return str;
}
/*----------------------------------------------------------------*/
char *strnstr(const char *HAYSTACK, const char *NEEDLE)
{
  char	*h, *n, *res;

  h = (char *)strdup(HAYSTACK);
  n = (char *)strdup(NEEDLE);
  lowerstr(h);
  lowerstr(n);

  res = (char*)strstr(h, n);
  free(h);
  free(n);
  return res;
}
