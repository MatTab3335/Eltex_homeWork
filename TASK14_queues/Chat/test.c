#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int del_el_double_ar (char array[][20], char *element, int size)
{
    int i = 0;
    for (i = 0; i < size; i++) {
        if (!strcmp (array[i], element)) {
            if (i == size -1) memset (array[i], 0, strlen (array[i]));
            else {
                for (int y = i; y < size - 1; y++)
                    strcpy (array[y], array[y + 1]);
                memset (array[size - 1], 0, strlen (array[size - 1]));
            }
            return i;
        }
    }
}
void del_el_mqd_t (int *array, int idx, int size)
{
    if (idx == size - 1) array[idx] = 0;
    else {
        for (int i = idx; i < size - 1; i++)
            array[i] = array[i + 1];
        array[size - 1] = 0;
    }
    return;
}

int main()
{
	char array[4][20] = {"ANton", "Lexa", "Sara", "Iza"};
	int array1[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};


	for (int i = 0; i < 4; i++)
		printf("%s\n", array[i]);
	for (int i = 0; i < 10; i++)
		printf("%i\n", array1[i]);

	del_el_double_ar (array, "Lexa", 4);
	del_el_mqd_t (array1, 4, 10);

	for (int i = 0; i < 4; i++)
		printf("%s\n", array[i]);
	for (int i = 0; i < 10; i++)
		printf("%i\n", array1[i]);
	exit (0);
}