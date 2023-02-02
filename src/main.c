#include <common.h>
#include <unistd.h>
#include <stdbool.h>


int main(int argc, char* argv[]) {
    char file_directory[30] = {0};
    char file_list[BUF_SIZE] = {0};
    int number_of_thread = 0;
    LinkedList *user_list = NULL;
    int pass_arr_len = (int)strlen(passwd_arr);
    int password_length = PASS_LEN;
    clock_t start, end;
    float time;

    user_list = createLinkedList();
    option_init(file_directory, &number_of_thread);
    parse_command(argc, argv, file_directory, &number_of_thread, user_list);
    read_from_shadow(file_directory, file_list);
    find_user(file_list, user_list);
    compare_password_with_salt(user_list);
    for (int u = 0; u < user_list->currentElementCount; u++) {
        start = clock();
        bool found = FALSE;

#pragma omp parallel shared(password_length, pass_arr_len, found) num_threads(user_list->num_thread)
        {
            for (int i = 0; i < PASS_LEN + 1; ++i) {
                char ptr1[i], ptr2[i];
                for (int j = 0; j < i; j++)
                    ptr1[j] = ptr2[j] = 0;
#pragma omp for schedule(dynamic)
                for (int k = 0; k < PASS_ARR_LEN; k++) {
                    if (getLLElement(user_list, u)->flag == TRUE) {
                        #pragma omp cancel for
                    }
                    else {
                        ptr1[0] = k;
                        ptr2[0] = k + 1;
                        recursive(ptr1, ptr2, passwd_arr, PASS_ARR_LEN, i, user_list, u);
                    }
                }
            }
        }
        end = clock();
        time = (float) (end - start) / CLOCKS_PER_SEC;
        getLLElement(user_list, u)->time = time;
    }

    displayLinkedList(user_list);
    free_heap_memory(user_list);
    deleteLinkedList(user_list);
    return 0;
}





