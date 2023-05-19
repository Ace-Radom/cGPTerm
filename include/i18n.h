#ifndef _I18N_H_
#define _I18N_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<locale.h>

#include<yaml.h>

#define _( TAG ) i18n_gettext( TAG )

extern const char* LANG;

void i18n_init( const char* __translation_prefix );
void i18n_set( const char* __l );
const char* i18n_gettext( const char* __tag );


#ifdef __cplusplus
}
#endif

#endif