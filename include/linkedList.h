#ifndef PROJECT_LINKEDLIST_H
#define PROJECT_LINKEDLIST_H

typedef struct ListNodeType {
    char id[32];
    char hash_id[4];
    char crypt_parm[10];
    char hash_type[15];
    char salt[64];
    char salt_setting[128];
    char hash_value[128];
    char password[10];
    char original[256];
    int count;
    float time;
    struct ListNodeType *pLink;
}ListNode;


typedef struct ThreadNodeType {
    int start;
    int end;
    int* id; //TODO: need to FREE
    struct ThreadNodeType *pLink;
}ThreadNode;

typedef struct LinkedListType {
    int currentElementCount;
    int currentThreadCount;
    int num_thread;
    ListNode headerNode;
    ThreadNode threadNode;
}LinkedList;




LinkedList* createLinkedList(void);
int addLLElement(LinkedList *pList, int position, ListNode element);
int addTLElement(LinkedList *pList, int position, ThreadNode element);
int removeLLElement (LinkedList *pList, int position);
ListNode* getLLElement(LinkedList *pList, int position);

void clearLinkedList(LinkedList *pList);
int getLinkedListLength(LinkedList *pList);
void deleteLinkedList(LinkedList *pList);
void displayLinkedList(LinkedList* pList);

int addTLElement(LinkedList* pList, int position, ThreadNode element);
ThreadNode* getTLElement(LinkedList* pList, int position);

#define FALSE 0
#define TRUE 1

#endif //PROJECT_LINKEDLIST_H
