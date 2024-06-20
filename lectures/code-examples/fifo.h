#include <stdlib.h>

typedef struct {
    int head;
    int tail;
    int size;
    int capacity;
    void** items;
} fifo_t;

void fifo_init(fifo_t *fifo, int cap) {
    fifo->head = 0;
    fifo->tail = 0;
    fifo->size = 0;
    fifo->capacity = cap;
    fifo->items = malloc(cap * sizeof(void*));
}

void fifo_destroy(fifo_t *fifo) {
    free(fifo->items);
}

void* fifo_head(fifo_t *fifo) {
    if (fifo->size == 0) {
        return NULL;
    }
    return fifo->items[fifo->head];
}

void *fifo_pop(fifo_t *fifo) {
    if (fifo->size == 0) {
        return NULL;
    }
    void* item = fifo->items[fifo->head];
    fifo->head = (fifo->head + 1) % fifo->capacity;
    fifo->size = fifo->size - 1;
    return item;
}

int fifo_push(fifo_t *fifo, void* item) {
    if (fifo->size == fifo->capacity) {
        return -1;
    }
    fifo->items[fifo->tail] = item;
    fifo->tail = (fifo->tail + 1) % fifo->capacity;
    fifo->size = fifo->size + 1;
    return 0;
}