#include "pointer_list.h"

PList* new_PList(){
    PList* list = calloc(1, sizeof(PList));
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
    list->size = 0;
    return list;
}

void free_PList(PList* list, bool free_data){
    PListNode* node = list->head;
    while(node){
        PListNode* next = node->next;
        if(free_data){
            free(node->data);
        }
        free(node);
        node = next;
    }
    free(list);
    return;
}

PList* append_PList(PList* list, void* data){
    PListNode* node = calloc(1, sizeof(PListNode));
    node->data = data;
    node->next = NULL;

    if(list->head == NULL){
        list->head = node;
        list->tail = node;
        list->current = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
    list->size++;
    return list;
}

void* get_head_PList(PList* list){
    if(list->head == NULL) return NULL;
    return list->head->data;
}

void* get_tail_PList(PList* list){
    if(list->tail == NULL) return NULL;
    return list->tail->data;
}

void* get_current_PList(PList* list){
    if(list->current == NULL) return NULL;
    return list->current->data;
}

void* get_next_PList(PList* list){
    if(list->current == NULL) return NULL;
    list->current = list->current->next;
    return list->current ? list->current->data : NULL;
}

int size_PList(PList* list){
    return list->size;
}

bool is_empty_PList(PList* list){
    return list->size == 0;
}

void reset_PList(PList* list){
    list->current = list->head;
}

void move_next_PList(PList* list){
    if(list->current != NULL){
        list->current = list->current->next;
    }
}
