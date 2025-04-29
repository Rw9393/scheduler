#include <stdio.h>
#include <stdlib.h>
//#include <windows.h>
#include <time.h>
#include <string.h>
#include "cJSON.h"

#define TIME_BUFFER_SIZE 100
#define DECORATION_TEXT "---------------"
char time_string[TIME_BUFFER_SIZE] = "";

void get_current_time(void);
cJSON* read_json_file(const char *filename);
void show_list(const cJSON *data);

int main(int argc, char *argv[]) 
{
  //SetConsoleOutputCP(65001);
  get_current_time();
  cJSON *data = read_json_file("data.json");
  if (data == NULL) {
    return 1;
  }
  
  if (argc == 1 || strcmp(argv[1], "list") == 0) {
    show_list(data);
  } else if (argc == 2) {
    if (strcmp(argv[1], "add") == 0) {
      printf("add\n");
    } else if (strcmp(argv[1], "del") == 0) {
      printf("delete\n");
    } else {
      fprintf(stderr, "invalid\n");
    }
  }
  return 0;
}


void get_current_time(void)
{
  time_t current_time = time(NULL);
  struct tm *local_time = localtime(&current_time);
  strftime(time_string, TIME_BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", local_time);
}

cJSON* read_json_file(const char *filename) {
  // Open the json file 
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    fprintf(stderr, "Cannot open the file: %s\n", filename);
    return NULL;
  }

  // Get the file size
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);

  // Allocate memory
  char *json_data = (char*)malloc(file_size + 1);
  if (json_data == NULL) {
    fclose(file);
    fprintf(stderr, "Cannot allocate memory\n");
    return NULL;
  }

  // Read the file
  size_t read_size = fread(json_data, 1, file_size, file);
  fclose(file);

  if (read_size != file_size) {
    free(json_data);
    fprintf(stderr, "Cannot read the file\n");
    return NULL;
  }

  json_data[file_size] = '\0';

  cJSON *json = cJSON_Parse(json_data);
  free(json_data);

  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr != NULL) {
      fprintf(stderr, "Cannot parse json: %s\n", error_ptr);
    }
    return NULL;
  }
  return json;
}


void show_list(const cJSON *data)
{
  printf("%sPlans%s\n", DECORATION_TEXT, DECORATION_TEXT);

  char partition_text[50];
  strcpy(partition_text, DECORATION_TEXT);
  strcat(partition_text, DECORATION_TEXT);
  strcat(partition_text, "-----");

  if (!cJSON_IsArray(data)) {
    fprintf(stderr, "This data isn't array\n");
    return;
  }

  int array_size = cJSON_GetArraySize(data);

  for (int n = 0; n < array_size; n++) {
    cJSON *datum = cJSON_GetArrayItem(data, n);
    
    cJSON *id = cJSON_GetObjectItem(datum, "id");
    if (cJSON_IsNumber(id)) {
      printf("Id:    %d\n", id->valueint);
    }
    
    cJSON *name = cJSON_GetObjectItem(datum, "name");
    if (cJSON_IsString(name) && name->valuestring != NULL) {
      printf("Name:  %s\n", name->valuestring);
    }
    
    cJSON *date = cJSON_GetObjectItem(datum, "date");
    
    cJSON *begin = cJSON_GetObjectItem(date, "begin");
    if (cJSON_IsString(begin) && begin->valuestring != NULL) {
      printf("Start: %s\n", begin->valuestring);
    }
    
    cJSON *end = cJSON_GetObjectItem(date, "end");
    if (cJSON_IsString(end) && end->valuestring != NULL) {
      printf("End:   %s\n", end->valuestring);
    }

    printf("%s\n", partition_text);
  }
}
