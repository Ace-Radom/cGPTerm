#include"cli.hpp"

/**
 * @brief start cGPTerm CLI
*/
int start_CLI(){
    get_original_terattr();

    while ( 1 )
    {
        std::string input;
        std::cout << "> ";
        std::getline( std::cin , input );
        if ( input == "quit" )
        {
            break;
        }
        char* response;
        turn_off_echo();
        write_ANSI( HIDE_CURSOR );
        openai_send_chatrequest( input.c_str() , response );
        // while ( request_working )
        // {
        //     print_wait_msg( "ChatGPT is thinking" );
        // }
        reset_terattr();
        write_ANSI( SHOW_CURSOR );
        if ( response )
        {
            std::cout << response << std::endl;
        }
    }
    return 0;
}