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
char* append(char* s1, char* s2);

int dev_number;

int main(int argc, char **argv) {

    int ch;
    int uid = -1;
    int mtime = 0;
    int dev_number = -1;
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
    if (optind == (argc-1)) {
        path = argv[optind];
    } else {
        fprintf(stderr, "Did not get a single valid starting path.  Please try again.\n");
        return -1;
    }

    findFiles(uid, mtime, target, path, 1); //pass 0 as first call

return 0;
}

void findFiles(int uid, int mtime, char* target, char* path, int first_call) {

    DIR *directory;
    struct dirent *rd;
    struct stat stat_struct;
    struct stat symlink_struct;
    struct stat target_struct;
    struct group *group_info;
    struct passwd *user_info;
    char which_type;
    char* file_owner = malloc(sizeof(char) * USER_NAME_MAX);
    char* group_owner = malloc(sizeof(char) * USER_NAME_MAX);
    int node_size;
    struct tm *modif_time;
    char* called_path = malloc(sizeof(char)*FULL_PATH_MAX);

    if ((directory = opendir(path)) == NULL) {
        fprintf(stderr, "Failed to open directory %s: %s\n", path, strerror(errno));
        exit(-1);
    }

    while ((rd=readdir(directory)) != NULL) {

        if (strcmp(rd->d_name, "..") == 0)
            continue;

        //use lstat over stat to follow the symbolic links
        char* file_name = malloc(sizeof(char)*FULL_PATH_MAX);
        sprintf(file_name, "%s/%s", path, rd->d_name);

        if (lstat(file_name, &stat_struct) != 0) {
            fprintf(stderr, "Failed to get stat struct on %s: %s\n", rd->d_name, strerror(errno));
            continue; //Not cause for exiting program completely
        }
        if (first_call == 1)
            dev_number = stat_struct.st_dev;

        if (stat_struct.st_dev != dev_number)
            fprintf(stderr, "Note: Not crossing mount point at %s\n", file_name);
            //Will not recurse next time

        if ((rd->d_type == DT_DIR) && !((strcmp(rd->d_name, ".") == 0) || (strcmp(rd->d_name, "..") == 0)) && (dev_number == stat_struct.st_dev)) {
            sprintf(called_path, "%s/%s", path, rd->d_name);
            findFiles(uid, mtime, target, called_path, 0);
            continue;
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
        sprintf(date, "%d-%02d-%02d  %02d:%02d:%02d", modif_time->tm_year+1900, modif_time->tm_mon, modif_time->tm_mday, modif_time->tm_hour, modif_time->tm_min, modif_time->tm_sec);
 
        //TYPES FROM NOTES
        char perm_0, perm_1, perm_2, perm_3, perm_4, perm_5, perm_6, perm_7, perm_8, perm_9;
        if (S_ISREG(stat_struct.st_mode))
            perm_0 = '-';
        else if (S_ISDIR(stat_struct.st_mode))
            perm_0 = 'd';
        else if (S_ISLNK(stat_struct.st_mode))
            perm_0 = 'l';
        else if (S_ISCHR(stat_struct.st_mode))
            perm_0 = 'c';
        else if (S_ISBLK(stat_struct.st_mode))
            perm_0 = 'b';
        else if (S_ISFIFO(stat_struct.st_mode))
            perm_0 = 'p';
        else if (S_ISSOCK(stat_struct.st_mode))
            perm_0 = 's';
        else
            perm_0 = '-';
       //perm3, 6, 9 from notes as well as help from stack overflow 
        if (stat_struct.st_mode & S_IRUSR)
            perm_1 = 'r';
        else
            perm_1 = '-';

        if (stat_struct.st_mode & S_IWUSR)
            perm_2 = 'w';
        else
            perm_2 = '-';

        if (stat_struct.st_mode & S_IXUSR)
            perm_3 = 'x';
        else
            perm_3 = '-';
        if (stat_struct.st_mode & S_ISUID) {
            if (stat_struct.st_mode & S_IXUSR)
                perm_9 = 's';
            else
                perm_9 = 'S';
        }

        if (stat_struct.st_mode & S_IRGRP)
            perm_4 = 'r';
        else
            perm_4 = '-';

        if (stat_struct.st_mode & S_IWGRP)
            perm_5 = 'w';
        else
            perm_5 = '-';

        if (stat_struct.st_mode & S_IXGRP)
            perm_6 = 'x';
        else
            perm_6 = '-';
        if (stat_struct.st_mode & S_ISGID) {
            if (stat_struct.st_mode & S_IXGRP)
                perm_6 = 's';
            else
                perm_6 = 'S';
        }
        
        if (stat_struct.st_mode & S_IROTH)
            perm_7 = 'r';
        else
            perm_7 = '-';

        if (stat_struct.st_mode & S_IWOTH)
            perm_8 = 'w';
        else
            perm_8 = '-';

        if (stat_struct.st_mode & S_IXOTH)
            perm_9 = 'x';
        else
            perm_9 = '-';
        if (stat_struct.st_mode & S_ISVTX) {
            if (stat_struct.st_mode & S_IXOTH)
                perm_9 = 't';
            else
                perm_9 = 'T';
        }

        char perms[11];
        sprintf(perms, "%c%c%c%c%c%c%c%c%c%c", perm_0, perm_1, perm_2, perm_3, perm_4, perm_5, perm_6, perm_7, perm_8, perm_9);

        //handle the -l case

        char symlink[FULL_PATH_MAX];
        if (target) {
            if (perm_0 != 'l') {
                continue;
            }
            if(perm_0 == 'l') {
                if(readlink(file_name,symlink, FULL_PATH_MAX) == -1) {
                    fprintf(stderr, "Unable to follow get symlink: %s\n", strerror(errno));
                    continue;
                }
                symlink[strlen(symlink)-2] = '\0';
                if ((lstat(symlink, &symlink_struct) != 0) && (lstat(target, &target_struct) != 0)) {
                    if ((symlink_struct.st_dev != target_struct.st_dev) || (symlink_struct.st_ino != target_struct.st_ino)) {
                        continue;
                    } else {
                        sprintf(file_name, "%s -> %s", file_name, symlink);
                    }
                }
            }
        }

        if (perm_0 == 'l') {
            if(readlink(file_name, symlink, FULL_PATH_MAX) != -1)
                sprintf(file_name, "%s -> %s", file_name, symlink);
        }

        if (which_type == 'i') {
            printf("%d / %llu\t%c%c%c%c%c%c%c%c%c%c  %d  %s \t%s %d\t  %s   %s\n", (int) stat_struct.st_dev, (unsigned long long) stat_struct.st_ino, perm_0, perm_1, perm_2, perm_3, perm_4, perm_5, perm_6, perm_7, perm_8, perm_9, (int) stat_struct.st_uid, file_owner, group_owner, (int) node_size, date, file_name);
        } else {
            printf("%d / %llu\t%c%c%c%c%c%c%c%c%c%c  %x  %s \t%s %d\t  %s   %s\n", (int) stat_struct.st_dev, (unsigned long long) stat_struct.st_ino, perm_0, perm_1, perm_2, perm_3, perm_4, perm_5, perm_6, perm_7, perm_8, perm_9, (int) stat_struct.st_uid, file_owner, group_owner, (int) node_size, date, file_name);
        }
    }

    return;
}
