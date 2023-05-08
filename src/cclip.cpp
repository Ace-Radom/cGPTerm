#include"cclip.hpp"

int clipboard_copy( const char* __text ){
    if ( !__text )
        return 1;
    clip::set_text( std::string( __text ) );
    return 0;
}