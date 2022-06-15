#include <stdio.h>
#include <stdlib.h>

#define NAME_SIZE 20
#define NUMBER_SIZE 11
#define SPRAVOCHNIK_SIZE 3

struct abonent 
{
	char name[NAME_SIZE];
	char number[NUMBER_SIZE];	//+X-XXX-XXX-XX-XX
} spravochnik[SPRAVOCHNIK_SIZE] = {};

void add_abonent(struct abonent *spravochnik)
{
	int id = 0;
	for (int i = 0; i < SPRAVOCHNIK_SIZE; i++) {
		if (spravochnik[i].name[0] && spravochnik[i].number[0])
			id++;
	}
	if (id >= SPRAVOCHNIK_SIZE) 
		printf("The memory is full, reccomend to erase spravochnik\n");

	struct abonent *ptr = spravochnik;
	
  ptr += id;
  
	printf("--------ADD NEW ABONENT--------\n");
	printf("Name (20 symbols max): ");
	fgets(ptr->name, NAME_SIZE, stdin);
	
	printf("Telephone number (+X-XXX-XXX-XX-XX): ");
	fgets(ptr->number, NUMBER_SIZE, stdin);
  ++id;
  if (id >= 99) {
    printf("Spravochnik is full, next record will overwrite record #1");
    id = 0;
    ptr = spravochnik;
  }
}
void print_abonent(const struct abonent *spravochnik, const int number)
{
	if (number > SPRAVOCHNIK_SIZE) {
		printf("We can't print such abonent, try less number");
		return;
	}
  printf("\nAbonent #%i\nName: ", number);
	for(int i = 0; i < NAME_SIZE; ++i) 
		printf("%c", spravochnik[number - 1].name[i]);
	printf("Number: ");
	for(int i = 0; i < NUMBER_SIZE; ++i) 
		printf("%c", spravochnik[number - 1].number[i]);
}
void print_spravochnik(const struct abonent *spravochnik, int amount) 
{
	if (amount > SPRAVOCHNIK_SIZE) {
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
	for (int i = 0; i < NAME_SIZE; ++i) {
			if (name1[i] != name2[i])
				return 0;
	}
	return 1;
}
char compare_numbers(const char *number1, const char *number2)
{
	for (int i = 0; i < NUMBER_SIZE; ++i) {
			if (number1[i] != number2[i])
				return 0;
	}
	return 1;
}

void search_abonent_name(const struct abonent *spravochnik)
{
	printf("--------SEARCHING ABONENT--------\n");
	char name_to_find[NAME_SIZE] = {};

	printf("What name do you want to find? Type here: ");
	fgets(name_to_find, NAME_SIZE, stdin);

	for (int i = 0; i < SPRAVOCHNIK_SIZE; ++i) {
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
	char number_to_find[NUMBER_SIZE] = {};

	printf("What number do you want to find (+X-XXX-XXX-XX-XX)? Type here: ");
	fgets(number_to_find, NUMBER_SIZE, stdin);

	for (int i = 0; i < SPRAVOCHNIK_SIZE; ++i) {
		if (compare_numbers(spravochnik[i].number, number_to_find) == 1) {
			printf("Your abonent is Abonent#%i \n", i + 1);
			print_abonent(spravochnik, i + 1);
			return;
		}
	}
	printf("We can't find your abonent, please check the number\n");
}
void erase(struct abonent *spravochnik)
{
	for (int i = 0; i < SPRAVOCHNIK_SIZE; i++) {
		for (int y = 0; y < NAME_SIZE; y++) 
			spravochnik[i].name[y] = 0;
		for (int y = 0; y < NUMBER_SIZE; y++) 
			spravochnik[i].number[y] = 0;
	}
	printf("Erased!\n");
	return;
}
void empty_stdin() {
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

FILE *spravochnik_file;

int main()
{
	spravochnik_file = fopen ("Spravochnik", "a+");
		if (!spravochnik_file) {
			perror ("fopen");
			return 1;
		}
	fread (spravochnik, sizeof (struct abonent), SPRAVOCHNIK_SIZE, spravochnik_file);
	if (fclose (spravochnik_file)) {
  	perror("fclose");
  	return 1;
  } 

	printf("------------Welcome to the spravochnik 2022!------------\n\n");
	printf("What do you want to do?\n");
	printf("# Add abonent - type 'a'\n");
	printf("# Print spravochnik - type 'p'\n");
	printf("# Find abonent using name - type 'n'\n");
	printf("# Find abonent using number - type '+'\n");
	printf("# Erase memory - type 'e'\n");
	printf("# Exit - type 'x'\n\n");
	char command;

	while(1) {
		printf("\nCommand: "); 
		command = getchar();
		empty_stdin();

		if (command == 'a')
			add_abonent(spravochnik);
		else if (command == 'p')
			print_spravochnik(spravochnik, SPRAVOCHNIK_SIZE);
		else if (command == 'n')
			search_abonent_name(spravochnik);
		else if (command == '+')
			search_abonent_number(spravochnik);
		else if (command == 'e')
			erase(spravochnik);
		else if (command == 'x')
			break;
		else
			printf("Unknown command, try again\n");
	}
  printf("------------Thanks for using spravochnik 2022!------------\n");

  spravochnik_file = fopen ("Spravochnik", "w");
		if (!spravochnik_file) {
			perror ("fopen");
			return 1;
		}
  if (!fwrite (spravochnik, sizeof (struct abonent), SPRAVOCHNIK_SIZE, spravochnik_file)) {
  	perror ("fwrite");
  	return 1;
  }
  if (fclose (spravochnik_file)) {
  	perror("fclose");
  	return 1;
  } 
	return 0;
}

