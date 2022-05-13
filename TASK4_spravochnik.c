#include <stdio.h>

#define name_size 20
#define number_size 11

struct abonent 
{
	char name[name_size];
	char number[number_size];	//+X-XXX-XXX-XX-XX
} spravochnik[100];

void add_abonent(struct abonent *ptr_spravochnik)
{
	static char id = 0;

	printf("--------ADD NEW ABONENT--------\n");
	printf("Name (20 symbols max): ");
	for(int i = 0; i < name_size; ++i) {
		scanf("%[^\n]", &ptr_spravochnik->name);
	}
	fflush(stdin);
	printf("Telephone number (+X-XXX-XXX-XX-XX): ");
	for(int i = 0; i < number_size; ++i) {
		scanf("%[^\n]", &ptr_spravochnik->number);
	}

}

int main()
{
	struct abonent *ptr_spravochnik = spravochnik;
	add_abonent(ptr_spravochnik);

	for(int i = 0; i < name_size; ++i) {
		printf("%c", spravochnik[0].name[i]);
	}
	for(int i = 0; i < number_size; ++i) {
		printf("%c", spravochnik[0].number[i]);
	}
	return 0;
}
