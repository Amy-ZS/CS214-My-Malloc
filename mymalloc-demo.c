#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mymalloc.h"
#define MEMLENGTH 4096

// 使用 union 来模拟堆内存池
static union {
    memory_block_t block;
    char bytes[MEMLENGTH];  // 模拟内存池（字节数组）
    double not_used;        // 用于内存对齐
} heap;

static memory_block_t *free_list = NULL;  // 空闲块链表
static int pool_initialized = 0;  // 标记内存池是否初始化

typedef struct memory_block {
    size_t size;            // 块大小（包括元数据）
    int is_free;            // 0 = 已分配, 1 = 空闲
    struct memory_block *next; // 指向下一个块
} memory_block_t;

static void initialize_free_list();  // 分离的空闲块链表初始化函数
static void leak_detector();
static void split_block(memory_block_t *block, size_t size);
static void coalesce(memory_block_t *block);
size_t align8(size_t size);
memory_block_t *find_free_block(size_t size);

// 内存分配
void *mymalloc(size_t size, char *file, int line) {
    if (!pool_initialized) {
        initialize_pool();
    }

    // 对齐大小
    size = align8(size);

    // 找到合适的空闲块
    memory_block_t *block = find_free_block(size);
    if (block == NULL) {
        // 内存池不足，返回NULL
        fprintf(stderr, "malloc: Unable to allocate %zu bytes (%s:%d)\n", size, file, line);
        return NULL;
    }

    // 如果找到的块大于请求的大小，尝试拆分
    split_block(block, size);

    block->is_free = 0;  // 标记为已分配
    return (void *)(block + 1);  // 返回指向数据区的指针
}

// 内存释放
void myfree(void *ptr, char *file, int line) {
    if (ptr == NULL) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    memory_block_t *block = (memory_block_t *)ptr - 1;  // 获取元数据块

    // 检查指针是否有效
    if (block->is_free) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    // 标记块为空闲
    block->is_free = 1;

    // 合并相邻的空闲块，防止内存碎片
    coalesce(block);

    // 检查泄漏情况
    leak_detector();
}

// 内存池初始化
static void initialize_pool() {
    initialize_free_list();  // 初始化 free_list
    pool_initialized = 1;    // 标记内存池已初始化
    atexit(leak_detector);   // 注册泄漏检测器
}

// 空闲块链表初始化
static void initialize_free_list() {
    free_list = (memory_block_t *)heap.bytes;  // 设置 free_list 为内存池的开始位置
    free_list->size = MEMLENGTH;               // 设置空闲块的大小为内存池的总大小
    free_list->is_free = 1;                    // 设置为空闲
    free_list->next = NULL;                    // 初始化为没有后续块
}

// 查找适合的空闲块
memory_block_t *find_free_block(size_t size) {
    memory_block_t *current = free_list;

    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            return current;  // 找到合适的空闲块
        }
        current = current->next;
    }

    return NULL;  // 没有合适的空闲块
}

// 拆分块，如果剩余空间大于最小块大小
static void split_block(memory_block_t *block, size_t size) {
    // 如果剩余空间大于最小块大小
    if (block->size >= size + sizeof(memory_block_t) + 8) {
        memory_block_t *new_block = (memory_block_t *)((char *)block + size + sizeof(memory_block_t));
        new_block->size = block->size - size - sizeof(memory_block_t);
        new_block->is_free = 1;
        new_block->next = block->next;

        block->size = size;
        block->next = new_block;  // 链接新的空闲块
    }
}

// 合并相邻的空闲块
static void coalesce(memory_block_t *block) {
    if (block->next && block->next->is_free) {
        block->size += block->next->size + sizeof(memory_block_t);
        block->next = block->next->next;  // 跳过已合并的块
    }
}

// 内存泄漏检测
static void leak_detector() {
    memory_block_t *current = free_list;
    size_t total_leaked = 0;
    size_t leaked_count = 0;

    while (current != NULL) {
        if (!current->is_free) {
            total_leaked += current->size;
            leaked_count++;
        }
        current = current->next;
    }

    if (leaked_count > 0) {
        fprintf(stderr, "mymalloc: %zu bytes leaked in %zu objects.\n", total_leaked, leaked_count);
    }
}

// 确保 8 字节对齐
size_t align8(size_t size) {
    return (size + 7) & ~7;  // 向上对齐到 8 的倍数
}
