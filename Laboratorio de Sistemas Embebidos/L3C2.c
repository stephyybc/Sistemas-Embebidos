#include <stdio.h>
#include <string.h>

int main(int argc,char*argv[]) {
  if(strcmp(argv[1],"-a") || strcmp(argv[1],"-p") || (strcmp(argv[2],"-l") && strcmp(argv[4],"-w"))
  {
    if(strcmp(argv[1],"-a"))
    {
      int area = (atoi(argv[3])*atoi(argv[5]));
      printf("Area: %d units\n",area);
    }
    if (strcmp(argv[1],"-p"))
    {
      int perimetro = (atoi(argv[3])*2 + atoi(argv[5])*2);
      printf("Perimetro: %d units\n",perimetro);
    }
  }
  printf("\n");
  return 0;
}