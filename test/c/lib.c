
#include "testinc.h"
#include "pointer_list.h"
int abcbalkdsjfal;


int test_lib(){
    printf("test of library..\n");

    PList* list = new_PList();

    int data1 = 10;
    int data2 = 20;
    int data3 = 30;

    ASSERT(is_empty_PList(list), true);
    ASSERT(size_PList(list), 0);
    ASSERT(get_head_PList(list), NULL);
    ASSERT(get_tail_PList(list), NULL);
    ASSERT(get_current_PList(list), NULL);
    ASSERT(get_next_PList(list), NULL);

    append_PList(list, &data1);
    append_PList(list, &data2);
    append_PList(list, &data3);

    ASSERT(is_empty_PList(list), false);

    ASSERT(size_PList(list), 3);
    ASSERT(*(int*)get_head_PList(list), 10);
    ASSERT(*(int*)get_tail_PList(list), 30);
    ASSERT(*(int*)get_current_PList(list), 10);
    ASSERT(*(int*)get_next_PList(list), 20);
    ASSERT(*(int*)get_current_PList(list), 20);
    ASSERT(*(int*)get_next_PList(list), 30);
    ASSERT(*(int*)get_current_PList(list), 30);

    reset_PList(list);
    ASSERT(*(int*)get_current_PList(list), 10);
    move_next_PList(list);
    ASSERT(*(int*)get_current_PList(list), 20);
    move_next_PList(list);
    ASSERT(*(int*)get_current_PList(list), 30);
    move_next_PList(list);
    ASSERT(get_current_PList(list), NULL);

    free_PList(list, false);

    return 0;
}
