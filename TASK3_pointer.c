#include <stdio.h>

int main()
{
	int number = 0x7F557F7F; 
	char *ptr = &number;
	/*------print each byte--------*/
	for (int i = 0; i < 4; ++i) {
		printf("%X ", *ptr);
		ptr++;
	}
	/*------replace 3d byte------*/
	ptr = &number;
	ptr += 2;
	*ptr = 0x7F;
	ptr = &number;
	printf("\n");
	for (int i = 0; i < 4; ++i) {
		printf("%X ", *ptr);
		ptr++;
	}
	return 0;
}
