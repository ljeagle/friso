/*
 * utf-8 handle function implements.
 * 		you could modify it or re-release it but never for commercial use.
 * 
 * @author	chenxin <chenxin619315@gmail.com>
 */
#include "friso_API.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ******************************************
 * fstring buffer functions implements.		*
 ********************************************/
__STATIC_API__ fstring create_buffer( uint_t length ) 
{
    fstring buffer = ( fstring ) FRISO_MALLOC( length );
    if ( buffer == NULL ) {
	___ALLOCATION_ERROR___
    }

    memset( buffer, 0x00, length );

    return buffer;
}

//the __allocs should not be smaller than sb->length
__STATIC_API__ string_buffer_t resize_buffer( 
	string_buffer_t sb, uint_t __allocs ) 
{
    //create a new buffer.
    //if ( __allocs < sb->length ) __allocs = sb->length + 1; 
    fstring str = create_buffer( __allocs );

    //register uint_t t;
    //for ( t = 0; t < sb->length; t++ ) {
    //	str[t] = sb->buffer[t];
    //}
    memcpy( str, sb->buffer, sb->length );
    FRISO_FREE( sb->buffer );

    sb->buffer = str;
    sb->allocs = __allocs;

    return sb;
}

//create a new fstring buffer with a default opacity.
//FRISO_API string_buffer_t new_string_buffer( void ) 
//{
//    return new_string_buffer_with_opacity( __BUFFER_DEFAULT_LENGTH__ );
//}

//create a new fstring buffer with the given opacity.
FRISO_API string_buffer_t new_string_buffer_with_opacity( uint_t opacity ) 
{
    string_buffer_t sb = ( string_buffer_t ) 
	FRISO_MALLOC( sizeof( string_buffer_entry ) );
    if ( sb == NULL ) {
	___ALLOCATION_ERROR___
    } 

    sb->buffer = create_buffer( opacity );
    sb->length = 0;
    sb->allocs = opacity;

    return sb;
}

//create a buffer with the given string.
FRISO_API string_buffer_t new_string_buffer_with_string( fstring str ) 
{
    //buffer allocations.
    string_buffer_t sb = ( string_buffer_t ) 
	FRISO_MALLOC( sizeof( string_buffer_entry ) );
    if ( sb == NULL ) {
	___ALLOCATION_ERROR___
    }

    //initialize
    sb->length = strlen( str );
    sb->buffer = create_buffer( sb->length + __BUFFER_DEFAULT_LENGTH__ );
    sb->allocs = sb->length + __BUFFER_DEFAULT_LENGTH__;

    //register uint_t t;
    //copy the str to the buffer.
    //for ( t = 0; t < sb->length; t++ ) {
    //	sb->buffer[t] = str[t];
    //}
    memcpy( sb->buffer, str, sb->length );

    return sb;
}

FRISO_API void string_buffer_append( 
	string_buffer_t sb, fstring __str ) 
{
    register uint_t __len__ = strlen( __str );

    //check the necessity to resize the buffer.
    if ( sb->length + __len__ > sb->allocs ) {
	sb = resize_buffer( sb, ( sb->length + __len__ ) * 2 + 1 );
    }

    //register uint_t t;
    ////copy the __str to the buffer.
    //for ( t = 0; t < __len__; t++ ) {
    //	sb->buffer[ sb->length++ ] = __str[t];
    //}
    memcpy( sb->buffer + sb->length, __str, __len__ );
    sb->length += __len__;
}

FRISO_API void string_buffer_insert( 
	string_buffer_t sb, 
	uint_t idx, 
	fstring __str ) 
{
}

/*
 * remove the given bytes from the buffer start from idx.
 *		this will cause the byte move after the idx+length.
 *
 * @return the new string.
 */
FRISO_API fstring string_buffer_remove( 
	string_buffer_t sb, 
	uint_t idx, 
	uint_t length ) 
{
    uint_t t;
    //move the bytes after the idx + length
    for ( t = idx + length; t < sb->length; t++ ) {
	sb->buffer[t - length] = sb->buffer[t];
    }
    sb->buffer[t] = '\0';
    //memcpy( sb->buffer + idx, 
    //		sb->buffer + idx + length, 
    //		sb->length - idx - length );

    t = sb->length - idx;
    if ( t > 0 ) {
	sb->length -= ( t > length ) ? length : t;
    }
    sb->buffer[sb->length-1] = '\0';

    return sb->buffer;
}

/*
 * turn the string_buffer to a string.
 *		or return the buffer of the string_buffer.
 */
FRISO_API string_buffer_t string_buffer_trim( string_buffer_t sb ) 
{
    //resize the buffer.
    if ( sb->length < sb->allocs - 1 ) {
	sb = resize_buffer( sb, sb->length + 1 );
    }
    return sb;
}

/*
 * free the given fstring buffer.
 * and this function will not free the allocations of the 
 * 	string_buffer_t->buffer, we return it to you, if there is
 * 	a necessary you could free it youself by calling free();
 */
FRISO_API fstring string_buffer_devote( string_buffer_t sb ) 
{
    fstring buffer = sb->buffer;
    FRISO_FREE( sb );
    return buffer;
}

/*
 * clear the given fstring buffer.
 *		reset its buffer with 0 and reset its length to 0.
 */
FRISO_API void string_buffer_clear( string_buffer_t sb ) 
{
    memset( sb->buffer, 0x00, sb->length );
    sb->length = 0;
}

//free everything of the fstring buffer.
FRISO_API void free_string_buffer( string_buffer_t sb ) 
{
    FRISO_FREE( sb->buffer );
    FRISO_FREE( sb );
}


/* ******************************************
 * utf-8 handle functions implements		*
 ********************************************/

/*
 * print a character in a binary style.
 *
 * @param int
 */
FRISO_API void print_char_binary( char value ) 
{
    register uint_t t;

    for ( t = 0; t < __CHAR_BYTES__; t++ ) 
    {
	if ( ( value & 0x80 ) == 0x80 ) {
	    printf("1");
	} else {
	    printf("0");
	}
	value <<= 1;
    }
}

/*
 * get the bytes of a utf-8 char.
 * 		between 1 - 6.
 *
 * @param __char
 * @return int 
 */
FRISO_API int get_utf8_bytes( char value ) 
{	
    register uint_t t = 0;

    //one byte ascii char.
    if ( ( value & 0x80 ) == 0 ) return 1;

    for ( ; ( value & 0x80 ) != 0; value <<= 1 ) 
	t++;

    return t;
}

/*
 * get the unicode serial of a utf-8 char.
 * 
 * @param ch
 * @return int.
 */
FRISO_API int get_utf8_unicode( const fstring ch ) 
{
    int code = 0, bytes = get_utf8_bytes( *ch );
    register uchar_t *bit = ( uchar_t * ) &code;
    register char b1,b2,b3;

    switch ( bytes ) {
	case 1:
	    *bit = *ch;
	    break;
	case 2:
	    b1 = *ch;
	    b2 = *(ch + 1);

	    *bit     	= (b1 << 6) + (b2 & 0x3F);
	    *(bit+1) 	= (b1 >> 2) & 0x07;
	    break;
	case 3:
	    b1 = *ch;
	    b2 = *(ch + 1);
	    b3 = *(ch + 2);

	    *bit		= (b2 << 6) + (b3 & 0x3F);
	    *(bit+1)	= (b1 << 4) + ((b2 >> 2) & 0x0F);
	    break;
	    //ignore the ones that are larger than 3 bytes;
    }

    return code;
}

//turn the unicode serial to a utf-8 string.
FRISO_API int unicode_to_utf8( uint_t u, fstring __word ) 
{
    if ( u <= 0x0000007F ) {
	//U-00000000 - U-0000007F
	//0xxxxxxx
	*__word    		= ( u & 0x7F );
	return 1;
    } else if ( u >= 0x00000080 && u <= 0x000007FF ) {
	//U-00000080 - U-000007FF
	//110xxxxx 10xxxxxx
	*( __word + 1 ) = ( u & 0x3F) | 0x80;
	*__word     	= ((u >> 6) & 0x1F) | 0xC0;
	return 2;
    } else if ( u >= 0x00000800 && u <= 0x0000FFFF ) {
	//U-00000800 - U-0000FFFF
	//1110xxxx 10xxxxxx 10xxxxxx
	*( __word + 2 ) = ( u & 0x3F) | 0x80;
	*( __word + 1 ) = ((u >> 6) & 0x3F) | 0x80;
	*__word     	= ((u >> 12) & 0x0F) | 0xE0;
	return 3;
    } else if ( u >= 0x00010000 && u <= 0x001FFFFF ) {
	//U-00010000 - U-001FFFFF
	//11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
	*( __word + 3 ) = ( u & 0x3F) | 0x80;
	*( __word + 2 ) = ((u >>  6) & 0x3F) | 0x80;
	*( __word + 1 ) = ((u >> 12) & 0x3F) | 0x80;
	*__word     	= ((u >> 18) & 0x07) | 0xF0;
	return 4;
    } else if ( u >= 0x00200000 && u <= 0x03FFFFFF ) {
	//U-00200000 - U-03FFFFFF
	//111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
	*( __word + 4 ) = ( u & 0x3F) | 0x80;
	*( __word + 3 ) = ((u >>  6) & 0x3F) | 0x80;
	*( __word + 2 ) = ((u >> 12) & 0x3F) | 0x80;
	*( __word + 1 ) = ((u >> 18) & 0x3F) | 0x80;
	*__word     	= ((u >> 24) & 0x03) | 0xF8;
	return 5;
    } else if ( u >= 0x04000000 && u <= 0x7FFFFFFF ) {
	//U-04000000 - U-7FFFFFFF
	//1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
	*( __word + 5 ) = ( u & 0x3F) | 0x80;
	*( __word + 4 ) = ((u >>  6) & 0x3F) | 0x80;
	*( __word + 3 ) = ((u >> 12) & 0x3F) | 0x80;
	*( __word + 2 ) = ((u >> 18) & 0x3F) | 0x80;
	*( __word + 1 ) = ((u >> 24) & 0x3F) | 0x80;
	*__word     	= ((u >> 30) & 0x01) | 0xFC;
	return 6;
    }

    return 0;
}

/*
 * check the given char is a CJK char or not.
 * 	2E80-2EFF CJK 部首补充 
 * 	2F00-2FDF 康熙字典部首
 * 	3000-303F CJK 符号和标点 				--ignore
 * 	31C0-31EF CJK 笔画
 * 	3200-32FF 封闭式 CJK 文字和月份 
 * 	3300-33FF CJK 兼容
 * 	3400-4DBF CJK 统一表意符号扩展 A 
 * 	4DC0-4DFF 易经六十四卦符号
 * 	4E00-9FBF CJK 统一表意符号 
 * 	F900-FAFF CJK 兼容象形文字
 * 	FE30-FE4F CJK 兼容形式 
 * 	FF00-FFEF 全角ASCII、全角标点			--ignore (as basic latin)
 *
 * Japanese:
 * 	3040-309F 日本平假名
 * 	30A0-30FF 日本片假名
 * 	31F0-31FF 日本片假名拼音扩展
 *
 * Korean:
 * 	AC00-D7AF 韩文拼音
 * 	1100-11FF 韩文字母
 * 	3130-318F 韩文兼容字母
 * 
 * @param ch :pointer to the char
 * @return int : 1 for yes and 0 for not. 
 */

//Comment one of the following macro define
//to clear the check of the specified language.
#define FRISO_CJK_CHK_C
#define FRISO_CJK_CHK_J
#define FRISO_CJK_CHK_K
FRISO_API int utf8_cjk_string( uint_t u ) 
{
    int c = 0, j = 0, k = 0;
    //Chinese.
#ifdef FRISO_CJK_CHK_C
    c = ( ( u >= 0x4E00 && u <= 0x9FBF )
	    || ( u >= 0x2E80 && u <= 0x2EFF ) || ( u >= 0x2F00 && u <= 0x2FDF )  
	    || ( u >= 0x31C0 && u <= 0x31EF ) || ( u >= 0x3200 && u <= 0x32FF )
	    || ( u >= 0x3300 && u <= 0x33FF ) //|| ( u >= 0x3400 && u <= 0x4DBF )
	    || ( u >= 0x4DC0 && u <= 0x4DFF ) || ( u >= 0xF900 && u <= 0xFAFF )
	    || ( u >= 0xFE30 && u <= 0xFE4F ) ); 
#endif

    //Japanese.
#ifdef FRISO_CJK_CHK_J
    j = ( (u >= 0x3040 && u <= 0x309F)
	    || ( u >= 0x30A0 && u <= 0x30FF ) || ( u >= 0x31F0 && u <= 0x31FF ) );
#endif

    //Korean
#ifdef FRISO_CJK_CHK_K
    k = ( ( u >= 0xAC00 && u <= 0xD7AF )
	    || ( u >= 0x1100 && u <= 0x11FF ) || ( u >= 0x3130 && u <= 0x318F ) );
#endif

    return ( c || j || k );
}

/*
 * check the given char is a Basic Latin letter or not.
 *	include all the letters and english punctuations.
 * 
 * @param c
 * @return int 1 for yes and 0 for not. 
 */
FRISO_API int utf8_halfwidth_en_char( uint_t u ) 
{
    return ( u >= 32 && u <= 126 );
}

/*
 * check the given char is a full-width latain or not.
 *	include the full-width arabic numeber, letters.
 *	but not the full-width punctuations.
 *
 * @param c
 * @return int
 */
FRISO_API int utf8_fullwidth_en_char( uint_t u ) 
{
    return ( (u >= 65296 && u <= 65305 ) 			//arabic number
	    || ( u >= 65313 && u <= 65338 )			//upper case letters
	    || ( u >= 65345 && u <= 65370 ) );			//lower case letters
}

//check the given char is a upper case char or not.
FRISO_API int utf8_uppercase_letter( uint_t u ) 
{
    if ( u > 65280 ) u -= 65248;
    return ( u >= 65 && u <= 90 );
}

//check the given char is a upper case char or not.
FRISO_API int utf8_lowercase_letter( uint_t u )
{
    if ( u > 65280 ) u -= 65248;
    return ( u >= 97 && u <= 122 );
}

//check the given char is a numeric
FRISO_API int utf8_numeric_letter( uint_t u ) 
{
    if ( u > 65280 ) u -= 65248;	//make full-width half-width.
    return ( ( u >= 48 && u <= 57 ) );
}

//check the given char is a english char.
//not the punctuation of course.
FRISO_API int utf8_en_letter( uint_t u )
{
    if ( u > 65280 ) u -= 65248;
    return ( ( u >= 65 && u <= 90 )
	    || ( u >= 97 && u <= 122 ) );
}

/*
 * check if the given fstring is make up with numeric.
 *	both full-width,half-width numeric is ok.
 *
 * @param str
 * @return int
 * 65296, ０
 * 65297, １
 * 65298, ２
 * 65299, ３
 * 65300, ４
 * 65301, ５
 * 65302, ６
 * 65303, ７
 * 65304, ８
 * 65305, ９
 */
FRISO_API int utf8_numeric_string( const fstring str ) 
{
    fstring s = str;
    int bytes, u;

    while ( *s != '\0' ) {
	//if ( ! utf8_numeric_letter( get_utf8_unicode( s++ ) ) ) {
	//    return 0;
	//} 
	
	//new implemention.
	//@date 2013-10-14
	bytes = 1;
	if ( *s < 0 ) {	
	    //full-width chars.
	    u = get_utf8_unicode(s);
	    bytes = get_utf8_bytes(*s);
	    if ( u < 65296 || u > 65305 ) 
		return 0;
	} 
	else if ( *s < 48 || *s > 57 )
	    return 0;

	s += bytes;
    }

    return 1;
}

FRISO_API int utf8_decimal_string( const fstring str )
{
    int len = strlen(str), i, p = 0;
    int bytes = 0, u;

    if ( str[0] == '.' || str[len-1] == '.' ) return 0;

    for ( i = 1; i < len; bytes = 1 ) {
	//count the number of char '.'
	if ( str[i] == '.' ) p++;
	//full-width numeric.
	else if ( str[i] < 0 ) {
	    u = get_utf8_unicode(str+i);
	    bytes = get_utf8_bytes(str[i]);
	    if ( u < 65296 || u > 65305 ) 
		return 0;
	}
	else if ( str[i] < 48 || str[i] > 57 )
	    return 0;

	i += bytes;
    }

    return (p == 1);
}

/*
 * check the given char is a whitespace or not.
 * 
 * @param ch
 * @return int 1 for yes and 0 for not. 
 */
FRISO_API int utf8_whitespace( uint_t u ) 
{
    if ( u == 32 || u == 12288 )
	return 1;
    return 0;
}


/*
 * check the given char is a english punctuation.
 * 
 * @param ch
 * @return int 
 */
FRISO_API int utf8_en_punctuation( uint_t u ) 
{
    //if ( u > 65280 ) u = u - 65248;		//make full-width half-width
    return ( (u > 32 && u < 48)
	    || ( u > 57 && u < 65 )
	    || ( u > 90 && u < 97 )		//added @2013-08-31
	    || ( u > 122 && u < 127 ) );
}

/*
 * check the given char is a chinese punctuation.
 * @date	2013-08-31 added.
 *
 * @param ch
 * @return int 
 */
FRISO_API int utf8_cn_punctuation( uint_t u ) 
{
    return ( ( u > 65280 && u < 65296 )
	    || ( u > 65305 && u < 65312 )
	    || ( u > 65338 && u < 65345 )	
	    || ( u > 65370 && u < 65382 ) 
	    //cjk symbol and punctuation.(added 2013-09-06)
	    //from http://www.unicode.org/charts/PDF/U3000.pdf
	    || ( u >= 12289 && u <= 12319) );
}

/*
 * check if the given char is a letter number in unicode.
 *		like 'ⅠⅡ'.
 * @param ch
 * @return int
 */
FRISO_API int utf8_letter_number( uint_t u ) 
{
    return 0;
}

/*
 * check if the given char is a other number in unicode.
 *		like '①⑩⑽㈩'.
 * @param ch
 * @return int
 */
FRISO_API int utf8_other_number( uint_t u ) 
{
    return 0;
}

//A macro define has replace this.
//FRISO_API int is_en_punctuation( char c ) 
//{
//    return utf8_en_punctuation( (uint_t) c );
//}

/* {{{
   '@', '$','%', '^', '&', '-', ':', '.', '/', '\'', '#', '+'
   */
static friso_hash_t __keep_punctuations_hash__ = NULL;

/*check the given char is an english keep char.*/
FRISO_API int utf8_keep_punctuation( fstring str ) 
{
    if ( __keep_punctuations_hash__ == NULL ) {
	__keep_punctuations_hash__ = new_hash_table();
	hash_put_mapping( __keep_punctuations_hash__, "@", NULL );
	hash_put_mapping( __keep_punctuations_hash__, "$", NULL );
	hash_put_mapping( __keep_punctuations_hash__, "%", NULL );
	hash_put_mapping( __keep_punctuations_hash__, "^", NULL );
	hash_put_mapping( __keep_punctuations_hash__, "&", NULL );
	hash_put_mapping( __keep_punctuations_hash__, "-", NULL );
	hash_put_mapping( __keep_punctuations_hash__, ":", NULL );
	hash_put_mapping( __keep_punctuations_hash__, ".", NULL );
	hash_put_mapping( __keep_punctuations_hash__, "/", NULL );
	hash_put_mapping( __keep_punctuations_hash__, "'", NULL );
	hash_put_mapping( __keep_punctuations_hash__, "#", NULL );
	hash_put_mapping( __keep_punctuations_hash__, "+", NULL );
    }
    //check the hash.
    return hash_exist_mapping( __keep_punctuations_hash__, str );
}
/* }}} */

/*
 * check the given english char is a full-width char or not.
 * 
 * @param ch
 * @return 1 for yes and 0 for not. 
 */
FRISO_API int utf8_fullwidth_char( uint_t u ) 
{
    if ( u == 12288 )
	return 1;					//full-width space
    //(32 - 126) ascii code
    return (u > 65280 && u <= 65406);
}
