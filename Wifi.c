#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR_LEN 1024

void getWifiProfiles() {
    FILE *fp;
    char path[MAX_STR_LEN];
    char line[MAX_STR_LEN];

    fp = popen("netsh wlan show profiles", "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        exit(1);
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strstr(line, "All User Profile") != NULL) {
            char* profileName = strchr(line, ':');
            if (profileName != NULL) {
                profileName += 2; // Move to start of profile name
                profileName[strcspn(profileName, "\n")] = 0; // Remove newline character
                char command[MAX_STR_LEN];
                sprintf(command, "netsh wlan show profile \"%s\" key=clear", profileName);
                FILE *profile_fp = popen(command, "r");
                if (profile_fp != NULL) {
                    char profile_line[MAX_STR_LEN];
                    while (fgets(profile_line, sizeof(profile_line), profile_fp) != NULL) {
                        if (strstr(profile_line, "Key Content") != NULL) {
                            char* key = strchr(profile_line, ':');
                            if (key != NULL) {
                                key += 2; // Move to start of key
                                key[strcspn(key, "\n")] = 0; // Remove newline character
                                printf("%-30s|  %s\n", profileName, key);
                            }
                        }
                    }
                    pclose(profile_fp);
                } else {
                    printf("Failed to get profile information for %s\n", profileName);
                }
            }
        }
    }
    pclose(fp);
}

int main() {
    getWifiProfiles();
    return 0;
}
