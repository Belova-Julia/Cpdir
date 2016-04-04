#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

struct El_file       
{ 
   char in_where[256];  
   char out_where[256]; 
}; 

struct List
{
   struct El_file* mas_file;    
   int fileCount;             
   int nextFile; 
};

static void _mkdir(const char *dir);
void fillRecursive(char* dir_from, char* dir_to, int* current);
void countRecursive(char* dir, int* current);
void copyFile();

struct List list={NULL, 0, 0};

int ChildFilesCopied = 0;


int main(int argc, char* argv[])
{  
   int m;
   char *in_dir = NULL;
   char *out_dir = NULL;
   if (argc >= 3)
   {
      m=atoi(argv[1]);
      in_dir = argv[2];
      out_dir=argv[3];
   }
   else 
   {
      printf("Error. Few arg.");
      exit(0);
   }

      if (m<1)
   { 
      printf("Invalid quantity of process");
      exit(0);
   }

   DIR *dp_i; 
   DIR *dp_o;
   if ((dp_i = opendir(in_dir)) == NULL) 
   {
      fprintf(stderr, "there isn't in directory: %s\n", in_dir);
      exit(0);
   }
   if (( dp_o = opendir(out_dir)) == NULL) 
   {
      _mkdir(out_dir);
      dp_o = opendir(out_dir);
   }

   countRecursive(in_dir, &list.fileCount);
   printf("File: %d",list.fileCount);
   list.mas_file = malloc(list.fileCount * sizeof (struct El_file));
   int counter = 0;
   fillRecursive(in_dir, out_dir, &counter);  
   copyFile();
   free(list.mas_file);
   exit(0);
}

void _mkdir(const char *dir)
{
   char tmp[256];
   char *p = NULL;
   size_t len;
 
   snprintf(tmp, sizeof(tmp),"%s",dir);
   len = strlen(tmp);
   if(tmp[len - 1] == '/')
      tmp[len - 1] = 0;
   for(p = tmp + 1; *p; p++)
      if(*p == '/') 
      {
         *p = 0;
         mkdir(tmp, S_IRWXU);
         *p = '/';
      }
   mkdir(tmp, S_IRWXU);
}

void fillRecursive(char* dir_from, char* dir_to, int* current)
{
   DIR *dp;
   struct dirent *entry;
   dp = opendir(dir_from);
   entry = readdir(dp);
   while(entry) {
   if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
      {
        entry = readdir(dp);
        continue;
       } 
   if(entry->d_type == DT_DIR) 
   {
      char * a = malloc(256*sizeof(char));
      char * b = malloc(256*sizeof(char));
      snprintf(a, 256, "%s/%s", dir_from, entry->d_name);
      snprintf(b, 256, "%s/%s", dir_to, entry->d_name);
      if(!(opendir(b)))  
         if(mkdir(b, S_IRWXU))
              printf("Err");
      fillRecursive(a, b, current);
      free(b);
      free(a);
   }
   else 
      if (entry->d_type == DT_REG) 
      {
         snprintf(list.mas_file[*current].in_where, 256, "%s/%s", dir_from, entry->d_name);
	 snprintf(list.mas_file[*current].out_where, 256, "%s/%s", dir_to, entry->d_name);
	 ++(*current);
      }
      entry = readdir(dp);
  }
  closedir(dp);
}

void countRecursive(char* dir, int* current)
{
   DIR *dp;
   struct dirent *entry;
   dp = opendir(dir);
   entry = readdir(dp);
   while(entry) 
   {
     if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
      {
        entry = readdir(dp);
        continue;
       } 
      if(entry->d_type == DT_DIR) 
      {
         char * a = malloc(256*sizeof(char));
	 snprintf(a, 256, "%s/%s", dir, entry->d_name);
	 countRecursive(a, current);
         free(a);
      }
      else if (entry->d_type == DT_REG) 
		++(*current);
      entry = readdir(dp);
   }
   closedir(dp);
}

void copyFile()
{
   while(ChildFilesCopied < list.fileCount)
   {
      FILE * inFile; 
      FILE * outFile;
      int  n;
      char buf [512];
      inFile=fopen(list.mas_file[ChildFilesCopied].in_where,"rb");
      outFile=fopen(list.mas_file[ChildFilesCopied].out_where,"wb");
      while ((n = read(fileno(inFile), buf, sizeof(buf))) > 0)
         write(fileno(outFile), buf, n);  
   ChildFilesCopied++;
   }
   exit(0);
}
