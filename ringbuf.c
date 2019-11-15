#include "ringbuf.h"


void ringbuf_init(ringbuf_t *rb, DATA_t *buf, size_t size) {
    rb->buf = buf;
    rb->size = size;
    rb->wpos = 0;
    rb->rpos = 0;
}


size_t ringbuf_count(ringbuf_t *rb) {
    size_t size = rb->size;
    size_t rpos = rb->rpos;
    size_t wpos = rb->wpos;
    return (size + wpos - rpos) % size;
}


int ringbuf_peek(ringbuf_t *rb, DATA_t *dst, size_t off, size_t len) {
    size_t size = rb->size;
    size_t pos = rb->rpos;
    size_t count = ringbuf_count(rb);
    if (count < len) {
        dbg_printf("ringbuf underflow\n");
        return -1;
    }
    pos += off;
    for (size_t i = 0; i < len; i++) {
        pos %= size;
        dst[i] = rb->buf[pos++];
    }
    return len;
}


int ringbuf_read(ringbuf_t *rb, DATA_t *dst, size_t len) {
    int amount = ringbuf_peek(rb, dst, 0, len);
    if (amount > 0) {
        size_t pos = rb->rpos + amount;
        pos %= rb->size;
        rb->rpos = pos;
    }
    return amount;
}


int ringbuf_write(ringbuf_t *rb, DATA_t *src, size_t len) {
    size_t size = rb->size;
    size_t space = size - ringbuf_count(rb) - 1;
    size_t wpos = rb->wpos;
    if (space < len) {
        dbg_printf("ringbuf overflow\n");
        return -1;
    }
    for (size_t i = 0; i < len; i++) {
        DATA_t byte = src[i];
        rb->buf[wpos++] = byte;
        wpos %= size;
    }
    if (wpos < rb->wpos) {
        dbg_printf("ringbuf wrap\n");
    }
    rb->wpos = wpos;
    return len;
}


DATA_t ringbuf_peek_byte(ringbuf_t *rb, size_t pos) {
    size_t size = rb->size;
    size_t rpos = rb->rpos;

    DATA_t d = rb->buf[(pos + rpos) % size];
    return d;
}


DATA_t ringbuf_read_byte(ringbuf_t *rb) {
    size_t size = rb->size;
    size_t rpos = rb->rpos;

    DATA_t d = rb->buf[rpos];
    rb->rpos = (rpos + 1) % size;
    return d;
}


void ringbuf_write_byte(ringbuf_t *rb, DATA_t d) {
    size_t size = rb->size;
    size_t wpos = rb->wpos;

    rb->buf[wpos] = d;
    rb->wpos = (wpos + 1) % size;
}


bool ringbuf_empty(ringbuf_t *rb) {
    size_t rpos = rb->rpos;
    size_t wpos = rb->wpos;

    return rpos == wpos;
}


bool ringbuf_full(ringbuf_t *rb) {
    size_t size = rb->size;
    size_t rpos = rb->rpos;
    size_t wpos = rb->wpos;

    return (wpos - rpos + size) % size > size - 1;
}
