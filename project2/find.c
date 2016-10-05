#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <grp.h>

#define PATH_MAX 4096 //Standard maximum length of pathname on unix based systems
#define FILE_NAME_MAX 256 //Standard maximum length of filename on unix based systems
#define FULL_PATH_MAX (PATH_MAX + FILE_NAME_MAX) //Standard maximum length of path+filename
#define USER_NAME_MAX 32 //Standard maximum length of username

void findFiles(int, int, char*, char*, int);

int main(int argc, char **argv) {

    int ch;
    int uid = -1;
    int mtime = 0;
    char* target = NULL;
    struct passwd *usr;
    while ((ch = getopt(argc, argv, "u:m:xl:")) != -1) {
        switch(ch) {
            case 'u':
                if (sscanf(optarg, "%d", &uid) != 1) { //If it gets a string
                    if ((usr = getpwnam(optarg)) == NULL) {
                        fprintf(stderr, "User cannot be found with name %s", optarg);
                        return -1;
                    }
                    uid = usr->pw_uid;
                }
                break;
            case 'm':
                if (sscanf(optarg, "%d", &mtime) != 1) { //If it gets a string
                    fprintf(stderr, "Invalid argument: %s. Usage: -m should precede integer.\n", optarg);
                    return -1;
                } 
                break;
            case 'x':
                break;
            case 'l':
                target = optarg; //Error checking later when we open the directory
                break;
        }
    }

    char* path = NULL;
    if (optind < argc) {
        path = argv[optind];
    } else {
        fprintf(stderr, "Did not get a starting path.  Please try again.\n");
        return -1;
    }

    findFiles(uid, mtime, target, path, 0); //pass 0 as first call

return 0;
}

void findFiles(int uid, int mtime, char* target, char* path, int first_call) {

    DIR *directory;
    struct dirent *rd;
    struct stat stat_struct;
    struct group *group_info;
    struct passwd *user_info;
    char which_type;
    char* file_owner = malloc(sizeof(char) * USER_NAME_MAX);
    char* group_owner = malloc(sizeof(char) * USER_NAME_MAX);
    int node_size;
    struct tm *modif_time;

    if ((directory = opendir(path)) == NULL) {
        fprintf(stderr, "Failed to open directory %s: %s\n", path, strerror(errno));
        exit(-1);
    }

    char abspath[PATH_MAX]; //Longest valid pathlength on unix systems
    char* fullname = malloc(sizeof(char) * FULL_PATH_MAX);

    realpath(path, abspath);
    int len = strlen(abspath);

    //if (first_call == 0) TODO
    //    printf("%s\n", abspath);

    while ((rd=readdir(directory)) != NULL) {
        //store len from before to keep same length of abspath
        //append a slash for path, then null terminate so strcat gets correct lengths
        realpath(path, abspath);         
        abspath[len] = '/';         
        abspath[len+1] = '\0';

        fullname = strcat(abspath, rd->d_name); //Now we have the full name
                                                //time to recurse
        
        //printf("%s\n", filename); TODO 
        //when we print at end, must include abspath (current directory)

        if ((rd->d_type == DT_DIR) && !((strcmp(rd->d_name, ".") == 0) || (strcmp(rd->d_name, "..") == 0)))
            findFiles(uid, mtime, target, abspath, 1);

        //use lstat over stat to follow the symbolic links
        if (lstat(fullname, &stat_struct) != 0) {
            fprintf(stderr, "Failed to get stat struct on %s: %s\n", fullname, strerror(errno));
            continue; //Not cause for exiting program completely
        }
        
        //handle the -m and -u cases
        if (((uid != -1) && (uid != stat_struct.st_uid)) || ((mtime > 0) && (time(0)-stat_struct.st_mtime) < mtime) || ((mtime < 0) && (-1)*(time(0) - stat_struct.st_mtime) < mtime))
            continue; //no error message as this is not an error
        
        user_info = getpwuid(stat_struct.st_uid);
        group_info = getgrgid(stat_struct.st_uid);

        if (user_info->pw_name != NULL)
            file_owner = user_info->pw_name;
        else
            sprintf(file_owner,"%d", stat_struct.st_uid);
            
        if (group_info->gr_name != NULL)
            group_owner = group_info->gr_name;
        else
            sprintf(group_owner, "%d", stat_struct.st_gid);

        if ((rd->d_type == DT_BLK) || (rd->d_type == DT_CHR)) {
            node_size = stat_struct.st_rdev; //Device ID instead of node size
            which_type = 'h'; //device ID in hex
        } else {
            node_size = stat_struct.st_size;
            which_type = 'i'; //node size in int
        } 

        modif_time = localtime(&stat_struct.st_mtime);
        char* date = malloc(sizeof(char) * 32);
        sprintf(date, "%d-%02d-%02d %02d:%02d:%02d", modif_time->tm_year+1900, modif_time->tm_mon, modif_time->tm_mday, modif_time->tm_hour, modif_time->tm_min, modif_time->tm_sec);
        
        char file_type;
        

    
    }


    return;
}
