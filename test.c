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

int main(int argc, char* argv[]) {
    // run on custom tests
    if(argc == 1) {
        struct dirent *entry;
        DIR *dp = opendir("tests");

        if (dp == NULL) {
            perror("opendir");
            return 1;
        }
        int tc = 0;
        short failed = 0;
        while ((entry = readdir(dp))) {
            if (strstr(entry->d_name, ".bin")) {
                tc++;
                int len = strstr(entry->d_name, ".bin") - entry->d_name;
                char fname[100] = "";
                strncpy(fname, entry->d_name, len);
                char cmd[1000] = "main tests/";
                strcat(cmd, entry->d_name);
                strcat(cmd, " 2> tests/");
                strcat(cmd, fname);
                strcat(cmd, ".tmp");
                exec_cmd(cmd);

                char dcmd[1000] = "cd tests & findstr /V /G:";
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
                fseek(file, 0, SEEK_END);
                printf("Test Case %d [" ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET "]: ", tc, fname);
                if (ftell(file) == 0) {
                    fclose(file);
                    printf(ANSI_COLOR_GREEN "PASSED" ANSI_COLOR_RESET "\n");
                    char delcmd[1000] = "cd tests & del ";
                    strcat(delcmd, fname);
                    strcat(delcmd, ".diff ");
                    strcat(delcmd, fname);
                    strcat(delcmd, ".tmp");
                    exec_cmd(delcmd);
                } else {
                    fclose(file);
                    printf(ANSI_COLOR_RED "FAILED" ANSI_COLOR_RESET "\n");
                    failed = 1;
                }
            }
        }
        closedir(dp);
        if (failed) {
            return 1;
        }
    }
    // run on riscv-tests from environment argv[1]
    else if(argc == 2) {
        struct dirent *entry;
        char dir[100] = "tests/";
        strcat(dir, argv[1]);
        strcat(dir, "_tests");
        DIR *dp = opendir(dir);

        if (dp == NULL) {
            perror("opendir");
            return 1;
        }
        int tc = 0;
        short failed = 0;
        while ((entry = readdir(dp))) {
            if (strstr(entry->d_name, ".bin")) {
                tc++;
                int len = strstr(entry->d_name, ".bin") - entry->d_name;
                char fname[100] = "";
                strncpy(fname, entry->d_name, len);
                char cmd[1000] = "main ";
                strcat(cmd, dir);
                strcat(cmd, "/");
                strcat(cmd, entry->d_name);
                strcat(cmd, " 2> ");
                strcat(cmd, dir);
                strcat(cmd, "/");
                strcat(cmd, fname);
                strcat(cmd, ".tmp");
                exec_cmd(cmd);

                char file_path[1000] = "";
                strcat(file_path, dir);
                strcat(file_path, "/");
                strcat(file_path, fname);
                strcat(file_path, ".tmp");
                FILE *file = fopen(file_path, "r");  // Open the file in read mode

                char line[256];
                int gp = -1, a0 = -1, a7 = -1, pass = 1;  // Initialize a0_value to an invalid number

                // Read the file line by line
                while (fgets(line, sizeof(line), file)) {
                    if(strstr(line, "x3 (gp)"))
                        sscanf(line, "x3 (gp) = %d", &gp);
                    else if (strstr(line, "x10 (a0)"))
                        sscanf(line, "x10 (a0) = %d", &a0);  // Extract the value of a0
                    else if(strstr(line, "x17 (a7)"))
                        sscanf(line, "x17 (a7) = %d", &a7);
                    else if(strstr(line, "ERROR: "))
                        pass = 0;
                }
                fclose(file);

                printf("Test Case %d [" ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET "]: ", tc, fname);
                if (strstr(entry->d_name, "-p-") && gp == 1 && a0 == 0 && a7 == 93 && pass) {
                    printf(ANSI_COLOR_GREEN "PASSED" ANSI_COLOR_RESET "\n");
                    char delcmd[1000] = "cd ";
                    strcat(delcmd, dir);
                    strcat(delcmd, " & del ");
                    strcat(delcmd, fname);
                    strcat(delcmd, ".tmp");
                    exec_cmd(delcmd);
                }
                else {
                    printf(ANSI_COLOR_RED "FAILED" ANSI_COLOR_RESET "\n");
                    failed = 1;
                }
            }
        }
        closedir(dp);
        if (failed) {
            return 1;
        }
    }
    return 0;
}