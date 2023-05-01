#include"cvector.h"

void cv_init( c_vector* __v , long long __cap ){
    __v -> cap = __cap;
    __v -> len = 0;
    __v -> items = calloc( __v -> cap , sizeof( void* ) );
    if ( !__v -> items )
    {
        printf( "Memory allocate failed\n" );
        exit( 1 );
    }
    return;
}

void cv_resize( c_vector* __v , long long __cap ){
    __v -> cap = __cap;
    __v -> items = realloc( __v -> items , sizeof( void* ) * ( __v -> cap ) );
    if ( !__v -> items )
    {
        printf( "Memory allocate failed\n" );
        exit( 1 );
    }
    return;
}

void cv_push_front( c_vector* __v , void* __item ){
    if ( __v -> len == __v -> cap )
    {
        cv_resize( __v , __v -> cap * 2 );
    } // Capacity reached the upper limit, resize
    memmove( __v -> items + 1 , __v -> items , __v -> len * sizeof( void* ) );
    __v -> items[0] = __item;
    __v -> len++;
    return;
}

void cv_push_back( c_vector* __v , void* __item ){
    if ( __v -> len == __v -> cap )
    {
        cv_resize( __v , __v -> cap * 2 );
    } // Capacity reached the upper limit, resize
    __v -> items[__v->len++] = __item;
    return;
}

void* cv_pop_front( c_vector* __v ){
    if ( __v -> len < 1 )
    {
        return NULL;
    } // vector already empty
    void* ret = __v -> items[0];
    __v -> len--;
    memmove( __v -> items , __v -> items + 1 , __v -> len * sizeof( void* ) );
    if ( __v -> len < __v -> cap / 3 )
    {
        cv_resize( __v , __v -> cap / 2 );
    } // free excess space
    return ret;
}

void* cv_pop_back( c_vector* __v ){
    if ( __v -> len < 1 )
    {
        return NULL;
    }
    int last = __v -> len - 1;
    void* ret = __v -> items[last];
    __v -> items[last] = NULL;
    --__v -> len;
    if ( __v -> len < __v -> cap / 3 )
    {
        cv_resize( __v , __v -> cap / 2 );
    }
    return ret;
}

void cv_insert( c_vector* __v , void* __item , long long __pos ){
    if ( __pos == 0 )
    {
        cv_push_front( __v , __item );
    }
    else if ( __pos >= __v -> len )
    {
        cv_push_back( __v , __item );
    }
    else
    {
        if ( __v -> len == __v -> cap )
        {
            cv_resize( __v , __v -> cap * 2 );
        } // Capacity reached the upper limit, resize
        memmove( __v -> items + __pos + 1 , __v -> items + __pos , ( __v -> len - __pos ) * sizeof( void* ) );
        __v -> len++;
        __v -> items[__pos] = __item;
    }
    return;    
}

void* cv_delete( c_vector* __v , long long __pos ){
    if ( __v -> len == 0 || __v -> len <= __pos || __pos < 0 )
    {
        return NULL;
    }
    else if ( __v -> len - 1 == __pos )
    {
        return cv_pop_back( __v );
    }
    else if ( __pos == 0 )
    {
        return cv_pop_front( __v );
    }

    void* ret = __v -> items[__pos];
    memmove( __v -> items + __pos , __v -> items + __pos + 1 , ( __v -> len - __pos - 1 ) * sizeof( void* ) );
    __v -> len--;
    if ( __v -> len < __v -> cap / 3 )
    {
        cv_resize( __v , __v -> cap / 2 );
    } // free excess space
    return ret;
}

void cv_clean( c_vector* __v ){
    free( __v -> items );
    __v -> cap = 1;
    __v -> len = 0;
    return;
}

long long cv_len( c_vector* __v ){
    return __v -> len;
}

// debug functions

void cv_print_int( c_vector* __v ){
    for ( int i = 0 ; i < __v -> len ; i++ )
    {
        printf( "%d\n" , ( int ) __v -> items[i] );
    }
    return;
}

void cv_print_str( c_vector* __v ){
    for ( int i = 0 ; i < __v -> len ; i++ )
    {
        printf( "%s\n" , ( char* ) __v -> items[i] );
    }
    return;
}