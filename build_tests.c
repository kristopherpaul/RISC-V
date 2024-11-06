#include<stdio.h>
#include<dirent.h>
#include <string.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

void exec_cmd(char* cmd){
    FILE *fp = popen(cmd, "r");
    if(fp != 0){
        char s[64];
        while(fgets(s, sizeof(s), fp) != NULL){
            fputs(s, stdout);
        }
        pclose(fp);
    }
}


// to be called in the format:
// build_tests <build_files> <isa_source_address>
int main(int argc, char* argv[]) {
    if(argc!=3) {
        perror("Invalid format");
        return 1;
    }
    char cmd1[1000] = "mkdir tests/";
    strcat(cmd1, argv[1]);
    strcat(cmd1, "_tests/");
    exec_cmd(cmd1);
    struct dirent *entry;
    DIR *dp = opendir(argv[2]);
    if(dp == NULL){
        perror("opendir");
        return 1;
    }
    int tc = 0;
    short failed = 0;
    while((entry = readdir(dp))){
        if(strstr(entry->d_name, argv[1]) && strlen(entry->d_name) > strlen(argv[1])) {
            if(strstr(entry->d_name, ".dump")){
                char cmd[1000] = "cp ";
                strcat(cmd, argv[2]);
                strcat(cmd, "/");
                strcat(cmd, entry->d_name);
                strcat(cmd, " tests/");
                strcat(cmd, argv[1]);
                strcat(cmd, "_tests");
                exec_cmd(cmd);
            }
            else {
                char cmd[1000] = "riscv64-unknown-elf-objcopy -O binary ";
                strcat(cmd, argv[2]);
                strcat(cmd, "/");
                strcat(cmd, entry->d_name);
                strcat(cmd, " tests/");
                strcat(cmd, argv[1]);
                strcat(cmd, "_tests/");
                strcat(cmd, entry->d_name);
                strcat(cmd, ".bin");
                exec_cmd(cmd);
            }
        }
    }
    closedir(dp);
    return 0;
}