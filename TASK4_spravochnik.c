#include <stdio.h>

#define name_size 20
#define number_size 11
#define spravochnik_size 10

struct abonent 
{
	char name[name_size];
	char number[number_size];	//+X-XXX-XXX-XX-XX
} spravochnik[spravochnik_size] = {};

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
void print_abonent(const struct abonent *spravochnik, const int number)
{
	if (number > spravochnik_size) {
		printf("We can't print such abonent, try less number");
		return;
	}
  printf("\nAbonent #%i\nName: ", number);
	for(int i = 0; i < name_size; ++i) 
		printf("%c", spravochnik[number - 1].name[i]);
	printf("Number: ");
	for(int i = 0; i < number_size; ++i) 
		printf("%c", spravochnik[number - 1].number[i]);
}
void print_spravochnik(const struct abonent *spravochnik, int amount) 
{
	if (amount > spravochnik_size) {
		printf("We can't print so many abonents, try less number");
		return;
	}
	printf("--------LIST OF ABONENTS--------\n");
  for (int y = 0; y < amount; ++y) {
    print_abonent(spravochnik, y + 1);
	}
}
char compare_names(const char *name1, const char *name2)
{
	for (int i = 0; i < name_size; ++i) {
			if (name1[i] != name2[i])
				return 0;
	}
	return 1;
}
char compare_numbers(const char *number1, const char *number2)
{
	for (int i = 0; i < number_size; ++i) {
			if (number1[i] != number2[i])
				return 0;
	}
	return 1;
}

void search_abonent_name(const struct abonent *spravochnik)
{
	printf("--------SEARCHING ABONENT--------\n");
	char name_to_find[name_size] = {};

	printf("What name do you want to find? Type here: ");
	fgets(name_to_find, name_size, stdin);

	for (int i = 0; i < spravochnik_size; ++i) {
		if (compare_names(spravochnik[i].name, name_to_find) == 1) {
			printf("Your abonent is Abonent#%i \n", i + 1);
			print_abonent(spravochnik, i + 1);
			return;
		}
	}
	printf("We can't find your abonent, please check the name\n");
}
void search_abonent_number(const struct abonent *spravochnik)
{
	printf("--------SEARCHING ABONENT--------\n");
	char number_to_find[number_size] = {};

	printf("What number do you want to find (+X-XXX-XXX-XX-XX)? Type here: ");
	fgets(number_to_find, number_size, stdin);

	for (int i = 0; i < spravochnik_size; ++i) {
		if (compare_numbers(spravochnik[i].number, number_to_find) == 1) {
			printf("Your abonent is Abonent#%i \n", i + 1);
			print_abonent(spravochnik, i + 1);
			return;
		}
	}
	printf("We can't find your abonent, please check the number\n");
}
void empty_stdin() {
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

int main()
{
	printf("------------Welcome to the spravochnik 2022!------------\n\n");
	printf("What do you want to do?\n");
	printf("# Add abonent - type 'a'\n");
	printf("# Print spravochnik - type 'p'\n");
	printf("# Find abonent using name - type 'n'\n");
	printf("# Find abonent using number - type '+'\n");
	printf("# Exit - type 'x'\n\n");
	char command;

	while(1) {
		printf("\nCommand: "); 
		command = getchar();
		empty_stdin();

		if (command == 'a')
			add_abonent(spravochnik);
		else if (command == 'p')
			print_spravochnik(spravochnik, spravochnik_size);
		else if (command == 'n')
			search_abonent_name(spravochnik);
		else if (command == '+')
			search_abonent_number(spravochnik);
		else if (command == 'x')
			break;
		else
			printf("Unknown command, try again\n");
	}
  printf("------------Thanks for using spravochnik 2022!------------\n");

	return 0;
}

