#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdbool.h>

int maxProcNum = 0;
int procCount = 0;

void CompareFiles(FILE *file1, FILE *file2, char* file1Path, char* file2Path, char* file1Name, char* file2Name)
{
	char c1, c2;
	int looked = 2;
	while( (c1=getc(file1))==(c2=getc(file2)) && c1!=EOF && c2!=EOF){looked+=2;}

	int flag = false;
	if(c1==c2)
		flag = true;

	printf("Process pid=%d \nFiles %s and %s\nCompared %d bytes\nResult: %s \n\n", getpid(), file1Name,file2Name, looked, flag?"Equal":"Unequal");

	fclose(file2);
	fclose(file1);
	free(file1Path);
	free(file2Path);

	sleep(2);
	exit(0);  
}

void CompareFileWithDir(struct dirent* f1, DIR* target_dir, char* parent_dir_name, char* target_dir_name)
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
				pid_t pid;
				while(procCount == maxProcNum) 
				{
					//printf("procCount %d waiting\n", procCount); 
					sleep(1);
				}
				procCount++;
				pid = fork();
				if(pid == 0)
				{
					//printf("Created process pid=%d\n", getpid());
					CompareFiles(file1, file2, file1Path, file2Path, f1->d_name, f2->d_name);
				}
			}
			else
				fclose(file1);
		}
		else
		{
			free(file1Path);
			free(file2Path);
		}
	}
	else if((f2->d_type)==DT_DIR && strcmp((f2->d_name),".")!=0 && strcmp((f2->d_name),"..")!=0)
	{
		char* toOpenPath = malloc(strlen(target_dir_name)+strlen(f2->d_name)+2);
		sprintf(toOpenPath,"%s/%s",target_dir_name,f2->d_name);
		DIR *toOpen;
		toOpen=opendir(toOpenPath);
		if(toOpen!=NULL)
		{
		CompareFileWithDir(f1, toOpen, parent_dir_name, toOpenPath);
		closedir(toOpen);
		}
		free(toOpenPath);
	}
  }
}

void CompareDirWithDir(DIR* dir1, DIR* dir2, char* dir_1_name, char* dir_2_name)
{
	struct dirent *f1;
	while((f1=readdir(dir1))!=NULL)
	{
		if((f1->d_type)==DT_REG)
		{
			CompareFileWithDir(f1, dir2, dir_1_name, dir_2_name);
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
				CompareDirWithDir(toOpen, dir2, toOpenPath, dir_2_name);
				closedir(toOpen);
			}
			free(toOpenPath);
		}
	}
}

static void
child_handler(int sig)
{
    pid_t pid;
    int status;

    //printf("process pid=%d ended live\n", waitpid(-1, &status, WNOHANG));
	while((pid = waitpid(-1, &status, WNOHANG)) > 0)
	{
		procCount--;
		//printf("process pid=%d ended live\n", pid);
	}
}

int main( int argc, char *argv[ ], char *envp[ ])
{
	if(argc!=4)
	{
		printf("Usage: {first dir} {second dir} {max process number}\n");
		return 1;
	}

	maxProcNum = atoi(argv[3]);
	if(maxProcNum < 1)
	{
		printf("Too low maximum process number. Should be at least 1");
		return 1;
	}

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

	signal(SIGCHLD, child_handler);

	CompareDirWithDir(dir1, dir2, argv[1], argv[2]);
	while(procCount > 0){}
}