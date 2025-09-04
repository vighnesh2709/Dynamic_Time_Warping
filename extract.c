#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char *path = "./audio";

    struct dirent *entry;
    struct dirent *entry_subfolder;
    struct dirent *entry_audio;

    DIR *dr = opendir(path);

    if (dr == NULL) {
        printf("cant access\n");
        return 1;
    }

    while ((entry = readdir(dr)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0 ||
            strcmp(entry->d_name, "combined") == 0) {
            continue;
        }

        char subdir[1024];
        snprintf(subdir, sizeof(subdir), "%s/%s", path, entry->d_name);
        printf("%s\n", subdir);

        DIR *dr_subdir = opendir(subdir);
        if (dr_subdir == NULL) continue;

        while ((entry_subfolder = readdir(dr_subdir)) != NULL) {
            if (strcmp(entry_subfolder->d_name, ".") == 0 ||
                strcmp(entry_subfolder->d_name, "..") == 0 ||
                strcmp(entry_subfolder->d_name, "dev_mfcc") == 0 ||
                strcmp(entry_subfolder->d_name, "train_mfcc") == 0) {
                continue;
            }

            char *dev_or_train = entry_subfolder->d_name;
            char audio[1024];
            snprintf(audio, sizeof(audio), "%s/%s", subdir, entry_subfolder->d_name);
            printf("%s\n", audio);

            DIR *dr_audio = opendir(audio);
            if (dr_audio == NULL) continue;

            while ((entry_audio = readdir(dr_audio)) != NULL) {

                if (strcmp(entry_audio->d_name, ".") == 0 ||
                    strcmp(entry_audio->d_name, "..") == 0) {
                    continue;
                }

                char *base_path;
               
                if (strcmp(dev_or_train,"train") == 0){
                  base_path = "./audio/combined/train_mfcc";
                }else if (strcmp(dev_or_train,"dev") == 0){
                  base_path = "./audio/combined/dev_mfcc";
                }else{
                  printf("ERROR");
                }

                char final_path_1[1024];
                snprintf(final_path_1,sizeof(final_path_1),"%s/%s", audio,entry_audio->d_name);
                printf("%s\n",final_path_1);

                char final_path_2[1024];
                snprintf(final_path_2,sizeof(final_path_2),"%s/%s",base_path,entry_audio->d_name);
                printf("%s\n",final_path_2);

                if (entry_audio->d_type == DT_REG) {
                    char command[3000];
                    sprintf(command,
                        "../Feature_Extraction_GDrive/Feature_Extraction_Sample/ComputeFeatures "
                        "../Feature_Extraction_GDrive/Feature_Extraction_Sample/mfcc.config "
                        "%s "
                        "frameCepstrum+frameDeltaCepstrum "
                        "%s "
                        "0.06 A",
                     final_path_1,
                     final_path_2);

                    printf("%s\n", entry_audio->d_name);
                    int res = system(command);

                    if (res != 0) {
                        printf("Error didnt work\n");
                    }
                }
            }
            closedir(dr_audio);
        }
        closedir(dr_subdir);
    }
    closedir(dr);

    return 0;
}
