#ifndef _TYPES_H
#define _TYPES_H

#ifndef __GNUC__
#define __asm__         asm
#define __typeof__      typeof
#define __inline__      inline
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef unsigned char bool;

typedef int ret_code;

/**
 *linux/include/linux/types.h 
 */
// struct list_head {
//     struct list_head *next, *prev;
// };

// struct hlist_head {
//     struct hlist_node *first;
// };

// struct hlist_node {
//     struct hlist_node *next, **pprev;
// };

/**
 *linux/include/linux/stddef.h 
 */
#define NULL        ((void *)0)

enum {
        false = 0,
        true  = 1
};

#define _offsetof(TYPE, MEMBER)  ((size_t)&((TYPE *)0)->MEMBER)

/**
 * offsetofend(TYPE, MEMBER)
 *
 * @TYPE: The type of the structure
 * @MEMBER: The member within the structure to get the end offset of
 */
#define _offsetend(TYPE, MEMBER) \
    (_offsetof(TYPE, MEMBER) + sizeof(((TYPE *)0)->MEMBER))

/** 
 * linux/include/linux/kernel.h
 *
 * _container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 *
 */
#define _container_of(ptr, type, member) ({ \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - _offsetof(type, member) );})

#define U32_SHIFT 32
#define U32_MASK ((1ULL << U32_SHIFT) - 1)

#define U16_SHIFT 16
#define U16_MASK ((1 << U16_SHIFT) - 1)

#define DWORD_SHIFT 32
#define DWORD_MASK ((1ULL << DWORD_SHIFT) - 1)

#define WORD_SHIFT 16
#define WORD_MASK ((1 << WORD_SHIFT) - 1)


#endif  /* ~ _TYPES_H */
