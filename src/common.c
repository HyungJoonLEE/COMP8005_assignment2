#include <assert.h>
#include "common.h"
#include "error.h"

pthread_mutex_t mutex;
omp_lock_t semaphore;
int occupied = 100;

void option_init(char* file_directory, int* number_of_thread) {
    strcpy(file_directory, DEFAULT_DIR);
    *number_of_thread = DEFAULT_SINGLE_THREAD;
}

void parse_command(int argc, char *argv[], char* file_directory, int* number_of_thread, LinkedList* user_list) {
    int c;

    while((c = getopt(argc, argv, ":f:t:")) != -1) {
        switch(c) {
            case 'f': {
                strcpy(file_directory, optarg);
                break;
            }
            case 't': {
                *number_of_thread = atoi(optarg);
                user_list->num_thread = *number_of_thread;
                break;
            }
            case ':': {
                fatal_message(__FILE__, __func__, __LINE__, "\"Options requires an operand\"", 5);
                break;
            }
            case '?': {
                fatal_message(__FILE__, __func__, __LINE__, "Unknown", 6);
                break;
            }
            default: {
                assert("Should not get here");
            };
        }
    }

    if (optind < argc) {
        int i = 0;
        while(argv[optind + i] != NULL) {
            ListNode user = {0,};
            strcpy(user.id, argv[optind + i]);
            addLLElement(user_list, i, user);
            i++;
        }
    }
}


int read_from_shadow(char* file_directory, char* file_list) {
    char buffer[BUF_SIZE];
    char sudo_string[70] = {0};
    FILE *fp;

    sprintf(sudo_string, "sudo -S cat %s", file_directory);
    memset(buffer, 0 ,BUF_SIZE);
    fp = popen(sudo_string, "r");
    if (fp == NULL) {
        perror("popen() failed\n");
        return -1;
    }

    while (fgets(buffer, BUF_SIZE, fp)) {
        strcat(file_list, buffer);
    }
    pclose(fp);

    // printf("%s", file_list);
    return 0;
}


void find_user(char* file_list, LinkedList* user_list) {
    char temp[BUF_SIZE] = {0};

    strcpy(temp, file_list);
    memset(file_list, 0, BUF_SIZE);
    for (int i = 0; i < user_list->currentElementCount; i++) {
        strcpy(file_list, temp);
        char* line = strtok(file_list, "\n");
        while (line != NULL) {
            if (strncmp(line, getLLElement(user_list, i)->id, strlen(getLLElement(user_list, i)->id)) == 0) {
                printf("%s\n", line);
                save_user(line, user_list, i);
                break;
            }
            line = strtok(NULL, "\n");
        }
    }
}


void save_user(char* user_info, LinkedList* user_list, int i) {
    char temp[256] = {0};
    char without_date[256] = {0};
    char* user_info_var = NULL;

    strcpy(temp, user_info);
    user_info += strlen(getLLElement(user_list, i)->id) + 1;
    strcpy(without_date, user_info);
    strtok(without_date, ":");
    strcpy(getLLElement(user_list, i)->original, without_date);

    user_info_var = strtok(user_info, "$");
    if (strcmp(user_info_var, "y") == 0) {
        strcpy(getLLElement(user_list, i)->hash_id, "y");
        strcpy(getLLElement(user_list, i)->hash_type, "yescrypt");
        user_info_var = strtok(NULL, "$");
        strcpy(getLLElement(user_list, i)->crypt_parm, user_info_var);
    }

    if (strcmp(user_info_var, "1") == 0) {
        strcpy(getLLElement(user_list, i)->hash_id, "1");
        strcpy(getLLElement(user_list, i)->hash_type, "MD5");
    }
    if (strcmp(user_info_var, "5") == 0) {
        strcpy(getLLElement(user_list, i)->hash_id, "5");
        strcpy(getLLElement(user_list, i)->hash_type, "SHA-256");
    }
    if (strcmp(user_info_var, "6") == 0) {
        strcpy(getLLElement(user_list, i)->hash_id, "6");
        strcpy(getLLElement(user_list, i)->hash_type, "SHA-512");
    }

    save_userinfo(temp, user_list, i);
}


void save_userinfo(char* user_info, LinkedList* user_list, int i) {
    char* user_info_var;
    user_info += strlen(getLLElement(user_list, i)->id) + strlen(getLLElement(user_list, i)->hash_id);

    if (strcmp(getLLElement(user_list, i)->hash_id, "y") == 0)
        user_info += strlen(getLLElement(user_list, i)->crypt_parm) + 4;
    else
        user_info += 3;

    user_info_var = strtok(user_info, "$");
    strcpy(getLLElement(user_list, i)->salt, user_info_var);
    user_info_var = strtok(NULL, ":");
    strcpy(getLLElement(user_list, i)->hash_value, user_info_var);
}


void compare_password_with_salt(LinkedList *user_list) {
    char salt_setting[200] = {0};
    clock_t start, end;
    float time;
    int thread_id;
    for (int i = 0; i < user_list->currentElementCount; i++) {
        if (strcmp(getLLElement(user_list, i)->hash_id, "y") == 0)
            sprintf(salt_setting, "$%s$%s$%s",
                    getLLElement(user_list, i)->hash_id,
                    getLLElement(user_list, i)->crypt_parm,
                    getLLElement(user_list, i)->salt);
        else
            sprintf(salt_setting, "$%s$%s",
                    getLLElement(user_list, i)->hash_id,
                    getLLElement(user_list, i)->salt);

        strcpy(getLLElement(user_list, i)->salt_setting, salt_setting);

        pthread_mutex_init(&mutex, NULL);
        start = clock();
        recursive_init(user_list, PASS_LEN, i);
        end = clock();

        time = (float) (end - start) / CLOCKS_PER_SEC;
        getLLElement(user_list, i)->time = time;
    }
}

void recursive_init(LinkedList *user_list, int passwd_len, int user_index) {

    char* buf = malloc(passwd_len + 1);
    int flag = 0;
    #pragma omp parallel
    for (int i = 1; i <= passwd_len; i++) {
        memset(buf, 0, passwd_len + 1);
        brute_force_crack(user_list, user_index, buf, 0, i,  &flag);
        if (flag == 1) break;
    }

    free(buf);
}


void brute_force_crack(LinkedList *user_list, int user_index, char* str, int index, int ptr, int* flag) {
    if (*flag == 1) return;
    #pragma omp critical
    for (int i = 0; i < PASS_ARR_LEN; ++i) {
        str[index] = passwd_arr[i];
        if (index == ptr - 1) {
//            pthread_mutex_lock(&mutex);
            if (strcmp(crypt(str, getLLElement(user_list, user_index)->salt_setting),
                       getLLElement(user_list, user_index)->original) == 0) {
                strcpy(getLLElement(user_list, user_index)->password, str);
                printf("[thread %d]: %s (HIT)\n", omp_get_thread_num(), str);
                *flag = 1;
            }
//            pthread_mutex_unlock(&mutex);
            printf("[thread %d]: %s (MISS)\n", omp_get_thread_num(), str);
            pthread_mutex_lock(&mutex);
            getLLElement(user_list, user_index)->count++;
//            pthread_mutex_unlock(&mutex);
        } else {
            brute_force_crack(user_list, index + 1, str, ptr, user_index, flag);
        }
    }
}



void free_heap_memory(LinkedList *user_list) {
    for (int i = 0; i < getLinkedListLength(user_list); i++) {
        removeLLElement(user_list, i);
    }
}

