#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main( int argc, char *argv[ ], char *envp[ ])
{
  if(argc!=3)
  {
    printf("Usage: {filename} {rows count}\n");
    return 1;
  }
  char c;
  FILE *f;
  f=fopen(argv[1], "r");
  if(f==NULL)
  {
    printf("Unable to open file in read mode\n");
    return 1;
  }
  int n = atoi(argv[2]);
  bool eof = false;
  if(n==0)
    while((c=getc(f))!=EOF)
    {
          printf("%c",c);
    }
  else
    while(1)
    {
	  int endlCount=0;

	  while(endlCount<n)
	  {
		 c=getc(f);
		 if(c==EOF)
		 {
		   eof=true;
		   break;
		 }
		 printf("%c",c);
		 if(c=='\n')
		   endlCount++;
	  }

	  if(eof)
		break;
	  getc(stdin);
    }

  if(fclose(f)!=0)
    printf("Unable to close file");
}