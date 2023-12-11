#include <stdio.h>

int main( int argc, char *argv[ ], char *envp[ ])
{
  if(argc !=2 )
  {
    printf("Enter output filename as argument\n");
    return 1;
  }
  char c;
  FILE *f;
  f=fopen(argv[1], "w");
  if(f==NULL)
  {
    printf("Unable to open file in write mode\n");
    return 1;
  }
  printf("Press Ctrl+f to end\n");
  while((c=getc(stdin))!=6)
  {
    putc(c, f);
  }
  if(fclose(f)!=0)
    printf("Unable to close file");
}