#include"cli.hpp"

/**
 * @brief start cGPTerm CLI
*/
int start_CLI(){
    while ( 1 )
    {
        std::string input;
        std::cout << "> ";
        std::cin >> input;
        std::cout << input << std::endl;
        if ( input == "quit" )
        {
            return 0;
        }
    }
}