#include<stdio.h>
#include<dirent.h>
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

int main() {
    struct dirent *entry;
    DIR *dp = opendir("tests");

    if(dp == NULL){
        perror("opendir");
        return 1;
    }
    int tc = 0;
    while((entry = readdir(dp))){
        if(strstr(entry->d_name, ".bin")){
            tc++;
            int len = strstr(entry->d_name, ".bin") - entry->d_name;
            char fname[100] = "";
            strncpy(fname, entry->d_name, len);
            char cmd[] = "main tests/";
            strcat(cmd, entry->d_name);
            strcat(cmd, " 2> tests/");
            strcat(cmd, fname);
            strcat(cmd, ".tmp");
            exec_cmd(cmd);

            char dcmd[] = "cd tests & findstr /V /G:";
            strcat(dcmd, fname);
            strcat(dcmd, ".out ");
            strcat(dcmd, fname);
            strcat(dcmd, ".tmp > ");
            strcat(dcmd, fname);
            strcat(dcmd, ".diff");
            exec_cmd(dcmd);

            char dfname[100] = "tests/";
            strcat(dfname, fname);
            strcat(dfname, ".diff");
            FILE *file = fopen(dfname, "r");
            fseek(file,0,SEEK_END);
            printf("Test Case %d [" ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET "]: ",tc,fname);
            if(ftell(file) == 0){
                fclose(file);
                printf(ANSI_COLOR_GREEN "PASSED" ANSI_COLOR_RESET "\n");
                char delcmd[100] = "cd tests & del ";
                strcat(delcmd, fname);
                strcat(delcmd, ".diff ");
                strcat(delcmd, fname);
                strcat(delcmd, ".tmp");
                exec_cmd(delcmd);
            }else{
                fclose(file);
                printf(ANSI_COLOR_RED "FAILED" ANSI_COLOR_RESET "\n");
            }
        }
    }
    closedir(dp);
    return 0;
}