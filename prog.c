/*
 Kolawole Ladipo
 Linux Shell Scripting Assignment
 11/6/2021
 */

/*
 *      _  ______  _          __          ______  _      ______ 
 *     | |/ / __ \| |        /\ \        / / __ \| |    |  ____|
 *     | ' / |  | | |       /  \ \  /\  / / |  | | |    | |__   
 *     |  <| |  | | |      / /\ \ \/  \/ /| |  | | |    |  __|  
 *     | . \ |__| | |____ / ____ \  /\  / | |__| | |____| |____ 
 *     |_|\_\____/|______/_/    \_\/  \/   \____/|______|______|
 *                                                              
 *                                                              
 *
 *	  *************DIRECTORY LISTING SCRIPT*************
 *	  Program takes a command line argument and displays
 *	  the contents of the directory.  Program also lists
 *	  each individual  file size,  total directory size,
 *	  and  total  number  of  files  in  the  directory.
 *	  **************************************************
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <ftw.h>


static unsigned int total = 0;
int countfiles(char *path);
int sum(const char *fpath, const struct stat *sb, int n);


void printDir(char *pathP, const int root) // takes in command line arg
{
    char abs_path[3000];
    int i;
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(pathP);

    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
	/* next line removes "." and ".." directories from the listing
	 * the for loop creates the indentation for the file hierarchy*/

        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            for (i=0; i<root; i++) 
            {
                if (i%2 == 0 || i == 0){
                    printf("%c  ", ' ');
		}
                else{
                    printf(" ");
	            }
            }

            // if directory entry is a regular file
	    if(dp->d_type == DT_REG){
           	 printf("%c   %s", ' ',  dp->d_name);
		
		 snprintf(abs_path, 30000, "%s/%s", pathP, dp->d_name);
	   	 FILE* fp = fopen(abs_path, "r"); // This portion for size
	    	 if(fp == NULL){
	    		printf("\n");
			continue;
	   		}
	         fseek(fp, 0L, SEEK_END);
		 long int res = ftell(fp);
		 printf("   %ld\n", res);
		 fclose(fp);

	    }
	    // if entry is not a regular file, it's a folder
	    else{
	   	 printf("%c   %s%c%c\n", ' ', dp->d_name, ' ',  '/');
	   	 


            strcpy(path, pathP);
            strcat(path, "/");
            strcat(path, dp->d_name);
	    printDir(path, root + 2);
	    
	}}
    }

    closedir(dir);
}





int main(int argc, char *argv[])
{
	// Pointer for directory entry
	struct dirent *de;

	// opendir() returns a pointer of DIR type.
	DIR *directory = opendir(argv[1]);

	if (directory == NULL) // opendir returns NULL if dir not opened
	{
		printf("Could not open directory\n" );
		return 0;
	}

	// for the SUM function (for calculating total space usage
	if(ftw(argv[1], &sum, 1)){
		perror("ftw");
		return 2;
		}

	printf("\nDirectory listing of %s:\n\n", argv[1]);
	
	char *ptr;	// for listing directory name
	int ch = '/';

	ptr = strrchr(argv[1], ch);
	printf("%s\n", ptr+1);	
	printDir(argv[1], 0);

	printf("\nTotal space usage:  %d bytes\n", total);
	printf("Total number of files:  #");
	printf("%d", countfiles(argv[1]));	// countfiles function
	printf("\n\nDone.\n");

	return 0;
}


int countfiles(char *path) {
    DIR *dir_ptr = NULL;
    struct dirent *direntp;
    char *npath;
    if (!path) return 0;
    if( (dir_ptr = opendir(path)) == NULL ) return 0;

    int count=0;
    while( (direntp = readdir(dir_ptr)))
    {
        if (strcmp(direntp->d_name,".")==0 ||
            strcmp(direntp->d_name,"..")==0) continue;
        switch (direntp->d_type) {
            case DT_REG:
                ++count;
                break;
            case DT_DIR:            
                npath=malloc(strlen(path)+strlen(direntp->d_name)+2);
                sprintf(npath,"%s/%s",path, direntp->d_name);
                count += countfiles(npath);
                free(npath);
                break;
        }
    }
    closedir(dir_ptr);
    return count;
}

int sum(const char *fpath, const struct stat *sb, int n){
	total += sb->st_size;
	return 0;
}
