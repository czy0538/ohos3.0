/*
 * Copyright (C) 2021 HiSilicon (Shanghai) Technologies CO., LIMITED.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __DRV_SYMC_H__
#define __DRV_SYMC_H__

#include "hi_types.h"
#include "drv_cipher_kapi.h"
#include "drv_osal_lib.h"

/* symmetric cipher max key size in words */
#define SYMC_KEY_MAX_SIZE_IN_WORD      8

/* symmetric cipher max iv size in word */
#define SYMC_IV_MAX_SIZE_IN_WORD       4

/* symmetric sm1 sk size in words */
#define SYMC_SM1_SK_SIZE_IN_WORD       4

/* DES BLOCK size */
#define DES_BLOCK_SIZE                 8

/* Numbers of nodes list */
#define SYMC_MAX_LIST_NUM              16

/* Length of CCM N */
#define SYMC_CCM_N_LEN                 16

/* Length of CCM A head */
#define SYMC_CCM_A_HEAD_LEN            16

/* Length of GCM CLEN */
#define SYMC_GCM_CLEN_LEN              16

/* Small length of CCM A  */
#define SYMC_CCM_A_SMALL_LEN           (0x10000 - 0x100)

/* If 2^16 - 2^8 <= a < 2^32, then a  is encoded as 0xff || 0xfe || a[0..31], i.e., six octets. */
#define SYMC_AAD_PAD_VAL_FF            0xFF
#define SYMC_AAD_PAD_VAL_FE            0xFE

/* AES KEY size 128bit */
#define AES_KEY_128BIT                 16

/* AES KEY size 192bit */
#define AES_KEY_192BIT                 24

/* AES KEY size 256bit */
#define AES_KEY_256BIT                 32

/* DES KEY size 128bit */
#define DES_KEY_SIZE                   8

/* TDES KEY size 128bit */
#define TDES_KEY_128BIT                16

/* TDES KEY size 192bit */
#define TDES_KEY_192BIT                24

/* DES block size */
#define DES_BLOCK_SIZE                 8

/* SM1 KEY size */
#define SM1_AK_EK_SIZE                 32
#define SM1_SK_SIZE                    16

/* SM4 KEY size */
#define SM4_KEY_SIZE                   16

/* symmetric cipher width */
typedef enum {
    SYMC_DAT_WIDTH_128 = 0,
    SYMC_DAT_WIDTH_8,
    SYMC_DAT_WIDTH_1,
    SYMC_DAT_WIDTH_64 = 3,
    SYMC_DAT_WIDTH_COUNT,
} symc_width;

/* symmetric cipher algorithm */
typedef enum {
    SYMC_ALG_DES,         /* Data Encryption Standard  */
    SYMC_ALG_TDES,        /* Triple Data Encryption Standard  */
    SYMC_ALG_AES,         /* Advanced Encryption Standard  */
    SYMC_ALG_SM4,         /* SM4 Algorithm  */
    SYMC_ALG_SM1,         /* SM1 Algorithm  */
    SYMC_ALG_NULL_CIPHER, /* null cipher, dma copy  */
    SYMC_ALG_COUNT,
} symc_alg;

/* symmetric cipher key length */
typedef enum {
    SYMC_KEY_DEFAULT = 0,       /* Default, aes-128, 3des-192, sm1-256, sm4-128 */
    SYMC_KEY_AES_192BIT,        /* AES 192 bit key */
    SYMC_KEY_AES_256BIT,        /* AES 256 bit key */
    SYMC_KEY_TDES_2KEY,         /* 3DES 128 bit key */
    SYMC_KEY_LEN_COUNT,
} symc_klen;

/* symmetric cipher mode */
typedef enum {
    SYMC_MODE_ECB = 0, /* Electronic Codebook Mode */
    SYMC_MODE_CBC,     /* Cipher Block Chaining */
    SYMC_MODE_CFB,     /* Cipher Feedback Mode */
    SYMC_MODE_OFB,     /* Output Feedback Mode */
    SYMC_MODE_CTR,     /* Counter Mode */
    SYMC_MODE_CCM,     /* Counter with Cipher Block Chaining-Message Authentication Code */
    SYMC_MODE_GCM,     /* Galois/Counter Mode */
    SYMC_MODE_CTS,     /* CTS Mode */
    SYMC_MODE_COUNT,
} symc_mode;

/* locational of buffer under symmetric cipher */
typedef enum {
    SYMC_NODE_USAGE_NORMAL      = 0x00, /* The normal buffer, don't update the iv */
    SYMC_NODE_USAGE_FIRST       = 0x01, /* The first buffer, the usage of iv is expired */
    SYMC_NODE_USAGE_LAST        = 0x02, /* The last buffer, must update the iv */
    SYMC_NODE_USAGE_ODD_KEY     = 0x40, /* Use the odd key to encrypt or decrypt this buffer */
    SYMC_NODE_USAGE_EVEN_KEY    = 0x00, /* Use the even key to encrypt or decrypt this buffer */
    SYMC_NODE_USAGE_IN_GCM_A    = 0x00, /* The buffer of GCM A */
    SYMC_NODE_USAGE_IN_GCM_P    = 0x08, /* The buffer of GCM P */
    SYMC_NODE_USAGE_IN_GCM_LEN  = 0x10, /* The buffer of GCM LEN */
    SYMC_NODE_USAGE_IN_CCM_N    = 0x00, /* The buffer of CCM N */
    SYMC_NODE_USAGE_IN_CCM_A    = 0x08, /* The buffer of CCM A */
    SYMC_NODE_USAGE_IN_CCM_P    = 0x10, /* The buffer of CCM P */
    SYMC_NODE_USAGE_CCM_LAST    = 0x20, /* The buffer of CCM LAST */
} symc_node_usage;

/* symc error code */
typedef enum {
    HI_SYMC_ERR_ALG_INVALID = HI_BASE_ERR_BASE_SYMC, /* invalid algorithm */
    HI_SYMC_ERR_MODE_INVALID,       /* invalid mode */
    HI_SYMC_ERR_LEN_INVALID,        /* data length invalid */
    HI_SYMC_ERR_IV_LEN_INVALID,     /* IV length invalid */
    HI_SYMC_ERR_TAG_LEN_INVALID,    /* TAG length invalid */
    HI_SYMC_ERR_KEY_LEN_INVALID,    /* key length invalid */
    HI_SYMC_ERR_KEY_INVALID,        /* key invalid */
    HI_SYMC_ERR_ID_INVALID,         /* channel id invalid */
    HI_SYMC_ERR_SMMU_INVALID,       /* SMMU invalid */
    HI_SYMC_ERR_TIME_OUT,           /* encrypt/decrypt timeout */
    HI_SYMC_ERR_BUSY,               /* busy */
} symc_error_code;

typedef struct {
    hi_u32 id;
    char *open;
    char *alg;
    char *mode;
    hi_u32 klen;
    char *ksrc;
    hi_u8 decrypt;
    hi_u32 inlen;
    hi_u32 inaddr;
    hi_u32 outlen;
    hi_u32 outaddr;
    hi_u8 intswitch;
    hi_u8 inten;
    hi_u8 inraw;
    hi_u8 outen;
    hi_u8 outraw;
    hi_u32 outintcnt;
    char  iv[AES_IV_SIZE * MUL_VAL_2 + BOUND_VAL_1];
} symc_chn_status;

/* symc capacity, 0-nonsupport, 1-support */
typedef struct {
    hi_u32 aes_ecb     : 1;    /* Support AES ECB  */
    hi_u32 aes_cbc     : 1;    /* Support AES CBC  */
    hi_u32 aes_cfb     : 1;    /* Support AES CFB  */
    hi_u32 aes_ofb     : 1;    /* Support AES OFB  */
    hi_u32 aes_ctr     : 1;    /* Support AES CTR  */
    hi_u32 aes_ccm     : 1;    /* Support AES CCM  */
    hi_u32 aes_gcm     : 1;    /* Support AES GCM  */
    hi_u32 aes_cts     : 1;    /* Support AES CTS  */
    hi_u32 tdes_ecb    : 1;    /* Support TDES ECB */
    hi_u32 tdes_cbc    : 1;    /* Support TDES CBC */
    hi_u32 tdes_cfb    : 1;    /* Support TDES CFB */
    hi_u32 tdes_ofb    : 1;    /* Support TDES OFB */
    hi_u32 tdes_ctr    : 1;    /* Support TDES CTR */
    hi_u32 des_ecb     : 1;    /* Support DES ECB */
    hi_u32 des_cbc     : 1;    /* Support DES CBC */
    hi_u32 des_cfb     : 1;    /* Support DES CFB */
    hi_u32 des_ofb     : 1;    /* Support DES OFB */
    hi_u32 des_ctr     : 1;    /* Support DES CTR */
    hi_u32 sm1_ecb     : 1;    /* Support SM1 ECB  */
    hi_u32 sm1_cbc     : 1;    /* Support SM1 CBC  */
    hi_u32 sm1_cfb     : 1;    /* Support SM1 CFB  */
    hi_u32 sm1_ofb     : 1;    /* Support SM1 OFB  */
    hi_u32 sm1_ctr     : 1;    /* Support SM1 CTR  */
    hi_u32 sm4_ecb     : 1;    /* Support SM4 ECB  */
    hi_u32 sm4_cbc     : 1;    /* Support SM4 CBC  */
    hi_u32 sm4_cfb     : 1;    /* Support SM4 CFB  */
    hi_u32 sm4_ofb     : 1;    /* Support SM4 OFB  */
    hi_u32 sm4_ctr     : 1;    /* Support SM4 CTR  */
    hi_u32 dma         : 1;    /* Support DMA  */
} symc_capacity;

typedef struct {
    compat_addr *in;
    compat_addr *out;
    hi_u32 *len;
    symc_node_usage *usage;
    hi_u32 num;
} symc_multi_pack;

/*
 * \brief          symc context structure
 *
 * \note           if the aes key derived from klad, the context must
 *                 attached with a independent hard key channel,
 *                 otherwise the context can attached with a fixed common channel.
 */
typedef struct {
    hi_u32 even_key[SYMC_KEY_SIZE / WORD_WIDTH];    /* SYMC even round keys, default */
    hi_u32 odd_key[SYMC_KEY_SIZE / WORD_WIDTH];     /* SYMC odd round keys, default */
    hi_u32 sk[SYMC_SM1_SK_SIZE / WORD_WIDTH];       /* sm1 sk */
    hi_u32 iv[AES_IV_SIZE / WORD_WIDTH];            /* symc IV */
    hi_u32 tag[AEAD_TAG_SIZE / WORD_WIDTH];         /* aead tag */
    hi_u32 ivlen;                                   /* symc IV length */
    hi_u32 iv_usage;                                /* symc IV usage */

    hi_u32 hard_chn;             /* hard channel number */
    hi_u32 hard_key;             /* Key derived from klad or CPU */

    symc_alg alg;                /* Symmetric cipher algorithm */
    symc_width width;            /* Symmetric cipher width */
    hi_u32 klen;                 /* Symmetric cipher key length */

    compat_addr aad;             /* Associated Data */
    hi_u32 alen;                 /* Associated Data length */
    hi_u32 tlen;                 /* Tag length */

    symc_mode mode;              /* Symmetric cipher mode */

    hi_u32 sm1_round;            /* SM1 round number */
    hi_u32 enclen;               /* encrypt length */

    hi_u32 block_size;           /* Block size */

    hi_u32 cur_nodes;            /* current nodes id  */
    hi_u32 total_nodes;          /* total number of nodes */

    compat_addr *input_list;     /* input node list */
    compat_addr *output_list;    /* output node list */
    hi_u32 *length_list;         /* length of node list */
    symc_node_usage *usage_list; /* usage of node list */
    hi_bool tdes2dma;            /* 3des with invalid key turns to dma */
} cryp_symc_context;

typedef hi_s32 (*callback_symc_isr)(hi_void *ctx);
typedef hi_void (*callback_symc_destroy)(hi_void);

/*
 * brief  Initialize the symc module.
 * retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_symc_init(hi_void);

/*
 * brief  Deinitialize the symc module.
 * retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_symc_deinit(hi_void);

/*
 * brief  suspend the symc module.
 * retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_void drv_symc_suspend(hi_void);

/*
 * brief  resume the symc module.
 * retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_symc_resume(hi_void);

/*
 * brief  allocate a hard symc channel.
 * param[out]  chn_num The channel number.
 * retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_symc_alloc_chn(hi_u32 *chn_num);

/*
 * brief  free a hard symc channel.
 * param[in]  chn_num The channel number.
 * retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_void drv_symc_free_chn(hi_u32 chn_num);

/*
 * brief  set the iv to the symc module.
 * param[in]  chn_num The channel number.
 * retval     NA.
 */
hi_s32 drv_symc_reset(hi_u32 chn_num);

/*
 * brief  symc get error code.
 * param[in]  chn_num The channel number.
 * retval     NA.
 */
hi_void drv_symc_get_err_code(hi_u32 chn_num);

/*
 * brief  check the length of nodes list.
 * param[in]  alg The symmetric cipher algorithm.
 * param[in]  mode The symmetric cipher mode.
 * param[in]  block_size The block size.
 * param[in]  pack pack data info.
 * retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 * retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_symc_node_check(symc_alg alg, symc_mode mode, symc_klen klen, hi_u32 block_size, symc_multi_pack *pack);

/*
 * brief  set work params.
 * param[in]  ctx cryp symc context info.
 * param[in]  decrypt Decrypt or encrypt.
 * param[in]  klen The key length.
 * retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_symc_cfg(cryp_symc_context *ctx, hi_u32 decrypt, symc_klen klen);

/*
 * brief  set the iv to the symc module.
 * param[in]  chn_num The channel number.
 * param[in]  iv The IV data, hardware use the valid bytes according to the alg.
 * param[in]  flag The IV flag, should be HI_CIPHER_IV_CHG_ONE_PACK or HI_CIPHER_IV_CHG_ALL_PACK.
 * retval     NA.
 */
hi_s32 drv_symc_set_iv(hi_u32 chn_num, const hi_u32 iv[SYMC_IV_MAX_SIZE_IN_WORD], hi_u32 ivlen, hi_u32 flag);

/*
 * brief  get the iv to the symc module.
 * param[in]  chn_num The channel number.
 * param[out] iv The IV data, the length is 16.
 * retval     NA.
 */
hi_void drv_symc_get_iv(hi_u32 chn_num, hi_u32 iv[SYMC_IV_MAX_SIZE_IN_WORD]);

/*
 * brief  set the key to the symc module.
 * param[in]  chn_num The channel number.
 * param[in]  key The key data, hardware use the valid bytes according to the alg.
 * param[in]  odd This id odd key or not .
 * retval     NA.
 */
hi_void drv_symc_set_key(hi_u32 chn_num, const hi_u32 key[SYMC_KEY_MAX_SIZE_IN_WORD], hi_u32 key_len, hi_u32 odd);

/*
 * brief  set the sm1 sk to the symc module.
 * param[in]  chn_num The channel number.
 * param[in]  key The sk data, the length is 16.
 * retval     NA.
 */
hi_void drv_symc_set_sm1_sk(hi_u32 chn_num, const hi_u32 key[SYMC_SM1_SK_SIZE_IN_WORD], hi_u32 key_len);

/*
 * brief  add a in buffer to the nodes list.
 * param[in]  chn_num The channel number.
 * param[in]  buf_phy The MMZ/SMMU address of in buffer.
 * param[in]  buf_size The MMZ/SMMU size of in buffer.
 * param[in]  local The locational of in buffer under a symmetric cipher.
 * retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_symc_add_inbuf(hi_u32 chn_num, compat_addr buf_phy, hi_u32 buf_size, symc_node_usage usage);

/*
 * brief  add a out buffer to the nodes list.
 * param[in]  chn_num The channel number.
 * param[in]  buf_phy The MMZ/SMMU address of out buffer.
 * param[in]  buf_size The MMZ/SMMU size of out buffer.
 * param[in]  local The locational of in buffer under a symmetric cipher.
 * retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_symc_add_outbuf(hi_u32 chn_num, compat_addr buf_phy, hi_u32 buf_size, symc_node_usage usage);

/*
 * brief  add a buffer usage to the nodes list.
 * param[in]  chn_num The channel number.
 * param[in]  in in or out.
 * param[in]  usage usage.
 * retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_void drv_symc_add_buf_usage(hi_u32 chn_num, hi_u32 in, symc_node_usage usage);

/*
 * brief  add N of CCM to the nodes list.
 * param[in]  chn_num The channel number.
 * param[in]  nonce The buffer of n, the size is 16.
 * param[in]  nonce_len The buffer of n, the size is 16.
 * retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_aead_ccm_add_n(hi_u32 chn_num, hi_u8 *nonce, hi_u32 nonce_len);

/*
 * brief  add A of CCM to the nodes list.
 * param[in]  chn_num The channel number.
 * param[in]  buf_phy The MMZ/SMMU address of A.
 * param[in]  buf_size The MMZ/SMMU size of A.
 * retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_aead_ccm_add_a(hi_u32 chn_num, compat_addr buf_phy, hi_u32 buf_size);

/*
 * brief  add A of GCM to the nodes list.
 * param[in]  chn_num The channel number.
 * param[in]  buf_phy The MMZ/SMMU address of A.
 * param[in]  buf_size The MMZ/SMMU size of A.
 * retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_aead_gcm_add_a(hi_u32 chn_num, compat_addr buf_phy, hi_u32 buf_size);

/*
 * brief  add length field of GCM to the nodes list.
 * param[in]  chn_num The channel number.
 * param[in]  buf_phy The MMZ/SMMU address of length field, the size is 16.
 * retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_aead_gcm_add_clen(hi_u32 chn_num, hi_u8 *clen, hi_u32 clen_len);

/*
 * brief  get ccm/gcm tag.
 * param[in]   chn_num The channel number.
 * param[out]  tag The tag value.
 * param[in]   tag_buf_len length in byte.
 * retval     On received interception, HI_TRUE is returned  otherwise HI_FALSE is returned.
 */
hi_s32 drv_aead_get_tag(hi_u32 chn_num, hi_u32 *tag, hi_u32 tag_buf_len);

/*
 * brief  start symmetric cipher calculation.
 * param[in]  chn_num The channel number.
 * retval     On success, HI_SUCCESS is returned.  On error, HI_FAILURE is returned.
 */
hi_s32 drv_symc_start(hi_u32 chn_num);

/*
 * brief  wait running finished.
 * param[in]  chn_num The channel number.
 * retval     On received interception, HI_TRUE is returned  otherwise HI_FALSE is returned.
 */
hi_s32 drv_symc_wait_done(hi_u32 chn_num, hi_u32 timeout);

/*
 * brief  set isr callback function.
 * param[in]  chn_num The channel number.
 * retval     On finished, HI_TRUE is returned otherwise HI_FALSE is returned.
 */
hi_s32 drv_symc_set_isr_callback(hi_u32 chn_num, callback_symc_isr callback, hi_void *tx);

/*
 * brief  proc status.
 * param[in]  status The  proc status.
 * retval     On received interception, HI_TRUE is returned  otherwise HI_FALSE is returned.
 */
hi_s32 drv_symc_proc_status(symc_chn_status *status);

/*
 * brief  get the symc capacity.
 * param[out] capacity The symc capacity.
 * retval     NA.
 */
hi_void drv_symc_get_capacity(symc_capacity *capacity);

/*
 * brief  check drv symc is secure or not.
 * retval     NA.
 */
hi_u32 drv_symc_is_secure(hi_void);

#endif /* End of __DRV_SYMC_H__ */
