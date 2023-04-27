extern crate libc;
use libc::c_char;
use std::ffi::CStr;

use tiktoken_rs::cl100k_base;

#[no_mangle]
pub extern "C" fn count_tokens( ptr: *const c_char ) -> usize {
    let c_str = unsafe { CStr::from_ptr( ptr ) };
    let msg = c_str.to_str().expect( "" );

    let bpe = cl100k_base().unwrap();
    let tokens = bpe.encode_with_special_tokens( msg );
    return tokens.len();
}
