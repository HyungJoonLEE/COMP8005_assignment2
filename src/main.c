#include <common.h>
#include <unistd.h>

void free_heap_memory(LinkedList *user_list);

int main(int argc, char* argv[]) {
    char file_directory[30] = {0};
    char file_list[BUF_SIZE] = {0};
    int number_of_thread = 0;
    LinkedList *user_list = NULL;


    user_list = createLinkedList();
    option_init(file_directory, &number_of_thread);
    parse_command(argc, argv, file_directory, &number_of_thread, user_list);

    read_from_shadow(file_directory, file_list);
    find_user(file_list, user_list);
    compare_password_with_salt(user_list);
    displayLinkedList(user_list);
    free_heap_memory(user_list);
    deleteLinkedList(user_list);
    return 0;
}


