#pragma once

#ifdef MCC
#include "mcc2_lib.h"
#else
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#endif

typedef struct PList PList;
typedef struct PListNode PListNode;

struct PListNode {
    void* data;
    PListNode* next;
};

struct PList {
    PListNode* head;
    PListNode* tail;
    PListNode* current;
    int size;
};

// リストの作成・破棄
PList* new_PList();
void free_PList(PList* list, bool free_data);

// 要素の追加
PList* append_PList(PList* list, void* data);

// 要素の取得
void* get_head_PList(PList* list);      // リストの先頭要素を取得
void* get_tail_PList(PList* list);      // リストの末尾要素を取得
void* get_current_PList(PList* list);   // 現在の要素を取得
void* get_next_PList(PList* list);      // 現在の要素の次の要素を取得

// リストの状態確認
int size_PList(PList* list);            // リストのサイズを取得
bool is_empty_PList(PList* list);       // リストが空かどうかを確認

// リストの操作
void reset_PList(PList* list);          // 現在の位置を先頭に動かす
void move_next_PList(PList* list);      // 現在の位置を次に動かす
