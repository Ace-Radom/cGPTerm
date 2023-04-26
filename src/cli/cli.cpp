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
        if ( input.size() == 0 )
            continue;
        if ( input == "quit" )
            break;
        
        turn_off_echo();
        write_ANSI( HIDE_CURSOR );
        // turn off echo; hide cursor
        openai_datatransfer_t data;
        data.msg = input.c_str();
        data.response = NULL;
        // build transfer data
        std::thread send_request( openai_send_chatrequest , &data );
        std::this_thread::sleep_for( std::chrono::microseconds( 10 ) );
        // start request; wait 10 ms in order to let openai_send_chatrequest to lock request_working (-> true)
        while ( request_working )
        {
            print_wait_msg( "ChatGPT is thinking" );
        } // until request done: print wait msg
        std::cout << "\r                             \r" << std::flush;
        send_request.join();
        reset_terattr();
        write_ANSI( SHOW_CURSOR );
        // request join; reset attr; show cursor
        // clean wait msg
        if ( data.response )
        {
            if ( HTTP_Response_code / 100 != 4 )
                std::cout << "ChatGPT:" << std::endl << data.response << std::endl;
            else
            {
                std::cout << "Request Error: " << data.response << std::endl;
                openai_msg_popback();
            } // request error, pop last user's msg
        }
        else
            openai_msg_popback();
        // same: request error, pop last user's msg
    }
    return 0;
}