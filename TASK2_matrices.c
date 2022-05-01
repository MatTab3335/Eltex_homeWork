#include <stdio.h>

void matrix_1(void)
{
  int a;
  int matrix[5][5];
  //fill matrix with a
  for (int i = 0; i < 5; i++)
  {
    for (int y = 0; y < 5; y++)
    {
      a++;
      matrix[i][y] = a;
    }
  }
  //print matrix
  for (int i = 0; i < 5; i++)
  {
    for (int y = 0; y < 5; y++)
    {
      printf("%d ", matrix[i][y]);
    }
    printf("\n");
  }
}

void matrix_2(void)
{
  int n = 6;
  int array[n];
  int reverse_array[n];
  //fill matrix
  for (int i = 0; i < n; i++)
  {
    array[i] = i + 1;
  }
  //print matrix
  printf("Array: ");
  for (int i = 0; i < n; i++)
  {
    printf("%d ", array[i]);
  }
  //reverse matrix
  for (int i = 0; i < n; i++)
  {
    reverse_array[i] = array[(n-1)-i];
  }
  //print matrix in reverse
  printf("\nArray in reverse: ");
  for (int i = 0; i < n; i++)
  {
    printf("%d ", reverse_array[i]);
  }
}

void matrix_3(void)
{
  int n = 5;
  char matrix[n][n];
  int a;
  for (int i = 0; i < n; i++)     //columns
  {
    for (int y = i; y < n; y++)   //rows
    {
      matrix[i][y] = 1;
    }
  }
  for (int i = 0; i < n; i++)
  {
    for (int y = 0; y < n; y++)
    {
      printf("%d ", matrix[i][y]);
    }
    printf("\n");
  }
}

//the function uses pointer to fill the matrix in the memory
void matrix_4(void)
{
  int n = 5;                 //matrix size - works for all sizes
  int matrix[n][n];
  int jump_amount = n;      //number of jumps in one for-cycle
  int jump = n;             //size of jump
  int operation_id = 1;     //store number of jumps with certain jump length, when == 2 reset to 0
  int a = 0;                //increments every step and fill the matrix elements
  int *ptr = &matrix[0][0]; //pointer to matrix
  
  for (int i = 0; i < n; i++)   //clear the matrix
  {
    for (int y = 0; y < n; y++)
      matrix[i][y] = 0;
  }
  //fill matrix using ptr: 1 2 3 4 5 | 16 17 18 19 6 | 15 24 25 20 7 | 14 23 22 21 8 | 13 12 11 10 9

  ptr -= 1;                                //make step back to start cycle with 0 element
  //fill the matrix
  while(1)
  {
    for (int i = 0; i < jump_amount; i++)
    {
      ptr += 1;
      a++;
      *ptr = a;
    }
    //check if 2 for-cycles have passed
    operation_id++;
    if (operation_id == 2){
      jump_amount--;
      operation_id = 0;
      if (jump_amount == 0) break;
    }
    
    for (int i = 0; i < jump_amount; i++)
    {
      a++;
      ptr += jump;
      *ptr = a;
    }
    operation_id++;
    if (operation_id == 2){
      jump_amount--;
      operation_id = 0;
      if (jump_amount == 0) break;
    }
    
    for (int i = 0; i < jump_amount; i++)
    {
      a++;
      ptr -= 1;
      *ptr = a;
    }
    operation_id++;
    if (operation_id == 2){
      jump_amount--;
      operation_id = 0;
      if (jump_amount == 0) break;
    }
    
    for (int i = 0; i < jump_amount; i++)
    {
      a++;
      ptr -= jump;
      *ptr = a;
    }
    operation_id++;
    if (operation_id == 2){
      jump_amount--;
      operation_id = 0;
      if (jump_amount == 0) break;
    }
  }
  
  //print matrix
  for (int i = 0; i < n; i++)
  {
    for (int y = 0; y < n; y++)
    {
      printf("%d ", matrix[i][y]);
    }
    printf("\n");
  }
}
int main(void)
{
/*----uncomment necessary function*/
//  matrix_1();
//  matrix_2();
//  matrix_3();
  matrix_4();
  return 0;
}
