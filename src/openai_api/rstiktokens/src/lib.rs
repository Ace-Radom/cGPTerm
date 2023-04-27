extern crate libc;
use libc::c_char;
use std::ffi::CStr;

use tiktoken_rs::cl100k_base;

/// Count tokens of a message (cl100k_base)
#[no_mangle]
pub extern "C" fn count_tokens_cl100k_base( ptr: *const c_char ) -> usize {
    let c_str = unsafe { CStr::from_ptr( ptr ) };
    let convert_result = c_str.to_str();
    let msg = match convert_result {
        Ok( s ) => s,
        Err( _e ) => {
            return 0;
        }
    };
    // change C string into a Rust string

    let bpe = cl100k_base().unwrap();
    let tokens = bpe.encode_with_special_tokens( msg );
    return tokens.len();
}
