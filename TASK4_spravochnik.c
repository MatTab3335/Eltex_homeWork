#include <stdio.h>

#define name_size 20
#define number_size 11
#define spravochnik_size 100

struct abonent 
{
	char name[name_size];
	char number[number_size];	//+X-XXX-XXX-XX-XX
} spravochnik[spravochnik_size];

void add_abonent(struct abonent *spravochnik)
{
	static char id = 0;
	struct abonent *ptr = spravochnik;
	
  ptr += id;
  
	printf("--------ADD NEW ABONENT--------\n");
	printf("Name (20 symbols max): ");
	fgets(ptr->name, name_size, stdin);
	
	printf("Telephone number (+X-XXX-XXX-XX-XX): ");
	fgets(ptr->number, number_size, stdin);
  ++id;
  if (id >= 99) {
    printf("Spravochnik is full, next record will overwrite record #1");
    id = 0;
    ptr = spravochnik;
  }
}
void print_spravochnik(int amount) 
{
	if (amount > spravochnik_size) {
		printf("We can't print so many abonents, try less number");
		return;
	}
	printf("--------LIST OF ABONENTS--------\n");
  for (int y = 0; y < amount; ++y) {
    printf("\nAbonent #%i\nName: ", y + 1);
	  for(int i = 0; i < name_size; ++i) {
		  printf("%c", spravochnik[y].name[i]);
	  }
	  printf("Number: ");
	  for(int i = 0; i < number_size; ++i) {
		printf("%c", spravochnik[y].number[i]);
	  }
	}
}

char compare_names(char *name1, char *name2)
{
	for (int i = 0; i < name_size; ++i) {
			if (name1[i] != name2[i])
				return 0;
	}
	return 1;
}

void search_abonent_name(struct abonent *spravochnik)
{
	char name[name_size];
	struct abonent *ptr = spravochnik;

	printf("What name do you want to find? Type here: ");
	fgets(name, name_size, stdin);

	for (int i = 0; i < spravochnik_size; ++i) {
		if (compare_names(ptr->name, name) == 0)
			++ptr;
		else printf("Your abonent is Abonent#%i \n", i + 1);
	}
}
int main()
{
	struct abonent *ptr_spravochnik = spravochnik;
	int amount = 5;
	char name[name_size];

  for(int i = 0; i < amount; ++i) {
    add_abonent(spravochnik); 
  }
  print_spravochnik(amount);
  search_abonent_name(spravochnik);

	return 0;
}
