/* Portions of this file are covered by */
/* -*- mode: c; c-file-style: "k&r" -*-

  strnatcmp.c -- Perform 'natural order' comparisons of strings in C.
  Copyright (C) 2000 by Martin Pool <mbp@humbug.org.au>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef API_STRINGS_H
#define API_STRINGS_H
//////////////////////////////////////////////////////////////////////////////////////////

/**
 * @file api_strings.h
 * @brief API Strings library
 */

#include <api_errno.h>
#include <api_lib.h>
//#include <api_object.h>
//#include <api_tables.h>
#define API_WANT_IOVEC
#include <api_want.h>

#if API_HAVE_STDARG_H
#include <stdarg.h>
#endif

#if API_HAVE_ICONV_H
#include <iconv.h>
#endif
#if API_HAVE_SYS_ICONV_H
#include <sys/iconv.h>
#endif

#if API_HAVE_STRING_H
#include <string.h>
#include <stdio.h> //snprintf()
#endif
//////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* A couple of prototypes for functions in case some platform doesn't 
 * have it
 */
#if (!API_HAVE_STRCASECMP) && (API_HAVE_STRICMP) 
#define strcasecmp(s1, s2) stricmp(s1, s2)
#elif (!API_HAVE_STRCASECMP)
int strcasecmp(const char *a, const char *b);
#endif

#if (!API_HAVE_STRNCASECMP) && (API_HAVE_STRNICMP)
#define strncasecmp(s1, s2, n) strnicmp(s1, s2, n)
#elif (!API_HAVE_STRNCASECMP)
int strncasecmp(const char *a, const char *b, size_t n);
#endif

/**
 * @defgroup api_ctype ctype functions
 * These macros allow correct support of 8-bit characters on systems which
 * support 8-bit characters.  Pretty dumb how the cast is required, but
 * that's legacy libc for ya.  These new macros do not support EOF like
 * the standard macros do.  Tough.
 * @{
 */

/** @see isalnum */
#define api_isalnum(c) (isalnum(((unsigned char)(c))))
/** @see isalpha */
#define api_isalpha(c) (isalpha(((unsigned char)(c))))
/** @see iscntrl */
#define api_iscntrl(c) (iscntrl(((unsigned char)(c))))
/** @see isdigit */
#define api_isdigit(c) (isdigit(((unsigned char)(c))))
/** @see isgraph */
#define api_isgraph(c) (isgraph(((unsigned char)(c))))
/** @see islower*/
#define api_islower(c) (islower(((unsigned char)(c))))
/** @see isascii */
#ifdef isascii
#define api_isascii(c) (isascii(((unsigned char)(c))))
#else
#define api_isascii(c) (((c) & ~0x7f)==0)
#endif
/** @see isprint */
#define api_isprint(c) (isprint(((unsigned char)(c))))
/** @see ispunct */
#define api_ispunct(c) (ispunct(((unsigned char)(c))))
/** @see isspace */
#define api_isspace(c) (isspace(((unsigned char)(c))))
/** @see isupper */
#define api_isupper(c) (isupper(((unsigned char)(c))))
/** @see isxdigit */
#define api_isxdigit(c) (isxdigit(((unsigned char)(c))))
/** @see tolower */
#define api_tolower(c) (tolower(((unsigned char)(c))))
/** @see toupper */
#define api_toupper(c) (toupper(((unsigned char)(c))))

/** @} */

/**
 * @defgroup api_strings String routines
 * @ingroup API 
 * @{
 */

/**
 * ȡ�ַ�������
 * @param[in] s �ַ����׵�ַ
 * @return �ַ�������
 */
#define api_strlen(s) ((s) == NULL ? 0 : strlen(s))

/**
 * Do a natural order comparison of two strings.
 * @param a The first string to compare
 * @param b The second string to compare
 * @return Either <0, 0, or >0.  If the first string is less than the second
 *          this returns <0, if they are equivalent it returns 0, and if the
 *          first string is greater than second string it retuns >0.
 */
API_DECLARE(int) api_strnatcmp(char const *a, char const *b);

/**
 * Do a natural order comparison of two strings ignoring the case of the 
 * strings.
 * @param a The first string to compare
 * @param b The second string to compare
 * @return Either <0, 0, or >0.  If the first string is less than the second
 *         this returns <0, if they are equivalent it returns 0, and if the
 *         first string is greater than second string it retuns >0.
 */
API_DECLARE(int) api_strnatcasecmp(char const *a, char const *b);

/**
 * Copy up to dst_size characters from src to dst; does not copy
 * past a NUL terminator in src, but always terminates dst with a NUL
 * regardless.
 * @param dst The destination string
 * @param src The source string
 * @param dst_size The space available in dst; dst always receives
 *                 NUL termination, so if src is longer than
 *                 dst_size, the actual number of characters copied is
 *                 dst_size - 1.
 * @return Pointer to the NUL terminator of the destination string, dst
 * @remark
 * <PRE>
 * Note the differences between this function and strncpy():
 *  1) strncpy() doesn't always NUL terminate; api_cpystrn() does.
 *  2) strncpy() pads the destination string with NULs, which is often 
 *     unnecessary; api_cpystrn() does not.
 *  3) strncpy() returns a pointer to the beginning of the dst string;
 *     api_cpystrn() returns a pointer to the NUL terminator of dst, 
 *     to allow a check for truncation.
 * </PRE>
 */
API_DECLARE(char *) api_cpystrn(char *dst, const char *src, size_t dst_size);

/**
 * Strip spaces from a string
 * @param dest The destination string.  It is okay to modify the string
 *             in place.  Namely dest == src
 * @param src The string to rid the spaces from.
 * @return The destination string, dest.
 */
API_DECLARE(char *) api_collapse_spaces(char *dest, const char *src);

/**
 * Split a string into separate null-terminated tokens.  The tokens are 
 * delimited in the string by one or more characters from the sep
 * argument.
 * @param str The string to separate; this should be specified on the
 *            first call to api_strtok() for a given string, and NULL
 *            on subsequent calls.
 * @param sep The set of delimiters
 * @param last Internal state saved by api_strtok() between calls.
 * @return The next token from the string
 */
API_DECLARE(char *) api_strtok(char *str, const char *sep, char **last);

/**
 * @defgroup API_Strings_Snprintf snprintf implementations
 * @warning
 * These are snprintf implementations based on api_vformatter().
 *
 * Note that various standards and implementations disagree on the return
 * value of snprintf, and side-effects due to %n in the formatting string.
 * api_snprintf (and api_vsnprintf) behaves as follows:
 *
 * Process the format string until the entire string is exhausted, or
 * the buffer fills.  If the buffer fills then stop processing immediately
 * (so no further %n arguments are processed), and return the buffer
 * length.  In all cases the buffer is NUL terminated. It will return the
 * number of characters inserted into the buffer, not including the
 * terminating NUL. As a special case, if len is 0, api_snprintf will
 * return the number of characters that would have been inserted if
 * the buffer had been infinite (in this case, *buffer can be NULL)
 *
 * In no event does api_snprintf return a negative number.
 * @{
 */

#if 0
/**
 * snprintf routine based on api_vformatter.  This means it understands the
 * same extensions.
 * @param buf The buffer to write to
 * @param len The size of the buffer
 * @param format The format string
 * @param ... The arguments to use to fill out the format string.
 */
API_DECLARE_NONSTD(int) api_snprintf(char *buf, api_size_t len,
                                     const char *format, ...)
        __attribute__((format(printf,3,4)));

/**
 * vsnprintf routine based on api_vformatter.  This means it understands the
 * same extensions.
 * @param buf The buffer to write to
 * @param len The size of the buffer
 * @param format The format string
 * @param ap The arguments to use to fill out the format string.
 */
API_DECLARE(int) api_vsnprintf(char *buf, api_size_t len, const char *format,
                               va_list ap);
#else
#ifdef __API_WINDOWS__
#define api_snprintf _snprintf
#define api_vsnprintf _vsnprintf
#else
#define api_snprintf snprintf
#define api_vsnprintf vsnprintf
#endif
#endif
/** @} */

/**
 * Convert a numeric string into an api_off_t numeric value.
 * @param offset The value of the parsed string.
 * @param buf The string to parse. It may contain optional whitespace,
 *   followed by an optional '+' (positive, default) or '-' (negative)
 *   character, followed by an optional '0x' prefix if base is 0 or 16,
 *   followed by numeric digits appropriate for base.
 * @param end A pointer to the end of the valid character in buf. If
 *   not NULL, it is set to the first invalid character in buf.
 * @param base A numeric base in the range between 2 and 36 inclusive,
 *   or 0.  If base is zero, buf will be treated as base ten unless its
 *   digits are prefixed with '0x', in which case it will be treated as
 *   base 16.
 */
API_DECLARE(api_status_t) api_strtoff(api_off_t *offset, const char *buf, 
                                      char **end, int base);

/**
 * parse a numeric string into a 64-bit numeric value
 * @param buf The string to parse. It may contain optional whitespace,
 *   followed by an optional '+' (positive, default) or '-' (negative)
 *   character, followed by an optional '0x' prefix if base is 0 or 16,
 *   followed by numeric digits appropriate for base.
 * @param end A pointer to the end of the valid character in buf. If
 *   not NULL, it is set to the first invalid character in buf.
 * @param base A numeric base in the range between 2 and 36 inclusive,
 *   or 0.  If base is zero, buf will be treated as base ten unless its
 *   digits are prefixed with '0x', in which case it will be treated as
 *   base 16.
 * @return The numeric value of the string.  On overflow, errno is set
 * to ERANGE.
 */
API_DECLARE(api_int64_t) api_strtoi64(const char *buf, char **end, int base);

/**
 * parse a base-10 numeric string into a 64-bit numeric value.
 * Equivalent to api_strtoi64(buf, (char**)NULL, 10).
 * @param buf The string to parse
 * @return The numeric value of the string
 */
API_DECLARE(api_int64_t) api_atoi64(const char *buf);

/**
 * Format a binary size (magnitiudes are 2^10 rather than 10^3) from an api_off_t,
 * as bytes, K, M, T, etc, to a four character compacted human readable string.
 * @param size The size to format
 * @param buf The 5 byte text buffer (counting the trailing null)
 * @return The buf passed to api_strfsize()
 * @remark All negative sizes report '  - ', api_strfsize only formats positive values.
 */
API_DECLARE(char *) api_strfsize(api_off_t size, char *buf);

//////////////////////////////////////////////////////////////////////////////////////////

/**
 * ת���ַ����Ӵ�д��Сд
 * @param[in] str �ַ���
 * @return ��д�ַ���
 */
API_DECLARE(char *) api_strtolc(char *str);

/**
 * ת���ַ�����Сд����д
 * @param[in] str Դ�ַ���
 * @return ��д���ַ���
 */
API_DECLARE(char *) api_strtouc(char *str);

/**
 * ���ַ�����ת����ʮ�������ַ���
 * @param[out] strHex ʮ�������ַ���
 * @param[in] str �ַ�����
 * @param[in] len �ַ����鳤��
 */
API_DECLARE(char *) api_mem2hex(char *strHex, const char *str, const api_int32_t len);

/**
 * ��ʮ�������ַ���ת�����ֽ�����
 * @param[out] str ASCII�ַ���
 * @param[in] strHex ʮ�������ַ���
 * @param[in] len  ʮ�������ַ�������
 * @return �ֽ����鳤��
 */
API_DECLARE(int) api_hex2mem(char  *str, const char  *strHex, const api_int32_t len);

/**
 * �����ַ���
 * @param[out] buffer Ŀ���ַ���
 * @param[in] lpszFormat ��ʽ���ַ���
 * @param[in] ... �������Ϣ����
 */
API_DECLARE_NONSTD(char *) api_strcat(char *buffer, const char * lpszFormat, ...);

/**
 * ��ȡ�ַ���
 * @param[out] str �ַ���
 * @param[in] src Դ�ַ���
 * @param[in] ... ��ʼλ�úͳ���
 */
API_DECLARE_NONSTD(char *) api_substr(char *str, const char *src, ...);

/**
 * �����ַ���
 * @param[in] haystack Ŀ���ַ���
 * @param[in] needle   �����Ӵ�
 */
#define api_strstr(s1, s2) (((s1) == NULL || (s2) == NULL) ? NULL : strstr((s1), (s2)))

/**
 * ���Դ�Сд���в����ַ���
 * @param[in] haystack Ŀ���ַ���
 * @param[in] needle   �����Ӵ�
 */
API_DECLARE_NONSTD(char *) api_stristr(const char* haystack, const char *needle);

/**
 * ȥ����β�ո���Ʊ��
 * @param[in] pstr �ֽ�������ַ���ָ��
 * @return ȥ�����ҿո��Ʊ���س����еȷ��ŵ��ַ���ָ��
 */
API_DECLARE(char *) api_strtrim(char *pstr);

/**
 * �滻�ַ���
 * @param[out] haystack Ŀ���ַ���
 * @param[in] needle �����Ӵ�
 * @param[in] rep ������ַ���
 * @param[in] times �滻����, Ĭ��1��
 */
API_DECLARE(char *) api_strrep(char* haystack, const char *needle, const char *rep, const api_int32_t times);

/**
 * ���Դ�Сд�����滻�ַ���
 * @param[out] haystack Ŀ���ַ���
 * @param[in] needle �����Ӵ�
 * @param[in] rep ������ַ���
 * @param[in] times �滻����, Ĭ��1��
 */
API_DECLARE(char *) api_strirep(char *haystack, const char *needle, const char *rep, const api_int32_t times);

/**
 * �滻�ַ���
 * @param[out] haystack Ŀ���ַ���
 * @param[in] needle �����Ӵ�, ��ʽ${needle}
 * @param[in] rep  ������ַ���
 */
API_DECLARE(char *) api_strcontain(char *haystack, const char *needle, const char *rep);

/**
 * URL����
 * @param[out] buf ���������
 * @param[in] str url�ַ���
 * @return ���ر������ַ�������
 */
API_DECLARE(int) api_url_encode(char *buf, const char *str);

/**
 * URL����
 * @param[out] buf ���������
 * @param[in] str url�ַ���
 * @return ���ر������ַ�������
 */
API_DECLARE(int) api_url_decode(char *buf, const char *str);

/**
 * ��������ת�����ַ���
 * @param[out] buf �ַ���ָ��
 * @param[in] val �ĸ��ֽ��޷�������
 * @param[in] base ����
 * @param[in] bitlen λ��
 */
API_DECLARE(char *) api_ultoa(char *buf, const api_uint32_t val, const api_int32_t base, const api_int32_t bitlen);

/**
 * ����ת��
 * @param[in] from_code ԭ������ʽ
 * @param[in] from ԭ��
 * @param[in] to_code Ŀ������ʽ
 * @param[in] to Ŀ����뻺����
 * @param[in] iLength ԭ�볤��
 */
API_DECLARE(int) api_iconv(const char *from_code, const char *from, const char *to_code, char *to, size_t iLength);

/**
 * ��GB2312����ת����UCS2����
 * @param[in] strDestination Ŀ���ַ���
 * @param[in] strSource Դ�ַ���
 * @param[in] nLength GB2312Դ�ַ�������
 */
API_DECLARE(int) A2U(char *strDestination, const char *strSource, int nLength);

/**
 * ��UCS2����ת����GB2312����
 * @param[out] strDestination Ŀ���ַ���
 * @param[in] strSource Դ�ַ���
 * @param[in] nLength UCS2����Դ�ַ�������
 */
API_DECLARE(int) U2A(char *strDestination, const char *strSource, int nLength);

/**
 * ��ȡ�����ַ���
 * @return ����ϵͳ���ַ���
 */
API_DECLARE(const char *) api_locale_encoding(void);

/**
 * ����ļ�·��
 * @param[in] path ·��
 * @param[in] size �ߴ�
 * @param[in] filename �ļ���
 * @return ����·�����ļ���
 */
API_DECLARE(const char *) api_filename_path_get(char *path, size_t size, const char *filename);

/**
 * ����ļ���չ��
 * @param[in] filename �ļ���
 * @return �ļ���չ��
 */
API_DECLARE(const char *) api_filename_ext_get(const char *filename);

/**
 * ����ǰ׺��׺��ȡ�м䲿�ֵ��ַ���
 * @param[out] buf ������
 * @param[in] str �ַ���
 * @param[in] prefix ǰ׺�ַ���
 * @param[in] suffix ��׺�ַ���
 */
API_DECLARE(char *) api_distill(char *buf, const char *str, const char *prefix, const char *suffix);

/**
 * �ָ��ַ���, ����ַ�����ά����
 * @param[out] buf ������
 * @param[in] str �ַ���
 * @param[in] spliter ǰ׺�ַ���
 * @param[in] pool �ڴ��
 * @return �ָ�����
 */
//API_DECLARE(api_uint32_t) api_split(char ***buf, const char *str, const char *spliter, api_pool_t *pool);

/**
 * �ָ��ַ���, �����浽hash tables
 * @param[out] tbl hash tables
 * @param[in] str �ַ���
 * @param[in] spliter ǰ׺�ַ���
 * @param[in] pool �ڴ��
 * @return �ָ�����
 */
//API_DECLARE(api_uint32_t) api_str2tbl(api_table_t *tbl, const char *str, const char *spliter, api_pool_t *pool);

/** @} */

#ifdef __cplusplus
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////
#endif  /* !API_STRINGS_H */
