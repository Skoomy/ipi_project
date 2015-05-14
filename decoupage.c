#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum bool bool;
enum bool {
  false, true
};

bool value_is_in_array(int val, int* arr, int size){
  int i;
  bool res = false;
  for (i = 0; i < size; i++){
    if(arr[i] == val){
      res = true;
    }
  }
  return res;
}

void traitement_supp_doublon(char* file_name){
  
  FILE* file = fopen(file_name, "r");
  
  FILE* file_to_write = fopen("new_file", "w+");
  
  char date_debut[255];
  char date_fin[255];
  char lieu[25];
  
  char date_debut_inter[255];
  char date_fin_inter[255];
  char lieu_inter[25];
  
  /* On saute la première ligne */
  char c;
  do
  c = fgetc(file);
  while (c != '\n');
  
  fscanf(file, "%s %s %s", date_debut, date_fin, lieu);
  strcpy(date_debut_inter, date_debut);
  strcpy(date_fin_inter, date_fin);
  strcpy(lieu_inter, lieu);
  
  while (fscanf(file, "%s %s %s", date_debut, date_fin, lieu) != EOF){
    
    if(strcmp(lieu,lieu_inter) != 0){
      fprintf(file_to_write, "%s %s %s\n", date_debut_inter, date_fin_inter, lieu_inter);
      strcpy(date_debut_inter, date_debut);
      strcpy(lieu_inter, lieu);
      strcpy(date_fin_inter, date_fin);
    }else {
      strcpy(date_fin_inter, date_fin);
    }
    
  }
  
  fclose(file);
  fclose(file_to_write);
}

/**
 * De mi-novembre à mi-décembre (on prend depuis début novembre)
 */
void incubation(char* file_name){
  
   FILE* file = fopen(file_name, "r");
   
   if(file == NULL){
      printf("Impossible d'ouvrir le fichier %s\n", file_name);
      exit(1);
  }
  
    FILE* file_to_write = fopen("new_file2", "w+");
    
    if(file_to_write == NULL){
      printf("Impossible de créer le fichier %s\n", "new_file");
      exit(1);
    }
  
  char date_debut[255];
  char date_fin[255];
  char lieu[25];
  
  char date_debut_copy[255];
  char *token;
  char *search = "-";
  
  char* year;
  char* month;
  char* day;
  
  int allow_month[2] = {11,12}; //mois de novembre et décembre
  
  int year_int;
  int month_int;
  int day_int;
  
  /* On saute la première ligne */
  char c;
  do
  c = fgetc(file);
  while (c != '\n');
  
  char* year_month_day;
  int current_year = 0;
  
  fprintf(file_to_write, "INCUBATION - périodes habituellement observées / mi-Novembre_mi-Decembre\n\n");
  
  fscanf(file, "%s %s %s", date_debut, date_fin, lieu); //exemple_type : 1997-12-26_21:47:40 1997-12-26_21:48:11 Terre
  //date_debut : 1997-12-26_21:47:40 ; date_fin : 1997-12-26_21:48:11 ; lieu : Terre
      
      search = "_";
      strcpy(date_debut_copy, date_debut);
      year_month_day = strtok(date_debut_copy, search); //1997-12-26
      search = "-";
    
      year = strtok (year_month_day, search); //1997
      printf("%s\n", year);
      month = strtok (NULL, search); //12
      printf("%s\n", month);
      day = strtok (NULL, search); //26
      printf("%s\n", day);
      
     if(value_is_in_array(atoi(month), allow_month, 2) == true){
	fprintf(file_to_write, "%s %s %s\n", date_debut, date_fin, lieu);
      }
      
      current_year = atoi(year);
  
  
  while (fscanf(file, "%s %s %s", date_debut, date_fin, lieu) != EOF){
      
      search = "_";
      strcpy(date_debut_copy, date_debut);
      year_month_day = strtok(date_debut_copy, search);
      search = "-";
    
      year = strtok (year_month_day, search);
      month = strtok (NULL, search);
      day = strtok (NULL, search);
      
      if(atoi(year) != current_year){
	fprintf(file_to_write, "\n\n");
      }
      
     if(value_is_in_array(atoi(month), allow_month, 2) == true){
	fprintf(file_to_write, "%s %s %s\n", date_debut, date_fin, lieu);
      }
      
      current_year = atoi(year);
  }
}

void decoupage(char* file_name){
  
  char date_debut[255];
  char date_fin[255];
  char lieu[25];
  
  FILE* file = fopen(file_name, "r");
  
  FILE* file_to_write = fopen("new_file", "w+");
  
  if(file == NULL){
      printf("Impossible d'ouvrir le fichier %s\n", file_name);
      exit(1);
  }
  /* On saute la première ligne */
  char c;
  do
  c = fgetc(file);
  while (c != '\n');
  
  while (fscanf(file, "%s %s %s", date_debut, date_fin, lieu) != EOF){
    
    fscanf(file_to_write, "%s %s\n", date_debut, lieu);
  }
  
  fclose(file);
  fclose(file_to_write);
}

int main(void){
  
  //incubation("00011n.txt");
  traitement_supp_doublon("00011n.txt");
  
  return 0;
}