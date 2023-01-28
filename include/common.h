#ifndef COMP8005_ASSIGNMENT1_COMMON_H
#define COMP8005_ASSIGNMENT1_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <crypt.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include "linkedList.h"

#define BUF_SIZE 4096
#define PASS_LEN 6
#define PASS_ARR_LEN 95
#define DEFAULT_DIR "/etc/shadow"
#define DEFAULT_SINGLE_THREAD 1

//static const char passwd_arr[] = {
//        '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j','k', 'l', 'm', 'n', 'o',
//        'p', 'q', 'r', 's', 't', 'u','v', 'w', 'x', 'y', 'z', '{', '|', '}', '~',
//        '0', '1', '2', '3','4', '5', '6', '7', '8', '9', ':', ';', '<', '=','>', '?',
//        '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I','J','K', 'L', 'M','N', 'O',
//        'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
//        ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
//        };

static const char* passwd_arr =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "1234567890~!@#+$%^&*()_`-.,"
        "{|}:;<=>?[\\]^ \"\'";



void option_init(char* file_directory, int* number_of_thread);
void parse_command(int argc, char *argv[], char* file_directory, int* number_of_thread, LinkedList* user_list);
int read_from_shadow(char* file_directory, char* file_list);
void find_user(char* file_list, LinkedList* user_list);
void save_user(char* user_info, LinkedList* user_list, int i);
void save_userinfo(char* user_info, LinkedList* user_list, int i);
void compare_password_with_salt(LinkedList *user_list);
void recursive_init(LinkedList *user_list, int user_index);
void free_heap_memory(LinkedList *user_list);
void* brute_force_crack(LinkedList *user_list, int user_index, char* str, int index, int ptr, int* flag);
void* thread_brute_force(void* arg);

void create_thread(LinkedList *user_list);
void *thread_func(void *data);
void* thread_test();






#endif //COMP8005_ASSIGNMENT1_COMMON_H
