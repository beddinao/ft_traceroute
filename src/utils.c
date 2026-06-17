#include <ft_traceroute.h>

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	unsigned char		*sdst;
	const unsigned char	*ssrc;
	size_t				i;

	sdst = (unsigned char *)dst;
	ssrc = (unsigned char *)src;
	i = 0;
	if (!sdst && !ssrc)
		return (NULL);
	while (i < n)
	{
		sdst[i] = ssrc[i];
		i++;
	}
	return (dst);
}

void	*ft_memset(void	*b, int c, size_t len)
{
	size_t			i;
	unsigned char	*ba;
	unsigned char	sc;

	i = 0;
	ba = (unsigned char *)b;
	sc = (unsigned char)c;
	while (i < len)
	{
		ba[i] = sc;
		i++;
	}
	return (b);
}

int	ft_strcmp(const void *s1, const void *s2)
{
	const unsigned char	*ss1 = (unsigned char*)s1;
	const unsigned char	*ss2 = (unsigned char*)s2;
	size_t i = 0;

	for (; ss1[i] != '\0' && ss2[i] != '\0' && ss1[i] == ss2[i]; i++);
	return ss1[i] - ss2[i];
}

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i] != '\0')
		i++;
	return (i);
}


