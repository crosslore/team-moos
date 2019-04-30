#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main()
{
  char program[] = "./launch.sh 15";
  system(program);
  return 0;
}