#include <stdio.h>


int origin = 0xFAFAFAFA; //11111010 11111010 11111010 11111010
int byte;


int main(void)
{

  printf("Enter a byte in hex: ");
  scanf("%x", &byte);
  printf("You entered: %x\n", byte);
  origin &= 0xFF00FFFF;               //make 3d byte empty
  byte <<= 16;
  origin |= byte;                     //fill 3d byte
  printf("New number is: %x\n", origin);    //nice!
  return 0;
}
