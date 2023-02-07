#include <common.h>
#include <unistd.h>


int main(int argc, char* argv[]) {
    char file_directory[30] = {0};
    char file_list[BUF_SIZE] = {0};
    LinkedList *user_list = NULL;
    struct timespec start, finish;
    double time;

    putenv("OMP_CANCELLATION=true");
    user_list = createLinkedList();
    option_init(file_directory, user_list);
    parse_command(argc, argv, file_directory, user_list);
    read_from_shadow(file_directory, file_list);
    find_user(file_list, user_list);
    compare_password_with_salt(user_list);

    for (int u = 0; u < user_list->currentElementCount; u++) {
        clock_gettime(CLOCK_MONOTONIC, &start);


#pragma omp parallel num_threads(user_list->num_thread)
        {
            for (int i = 0; i < PASS_LEN + 1; ++i) {
                int ptr1[i], ptr2[i];
                for (int j = 0; j < i; j++)
                    ptr1[j] = ptr2[j] = 0;

                #pragma omp for schedule(dynamic)
                for (int k = 0; k < PASS_ARR_LEN; k++) {
                    if (strlen(getLLElement(user_list, u)->password) > 0) {
                        #pragma omp cancel for
                        continue;
                    }
                    else {
                        ptr1[0] = k;
                        ptr2[0] = k + 1;
                        password_generator(ptr1, ptr2, i, user_list, u);
                    }
                #pragma omp cancellation point for
                }
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &finish);

        time = (finish.tv_sec - start.tv_sec);
        time += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

        getLLElement(user_list, u)->time = time;
    }

    displayLinkedList(user_list);
    free_heap_memory(user_list);
    deleteLinkedList(user_list);
    return 0;
}





