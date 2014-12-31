#ifndef __API_BUFFER_H_INCLUDED__
#define __API_BUFFER_H_INCLUDED__

#include "api.h"
//////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup api_buffer Buffer routines
 * @ingroup API 
 * @{
 */

/**
 * max size of a buffer which will just be reset
 * to ->used = 0 instead of really freeing the buffer
 *
 * 64kB (no real reason, just a guess)
 */
#define BUFFER_MAX_REUSE_SIZE  (64 * 1024)

typedef struct api_buffer_t
{
    char       *ptr;   /**< ������ָ�� */
    api_size_t  used;  /**< ʹ�óߴ� */
    api_size_t  size;  /**< �������ߴ� */
} api_buffer_t;

/** Э�����ݻ��� */
typedef struct api_packet_t
{
    api_uint8_t data[API_BUFFER_MAX]; /**< ������ */
    api_size_t  length;               /**< ���ݳ��� */
    int         gotheader;            /**< �Ƿ�������ͷ */
}api_packet_t;

//////////////////////////////////////////////////////////////////////////////////////////

/**
 * init the api_buffer_t
 * @return api_buffer_t point
 */
API api_buffer_t * api_buffer_init(void);

/**
 * �ͷ��ڴ�
 * @return api_buffer_t point
 */
API void api_buffer_free(api_buffer_t *b);

/**
 * �����ڴ�
 * @return api_buffer_t point
 */
API void api_buffer_reset(api_buffer_t *b);

/**
 * �������Ƿ�Ϊ��
 * @param[in] b api_buffer_t point
 * @return 1 or 0
 */
API int api_buffer_is_empty(api_buffer_t *b);

/**
 * �������Ƿ�Ϊ��
 * @param[in] a api_buffer_t point
 * @param[in] b api_buffer_t point
 * @return 1 or 0
 */
API int api_buffer_is_equal(api_buffer_t *a, api_buffer_t *b);

//////////////////////////////////////////////////////////////////////////////////////////

/**
 * �ڴ��ƶ�
 * @param[in] b ������
 * @param[in] from ��ʼλ��
 * @param[in] to ����λ��
 */
API api_buffer_t * api_buffer_move(api_buffer_t *b, int from, int to);

/**
 * �ڴ濽��
 * @param[in] b ������
 * @param[in] s �ڴ��ַ
 * @param[in] s_len �ڴ泤��
 */
API int api_buffer_memcpy(api_buffer_t *b, const void *s, api_size_t s_len);

/**
 * ׷���ڴ�
 * @param[in] b ������
 * @param[in] s �ڴ��ַ
 * @param[in] s_len �ڴ泤��
 */
API int api_buffer_memcat(api_buffer_t *b, const void *s, api_size_t s_len);

/**
 * �ַ�������
 * @param[in] b ������
 * @param[in] s �ַ����׵�ַ
 */
API int api_buffer_strcpy(api_buffer_t *b, const char *s);

/**
 * �ַ�������
 * @param[in] b ������
 * @param[in] s �ַ����׵�ַ
 * @param[in] s_len �ַ�������
 */
API int api_buffer_strncpy(api_buffer_t *b, const char *s, api_size_t s_len);

/**
 * �����ַ���
 * @param[in] b ������
 * @param[in] s �ַ����׵�ַ
 */
API int api_buffer_strcat(api_buffer_t *b, const char *s);

/**
 * append a string to the end of the api_buffer_t
 *
 * the resulting api_buffer_t is terminated with a '\0'
 * s is treated as a un-terminated string (a \0 is handled a normal character)
 *
 * @param b a api_buffer_t
 * @param s the string
 * @param s_len size of the string (without the terminating \0)
 */
API int api_buffer_strncat(api_buffer_t *b, const char *s, api_size_t s_len);

/**
 * �����ַ���
 * @param[in] b the specail api_buffer_t object
 * @param[in] needle ��Ҫ�������ַ���
 * @param[in] len �ַ�������
 */
API char * api_buffer_strnstr(api_buffer_t *b, const char *needle, api_size_t len);

/**
 * ת����Сд
 * @param[in] b the specail api_buffer_t object
 */
API int api_buffer_to_lower(api_buffer_t *b);

/**
 * ת���ɴ�д
 * @param[in] b the specail api_buffer_t object
 */
API int api_buffer_to_upper(api_buffer_t *b);

//////////////////////////////////////////////////////////////////////////////////////////

/**
 * ����ʮ�����ƴ���������
 * @param[in] b ������
 * @param[in] in ʮ�������ַ���
 * @param[in] in_len ʮ�������ַ�������
 */
API int api_buffer_cpyhex(api_buffer_t *b, const char *in, api_size_t in_len);

//////////////////////////////////////////////////////////////////////////////////////////
API api_size_t api_buffer_put_int(api_buffer_t *buf, int64_t val);
API api_size_t api_buffer_put_uint(api_buffer_t *buf, uint64_t val);
API api_size_t api_buffer_put_byte(api_buffer_t *buf, api_byte_t val);

API api_size_t api_buffer_put_uint16(api_buffer_t *buf, api_uint16_t val);

API api_size_t api_buffer_put_uint32(api_buffer_t *buf, api_uint32_t val);

API api_size_t api_buffer_put_uint64(api_buffer_t *buf, api_uint64_t val);

API api_size_t api_buffer_fix_byte(api_buffer_t *buf, api_size_t pos, api_byte_t val);
API api_size_t api_buffer_fix_uint16(api_buffer_t *buf, api_size_t pos, api_uint16_t val);
API api_size_t api_buffer_fix_uint32(api_buffer_t *buf, api_size_t pos, api_uint32_t val);
API api_size_t api_buffer_fix_uint64(api_buffer_t *buf, api_size_t pos, api_uint64_t val);
//////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#ifdef __cplusplus
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////

#endif /* ! __API_BUFFER_H_INCLUDED__ */
