#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<dirent.h>
#include<string.h>


#define FEAT_DIM 38

double readFile(const char *path1,const char *path2);
double dtw(double mfcc_ref[][FEAT_DIM], int n_frame_ref,
         double mfcc_test[][FEAT_DIM], int n_frame_test);
double distance(double *vect1, double *vect2, int coldim);

double readFile(const char *path1,const char *path2){
    int num_vectors_ref;
    int num_frames_ref;
    int num_vectors_test;
    int num_frames_test;

    FILE *file_1;
    FILE *file_2;

    file_1 = fopen(path1,"r");
    if(file_1 == NULL){
        printf("ERROR\n");
        return 0.0;
    }

    fscanf(file_1, "%d %d",&num_vectors_ref,&num_frames_ref);
    // printf("Num Vectors: %d\n",num_vectors_ref);
    // printf("Num Frames %d\n", num_frames_ref);

    double mfcc_ref[num_frames_ref][FEAT_DIM];
    for(int i = 0; i<num_frames_ref; i++){
        for(int j = 0; j<FEAT_DIM; j++){
            fscanf(file_1,"%lf", &mfcc_ref[i][j]);
        }
    }
    fclose(file_1);
    // printf("file 1 done\n");

    file_2 = fopen(path2, "r");
    if(file_2 == NULL){
        printf("Error\n");
        return 0.0;
    }

    fscanf(file_2, "%d %d",&num_vectors_test,&num_frames_test);
    // printf("Num Vectors: %d\n",num_vectors_test);
    // printf("Num Frames %d\n", num_frames_test); 

    double mfcc_test[num_frames_test][FEAT_DIM];
    for(int i = 0; i<num_frames_test; i++){
        for(int j = 0; j<FEAT_DIM; j++){
            fscanf(file_2,"%lf", &mfcc_test[i][j]);
        }
    }
    fclose(file_2);
    // printf("file 2 done\n");

    // printf("Calling DTW\n");
    double ans = dtw(mfcc_ref, num_frames_ref, mfcc_test, num_frames_test);
    return ans;
    // printf("Done\n");
}

double distance(double *vect1, double *vect2, int coldim) {
    double sum = 0.0;
    for (int i = 0; i < coldim; i++) {
        sum += fabs(vect1[i] - vect2[i]);
    }
    return sum;
}

double dtw(double mfcc_ref[][FEAT_DIM], int n_frame_ref,
         double mfcc_test[][FEAT_DIM], int n_frame_test){    

    // printf("Started\n");

    double **mat = (double **)malloc((n_frame_ref+1) * sizeof(double*));
    for (int i = 0; i<= n_frame_ref; i++){
        mat[i] = (double *)malloc((n_frame_test+1) * sizeof(double));
    }

    // printf("Started2\n");

    for(int i = 0; i<= n_frame_ref; i++){
        for(int j = 0; j<=n_frame_test; j++){
            mat[i][j] = INFINITY;
        }
    }

    mat[0][0] = 0;

    for (int i = 1; i <= n_frame_ref; i++){
        for (int j = 1; j<= n_frame_test; j++){
            double d = distance(mfcc_ref[i-1],mfcc_test[j-1],FEAT_DIM);
            double prev = fmin(fmin(mat[i-1][j], mat[i][j-1]), mat[i-1][j-1]);
            mat[i][j] = d + prev;
        }
    }
    
    // printf("ended\n");

    double final = mat[n_frame_ref][n_frame_test];
    // printf("%lf",final);
    return final;
    for (int i = 0; i <= n_frame_ref; i++) {
        free(mat[i]);
    }
    free(mat);
}

void top_k_elem(int k,int final[k][2]){
    for(int i = 0; i<k;i++){
        printf("%d",final[k][0]);
    }
}

int main(){

    int k = 5;
    double final[k][2];
    char path_1[1024];
    char path_2[1024];
    int count = 0 ;

    const char *path_train = "./audio/combined/train_mfcc";
    
    struct dirent *entry_train;

    DIR *dr_train = opendir(path_train);
    
    if(dr_train == NULL){
        printf("cant access dr_train");
    }

    while((entry_train = readdir(dr_train)) != NULL){
        
        if(entry_train->d_type == 4){
            continue;
        }
        
        // printf("%s\n",entry_train->d_name);
        // printf("%d\n",entry_train->d_type);

        sprintf(path_1, "%s/%s", path_train,entry_train->d_name);
        // printf("%s\n",path_1);
    
        const char *path_dev = "./audio/combined/dev_mfcc";
        struct dirent *entry_dev;
        DIR *dr_dev = opendir(path_dev);
        if(dr_dev == NULL){
            printf("cant access dr_dev");
        }

        while((entry_dev = readdir(dr_dev)) != NULL){
            
            // printf("%s\n",entry_dev->d_name);
            // printf("%d\n",entry_dev->d_type);
            if(entry_dev->d_type == 4){
                continue;
            }
            sprintf(path_2, "%s/%s", path_dev,entry_dev->d_name);
            // printf("%s\n",path_2);

            double final_ans = readFile(path_1,path_2);
            // printf("%lf\t",final_ans);
            int l = strlen(entry_dev->d_name);
            char ref_label = entry_dev->d_name[l-5];
            double num = (double)ref_label - '0';
            // printf("%lf\n", num);

            if (count < k){
                int z = count - 1;
                while (z >= 0 && final[z][0] > final_ans){
                    final[z+1][0] = final[z][0];
                    final[z+1][1] = final[z][1];
                    z--;
                }
                final[z+1][0] = final_ans;
                final[z+1][1] = num;
                count++;
            } else{
                if(final[k-1][0] > final_ans){

                    int z = k-2;

                    while (z >= 0 && final[z][0] > final_ans){
                        final[z+1][0] = final[z][0];
                        final[z+1][1] = final[z][1];
                        z--;
                    }
                    final[z+1][0] = final_ans;
                    final[z+1][1] = num;
                }
            }
        }
        
        closedir(dr_dev);



        int label_1 = 0;
        int label_4 = 0;
        int label_6 = 0;
        int label_o = 0;
        int label_z = 0;

        for(int i = 0; i<k;i++){
            int label = (int)(final[i][1]);
            printf("%lf\t",final[i][0]);
            printf("%d\n", (int)final[i][1]);
            
             

            switch(label){
                case 1:
                    label_1++;
                    break;

                case 4:
                    label_4++;
                    break;

                case 6: 
                    label_6++;
                    break;

                case 74:
                    label_z++;
                    break;

                case 63:
                    label_o++;
                    break;

                default:
                    continue;
                    break;
            }
            
            final[i][0] = INFINITY;
            final[i][1] = INFINITY;
        }   


        int max = label_1;
        int ans_audio = 1;
        char ans_audio_c = 'x'; 

        if(label_4 > max){
            max = label_4;
            ans_audio = 4;
        }

        if(label_6 > max){
            max = label_6;
            ans_audio = 6;
        }

        if(label_o > max){
            max = label_o;
            ans_audio_c = 'o';
        }

        if(label_z > max){
            max = label_z;
            ans_audio_c = 'z';
        }

        if (ans_audio_c == 'x') {
            printf("%d was voted as the most probable ref audio by %d/%d votes\n",ans_audio, max, k);
        } else {
            printf("%c was voted as the most probable ref audio by %d/%d votes\n",ans_audio_c, max, k);
        }
        
        count = 0;

        
        printf("\n\n");

    }

    closedir(dr_train);
}
