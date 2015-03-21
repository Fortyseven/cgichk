#include <stdlib.h>
#include <stdio.h>

#include "cgichk.h"

char *resultfile = NULL;
/*---------------------------------------------------*/
result_t *open_config(char *fn)
{
	result_t	*r;

	if (resultfile)
		free(resultfile);

	r = (result_t *) malloc(sizeof(result_t));
	r->v = NULL;
	r->vnum = 0;

	// Was file explicitly requested?  Process that first, else...
	if (fn)
	{
		resultfile = (char *)strdup(fn);

		if ((r->F = fopen(fn, "rt")) == NULL)
		{
			perror(fn);
			exit(-1);
		}
		return r;
	}

	// ...try defaults, current dir...
	if (r->F = fopen("./cgichk.cfg", "rb"))
	{
		resultfile = (char *) strdup("./cgichk.cfg");
		return r;
	}

	// ...and then config dir
	if (r->F = fopen(CONFIG_PATH, "rb"))
	{
		resultfile = (char *) strdup(CONFIG_PATH);
		return r;
	}
	perror("cgichk.cfg");
	exit(-1);
}
/*---------------------------------------------------*/
void free_strings(result_t *r)
{
	int x;

	if (r->v == NULL)
		return;

	for (x = 0; x < r->vnum; x++)
	{
		if (r->v[x])
			free(r->v[x]);
	}
	free(r->v);
	r->v = NULL;
	r->vnum = 0;
}
/*---------------------------------------------------*/
// TODO: strip spaces, ignore empty
void add_string(result_t *r, char *str)
{
	//fprintf(stderr, "\"%s\"\n", str);
	if (strlen(str) == 0)
		return;

	r->v = (void *)realloc(r->v, sizeof(char *) * (r->vnum + 1));
	r->v[r->vnum] = (char *)strdup(str);
	r->vnum++;
}
/*---------------------------------------------------*/
void chop(char *str)
{
	char *p;

	p = (char *)strstr(str, "#"); // ignore comments
	if (p)
		*p = 0;

	p = (char *)strstr(str, "\n"); // strip eol
	if (p)
		*p = 0;
}

/*---------------------------------------------------*/
void read_config(result_t *r, char *section)
{
	char	temp[256] = "",
			head[256],
			foot[256];
	int		done = 0;

	if (!r)
  		return;    // Structure not initalized
	if (!r->F)
  		return; // File isn't open

	free_strings(r);

	bzero(head, 256);
	bzero(foot, 256);
	snprintf(head, 256, "<%s>", section);
	snprintf(foot, 256, "</%s>", section);

	rewind(r->F);
	// Find opening marker (i.e. <marker>)
	while (!done)
	{
		bzero(temp, 256);
		fgets(temp, 256, r->F);
		chop(temp);
		if (!strcmp(head, temp)) // Found opening tag?
		{
			while(1)
			{
				if (fgets(temp, 256, r->F) == NULL)
				{
					done = 1;
					break;
				}
				chop(temp);
				if (!strcmp(foot, temp))
				{
					done = 1;
					break;
				}
				else
                {
					add_string(r, temp);
				}
			}
		}
	}
}
/*---------------------------------------------------*/
void close_config(result_t *r)
{
	if (resultfile)
		free(resultfile);

	if (r->F)
		fclose(r->F);
	if (r->v)
		free_strings(r);

	free(r);
}
/*---------------------------------------------------*/
