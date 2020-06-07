#ifndef __EVP_H__
#define __EVP_H__


#define MCU_8BIT                    (0)
#define MCU_8BIT_MEM                (0)
#define AES_256_CBC_MODE            (1)

#if (MCU_8BIT)
#define EVP_MAX_KEY_LENGTH          (32)        /* 256-bits */
#else
#define EVP_MAX_KEY_LENGTH          (64)        /* 512-bits */
#endif
#define EVP_MAX_IV_LENGTH           (16)        /* 128-bits */
#if (MCU_8BIT)
#define EVP_MAX_BLOCK_LENGTH        (16)        /* 128-bits */
#else
#define EVP_MAX_BLOCK_LENGTH        (32)        /* 256-bits */
#endif

#define AES_MAXNR                   (14)
#define AES_BLOCK_SIZE              (16)        /* 128-bits */

#define EVP_CIPH_STREAM_CIPHER      (0x0)
#define EVP_CIPH_ECB_MODE           (0x1)
#define EVP_CIPH_CBC_MODE           (0x2)
#define EVP_CIPH_CFB_MODE           (0x3)
#define EVP_CIPH_OFB_MODE           (0x4)
#define EVP_CIPH_CTR_MODE           (0x5)
#define EVP_CIPH_GCM_MODE           (0x6)
#define EVP_CIPH_CCM_MODE           (0x7)
#define EVP_CIPH_XTS_MODE           (0x10001)
#define EVP_CIPH_WARP_MODE          (0x10002)
#define EVP_CIPH_MODE               (0xF0007)
#define EVP_CIPHER_CTX_FLAGS(e)     ((e)->cipher.flags)
#define EVP_CIPHER_CTX_IV_LENGTH(e) ((e)->cipher.iv_len)
#define EVP_CIPHER_CTX_MODE(e)      (EVP_CIPHER_CTX_FLAGS(e) & EVP_CIPH_MODE)
#define M_do_cipher(out, in, inl) cipher.do_cipher(out, in, inl)

#if (MCU_8BIT_MEM)
#define mem xdata
#define text code
#else
#define mem
#define text
#endif

#if (MCU_8BIT_MEM)
typedef unsigned long evp_u32;
typedef unsigned char evp_u8;
#else
typedef unsigned int evp_u32;
typedef unsigned char evp_u8;
#endif

#if (MCU_8BIT)
typedef char evp_int;
typedef short evp_size_t;
//typedef unsigned char evp_uint;
//typedef unsigned char evp_ulong;
#else
typedef int evp_int;
typedef int evp_size_t;
//typedef char evp_long;
//typedef long evp_long;
//typedef unsigned int evp_uint;
//typedef unsigned long evp_ulong;
#endif
typedef struct evp_cipher_st EVP_CIPHER;
typedef struct evp_cipher_ctx_st EVP_CIPHER_CTX;

struct evp_cipher_st {
    evp_int block_size;
    evp_int iv_len;
    evp_int flags;
};

struct evp_cipher_ctx_st {
    evp_int buf_len;
    unsigned char oiv[EVP_MAX_IV_LENGTH];
    unsigned char iv[EVP_MAX_IV_LENGTH];
    unsigned char buf[EVP_MAX_BLOCK_LENGTH];
    evp_int final_used;
    evp_int block_mask;
    unsigned char final[EVP_MAX_BLOCK_LENGTH];
};

void encrypt(void);
void decrypt(void);
void aes_test(void);
#endif
