#include"cli.hpp"

/**
 * @brief start cGPTerm CLI
*/
int start_CLI(){
    while ( 1 )
    {
        std::string input;
        std::cout << "> ";
        std::getline( std::cin , input );
        if ( input == "quit" )
        {
            break;
        }
        openai_send_chatrequest( input.c_str() );
    }
    return 0;
}