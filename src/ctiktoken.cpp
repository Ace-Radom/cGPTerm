#include"ctiktoken.hpp"

std::shared_ptr<GptEncoding> encoder_cl100k_base;

void cout_tokens_init(){
    encoder_cl100k_base = GptEncoding::get_encoding( LanguageModel::CL100K_BASE );
    return;
}

size_t count_tokens( const char* __text ){
    if ( __text == NULL )
        return 0;
    std::string text( __text );
    auto tokens = encoder_cl100k_base -> encode( text );
    return tokens.size();
}