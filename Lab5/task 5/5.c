#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

int main( int argc, char *argv[ ], char *envp[ ])
{
  if(argc!=3)
  {
    printf("Usage: {copy from} {copy to}\n");
    return 1;
  }
  char c;

  FILE *from;
  from=fopen(argv[1], "r");
  if(from==NULL)
  {
    printf("Unable to open file1 in read mode\n");
    return 1;
  }

  FILE *to;
  to=fopen(argv[2], "w");
  if(to==NULL)
  {
    printf("Unable to open file2 in write mode\n");
    return 1;
  }

  while((c=getc(from))!=EOF)
  {
    putc(c, to);
  }

  struct stat st1;
  stat(argv[1], &st1);
  chmod(argv[2], st1.st_mode);

  if(fclose(from)!=0)
    printf("Unable to close file1");
  if(fclose(to)!=0)
    printf("Unable to close file2");
}