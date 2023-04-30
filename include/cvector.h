#ifndef _CVECTOR_VECTOR_H_
#define _CVECTOR_VECTOR_H_

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    long long cap;  // capacity
    long long len;  // length
    void** items;   // items
} c_vector;

void cv_init( c_vector* __v , long long __cap );
void cv_resize( c_vector* __v , long long __cap );
void cv_push_front( c_vector* __v , void* __item );
void cv_push_back( c_vector* __v , void* __item );
void* cv_pop_front( c_vector* __v );
void* cv_pop_back( c_vector* __v );
void cv_insert( c_vector* __v , void* __item , long long __pos );
void* cv_delete( c_vector* __v , long long __pos );
void cv_clean( c_vector* __v );
long long cv_len( c_vector* __v );

void cv_print_int( c_vector* __v );
void cv_print_str( c_vector* __v );

#ifdef __cplusplus
}
#endif

#endif // _CVECTOR_VECTOR_H_