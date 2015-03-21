#include "cgichk.h"

#define WHOIS_SERVER "whois.networksolutions.com"
#define WHOIS_PORT   43

char *whois_host = NULL;

/*----------------------------------------------------------------*/
char *strip_www(char *domain)
{
	static char newdomain[256];
	int			x, c = 0;

	for (x = strlen(domain); x >= 0; x--)
	{
		if (domain[x] == '.')
			c++;
			
		if (c == 2)
		{
			x++;
			return &(domain[x]);
		}
	}
	return domain;
}
/*----------------------------------------------------------------*/
void PerformWhois(char *domain)
{
	struct sockaddr_in 	addr;
	struct hostent 		*he;
	char   				*msg, *dom;
	int    				s;

	// Revert to default if no host was specified
	if (!whois_host)
		whois_host = WHOIS_SERVER;

	msg = (char*)malloc(8192); // excessive, but it's just 8k
	bzero(msg, 8192);

	s = socket(AF_INET, SOCK_STREAM, 0);

	if ((he = gethostbyname(whois_host)) == NULL)
	{
		perror("whois lookup");
		return;
	}

	addr.sin_family = AF_INET;          // host byte order
	addr.sin_port = htons(WHOIS_PORT);  // short, network byte order
	memcpy(&addr.sin_addr, he->h_addr, he->h_length);

	bzero(&(addr.sin_zero), 8);         // zero the rest of the struct

	s = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(s, (struct sockaddr *)&addr, sizeof(struct sockaddr)) != 0)
		perror("whois connect");

	snprintf(msg, 8192, "%s\n", strip_www(domain));

	if (send(s, msg, strlen(msg),0) == -1)
		perror("whois send");

	recv(s, msg, 8192, 0); bzero(msg, 8192); // ignore excess whois verbage
	recv(s, msg, 8192, 0);

	PRINT("------------------------------------------------------------\n");
	PRINT("WHOIS:\n\n");
	PRINT(msg);

	if (parms.OUTPUT == stdout)
		getchar();

	close(s);
	free(msg);
}
