#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

void PrintCurrentTime()
{
  struct timeval curTime;
  gettimeofday(&curTime, NULL);
  int milli = curTime.tv_usec / 1000;

  char buffer[20];
  strftime(buffer, 20, "%H:%M:%S", localtime(&curTime.tv_sec));

  printf("Current time: %s:%03d \n", buffer, milli);
}

void DoChild()
{
   printf ("Это Дочерний процесс, его pid=%d\n", getpid());
   printf ("А pid его Родительского процесса=%d\n", getppid());
   PrintCurrentTime();
   sleep(10);
}

void DoParent()
{
    pid_t pid;
    pid = fork();
    if (pid == 0)
      DoChild();
    else if (pid > 0)
    {
      printf ("Это Родительский процесс pid=%d\n", getpid());
      printf ("А pid его Родительского процесса=%d\n", getppid());
      PrintCurrentTime();
      system("ps -x");
      sleep(10);
    }
    else
      printf ("Ошибка вызова fork, потомок не создан\n");
}

int main()
{
    pid_t pid;
    pid = fork ();
    if (pid == 0)
        DoChild();
    else if (pid > 0)
        DoParent();
    else
        printf ("Ошибка вызова fork, потомок не создан\n");
}