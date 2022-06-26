#include <stdio.h>
#include <string.h>

int main()
{
	char s[][20] = {"home/mat/downloads", "home/mat/cucaracha"};
	char temp[strlen(s[0])];
	puts(s[0]);
	int i = strlen(s[0]);
	printf("%i\n", i);
	int remove = 0;
	while (s[1][i] != '/') {
		remove++;
		i--;
	}
	printf("%i\n", remove);
	strncpy(temp, s[0], strlen(s[0]) - remove);
	printf("%s",temp);
}