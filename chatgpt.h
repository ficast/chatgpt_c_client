
#ifndef CHATGPT_H
# define CHATGPT_H

char		*chatgpt_query(const char *api_key, const char *prompt);

struct		memory
{
	char	*response;
	size_t	size;
};

#endif