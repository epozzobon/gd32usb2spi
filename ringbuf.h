#pragma once

#include "util.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define DATA_t uint8_t

typedef struct ringbuf {
    size_t size;
    size_t wpos;
    size_t rpos;
    DATA_t *buf;
} ringbuf_t;

void ringbuf_init(struct ringbuf *rb, DATA_t *buf, size_t size);
size_t ringbuf_count(struct ringbuf *rb);
int ringbuf_peek(struct ringbuf *rb, DATA_t *dst, size_t off, size_t len);
int ringbuf_read(struct ringbuf *rb, DATA_t *dst, size_t len);
int ringbuf_write(struct ringbuf *rb, DATA_t *src, size_t len);
bool ringbuf_full(ringbuf_t *rb);
bool ringbuf_empty(ringbuf_t *rb);
void ringbuf_write_byte(ringbuf_t *rb, DATA_t d);
DATA_t ringbuf_read_byte(ringbuf_t *rb);
DATA_t ringbuf_peek_byte(ringbuf_t *rb, size_t pos);