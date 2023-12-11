#include <stdio.h>
#include <dirent.h>

void PrintDir(DIR* d)
{
  struct dirent *cont;
  while((cont=readdir(d))!=NULL)
  {
    printf("%s\n",cont->d_name);
  }
}

int main( int argc, char *argv[ ], char *envp[ ])
{
  if(argc!=2)
  {
    printf("Usage: {another dir name}\n");
    return 1;
  }

  DIR *dir1;
  DIR *dir2;

  dir1=opendir(".");
  if(dir1==NULL)
  {
    printf("Unable to open current directory\n");
    return 1;
  }

  dir2=opendir(argv[1]);
  if(dir2==NULL)
  {
    printf("Unable to open given directory\n");
    return 1;
  }

  printf("Current directory:\n");
  PrintDir(dir1);
  printf("\n");
  printf("%s:\n",argv[1]);
  PrintDir(dir2);

  if(closedir(dir1)!=0)
  {
    printf("Unable to close current directory\n");
    return 1;
  }
  if(closedir(dir2)!=0)
  {
    printf("Unable to close given directory\n");
    return 1;
  }
}