/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */
#ifndef RINGBUFFER_H__
#define RINGBUFFER_H__

#ifdef __cplusplus
extern "C"
{
#endif

    /* ring buffer */
    struct rt_ringbuffer
    {
        unsigned char *buffer_ptr;
        /* use the msb of the {read,write}_index as mirror bit. You can see this as
     * if the buffer adds a virtual mirror and the pointers point either to the
     * normal or to the mirrored buffer. If the write_index has the same value
     * with the read_index, but in a different mirror, the buffer is full.
     * While if the write_index and the read_index are the same and within the
     * same mirror, the buffer is empty. The ASCII art of the ringbuffer is:
     *
     *          mirror = 0                    mirror = 1
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Full
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     *  read_idx-^                   write_idx-^
     *
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Empty
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * read_idx-^ ^-write_idx
     *
     * The tradeoff is we could only use 32KiB of buffer for 16 bit of index.
     * But it should be enough for most of the cases.
     *
     * Ref: http://en.wikipedia.org/wiki/Circular_buffer#Mirroring */
        unsigned short read_mirror : 1;
        unsigned short read_index : 15;
        unsigned short write_mirror : 1;
        unsigned short write_index : 15;
        /* as we use msb of index as mirror bit, the size should be signed and
     * could only be positive. */
        unsigned short buffer_size;
    };

    enum rt_ringbuffer_state
    {
        RT_RINGBUFFER_EMPTY,
        RT_RINGBUFFER_FULL,
        /* half full is neither full nor empty */
        RT_RINGBUFFER_HALFFULL,
    };

    /**
 * RingBuffer for DeviceDriver
 *
 * Please note that the ring buffer implementation of RT-Thread
 * has no thread wait or resume feature.
 */
    void rt_ringbuffer_init(struct rt_ringbuffer *rb, unsigned char *pool,unsigned short size);
    void rt_ringbuffer_reset(struct rt_ringbuffer *rb);
    unsigned short rt_ringbuffer_put(struct rt_ringbuffer *rb, unsigned char *ptr, unsigned short length);
    unsigned short rt_ringbuffer_put_force(struct rt_ringbuffer *rb, unsigned char *ptr, unsigned short length);
    unsigned short rt_ringbuffer_putchar(struct rt_ringbuffer *rb, unsigned char ch);
    unsigned short rt_ringbuffer_putchar_force(struct rt_ringbuffer *rb, unsigned char ch);
    unsigned short rt_ringbuffer_get(struct rt_ringbuffer *rb, unsigned char *ptr, unsigned short length);
    unsigned short rt_ringbuffer_getchar(struct rt_ringbuffer *rb, unsigned char *ch);
    unsigned short rt_ringbuffer_data_len(struct rt_ringbuffer *rb);

#ifdef RT_USING_HEAP
    struct rt_ringbuffer *rt_ringbuffer_create(unsigned short length);
    void rt_ringbuffer_destroy(struct rt_ringbuffer *rb);
#endif

/** return the size of empty space in rb */
#define rt_ringbuffer_space_len(rb) ((rb)->buffer_size - rt_ringbuffer_data_len(rb))

#ifdef __cplusplus
}
#endif

#endif
