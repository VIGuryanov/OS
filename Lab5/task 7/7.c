#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void CompareFileWithDir(struct dirent* f1, DIR* target_dir, char* parent_dir_name, char* target_dir_name, FILE* output)
{
  struct dirent *f2;
  while((f2=readdir(target_dir))!=NULL)
  {
        if((f2->d_type)==DT_REG)
        {
          char* file1Path = malloc(strlen(parent_dir_name)+strlen(f1->d_name)+2);
          char* file2Path = malloc(strlen(target_dir_name)+strlen(f2->d_name)+2);
          FILE* file1;
          FILE* file2;
          sprintf(file1Path,"%s/%s",parent_dir_name,f1->d_name);
          sprintf(file2Path,"%s/%s",target_dir_name,f2->d_name);

          file1=fopen(file1Path, "r");
          if(file1!=NULL)
          {
                file2=fopen(file2Path, "r");
                if(file2!=NULL)
                {
                  char c1, c2;
                  while((c1=getc(file1))==(c2=getc(file2)) && c1!=EOF && c2!=EOF){}
                  if(c1==c2)
                  {
						printf("Found match:\n");
                        fprintf(output,"Found match:\n");
						printf("%s and %s\n",f1->d_name,f2->d_name);
                        fprintf(output, "%s and %s\n",f1->d_name,f2->d_name);
                        struct stat st1;
                        struct stat st2;
                        stat(file1Path, &st1);
                        stat(file2Path, &st2);
						printf("Sizes: %lu and %lu\n",st1.st_size, st2.st_size);
                        fprintf(output, "Sizes: %lu and %lu\n",st1.st_size, st2.st_size);
                        struct tm *time1 = localtime(&(st1.st_ctim).tv_sec);
                        struct tm *time2 = localtime(&(st2.st_ctim).tv_sec);
                        char time_1[80];
                        char time_2[80];
                        strftime(time_1, 80, "%Y-%m-%d %H:%M:%S", time1);
                        strftime(time_2, 80, "%Y-%m-%d %H:%M:%S", time2);
						printf("Created: %s and %s\n", time_1, time_2);
                        fprintf(output, "Created: %s and %s\n", time_1, time_2);
						printf("Access rights: %u and %u\n", st1.st_mode, st2.st_mode);
                        fprintf(output, "Access rights: %u and %u\n", st1.st_mode, st2.st_mode);
						printf("Index descriptor: %lu and %lu\n", st1.st_ino, st2.st_ino);
                        fprintf(output, "Index descriptor: %lu and %lu\n", st1.st_ino, st2.st_ino);
						printf("\n");
                        fprintf(output, "\n");
                  }
                  fclose(file2);
               }
               fclose(file1);
			}
           free(file1Path);
           free(file2Path);
        }
        else if((f2->d_type)==DT_DIR && strcmp((f2->d_name),".")!=0 && strcmp((f2->d_name),"..")!=0)
        {
          char* toOpenPath = malloc(strlen(target_dir_name)+strlen(f2->d_name)+2);
          sprintf(toOpenPath,"%s/%s",target_dir_name,f2->d_name);
          DIR *toOpen;
          toOpen=opendir(toOpenPath);
          if(toOpen!=NULL)
          {
            CompareFileWithDir(f1, toOpen, parent_dir_name, toOpenPath, output);
            closedir(toOpen);
          }
          free(toOpenPath);
        }
  }
}

void CompareDirWithDir(DIR* dir1, DIR* dir2, char* dir_1_name, char* dir_2_name, FILE *output)
{
  struct dirent *f1;
  while((f1=readdir(dir1))!=NULL)
  {
    if((f1->d_type)==DT_REG)
    {
      CompareFileWithDir(f1, dir2, dir_1_name, dir_2_name, output);
      rewinddir(dir2);
    }
    else if((f1->d_type)==DT_DIR && strcmp((f1->d_name),".")!=0 && strcmp((f1->d_name),"..")!=0)
    {
      char* toOpenPath = malloc(strlen(dir_1_name)+strlen(f1->d_name)+2);
      sprintf(toOpenPath,"%s/%s",dir_1_name,f1->d_name);
      DIR *toOpen;
      toOpen=opendir(toOpenPath);
      if(toOpen!=NULL)
      {
        CompareDirWithDir(toOpen, dir2, toOpenPath, dir_2_name, output);
        closedir(toOpen);
      }
      free(toOpenPath);
    }
  }
}

int main( int argc, char *argv[ ], char *envp[ ])
{
  if(argc!=4)
  {
    printf("Usage: {first dir} {second dir} {output file}\n");
    return 1;
  }
  

  FILE *f;
  DIR *dir1;
  DIR *dir2;

  dir1=opendir(argv[1]);
  if(dir1==NULL)
  {
    printf("Unable to open %s directory\n", argv[1]);
        return 1;
  }

  dir2=opendir(argv[2]);
  if(dir2==NULL)
  {
    printf("Unable to open %s directory\n", argv[2]);
    return 1;
  }

  f=fopen(argv[3], "w");
  if(f==NULL)
  {
    printf("Unable to open output file in write mode\n");
    return 1;
  }

  CompareDirWithDir(dir1, dir2, argv[1], argv[2], f);

  if(fclose(f)!=0)
    printf("Unable to close output file");
}