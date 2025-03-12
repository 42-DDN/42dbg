#include <utils.h>

char	*strjoin(char *s1, char *s2)
{
	if (!s1 || !s2)
		return (0);
	size_t	s1_len = strlen(s1);
	size_t	s2_len = strlen(s2);
	char *res = malloc(s1_len + s2_len + 1);
	if (!res)
		return (0);
	*res = 0;
	strcat(res, s1);
	strcat(res, s2);
	res[s1_len + s2_len] = 0;
	return (res);
}
