#include<stdio.h>
#include<dirent.h>
#include<stdlib.h>



void main(){
   const char *path = "./audio/z/train";
   
   struct dirent *entry;

   DIR *dr  = opendir(path);

   char command[1024];

   if(dr == NULL){
    printf("cant access");
   }


   while((entry = readdir(dr))!= NULL){
      
      if(entry->d_type == DT_REG){
         sprintf(command, 
         "../Feature_Extraction_GDrive/Feature_Extraction_Sample/ComputeFeatures "
         "../Feature_Extraction_GDrive/Feature_Extraction_Sample/mfcc.config "
         "audio/z/train/%s "
         "frameCepstrum+frameDeltaCepstrum "
         "audio/z/train_mfcc/%s.mfcc "
         "0.06 A", 
         entry->d_name, 
         entry->d_name);

         printf("%s\n",entry->d_name);   
         int res = system(command);
         
         if(res != 0){
            printf("Error didnt work");
         }
   
        }
   }
} 

