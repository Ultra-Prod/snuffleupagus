#include "php_snuffleupagus.h"

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

static int sp_hook_s_read(PS_READ_ARGS) {
	return SNUFFLEUPAGUS_G(session_mod_orig)->s_read(mod_data, key, val, maxlifetime);
}
static int sp_hook_s_write(PS_WRITE_ARGS) {
	return SNUFFLEUPAGUS_G(session_mod_orig)->s_write(mod_data, key, val, maxlifetime);
}

void sp_hook_session() {
	if (PS(mod) == NULL || SNUFFLEUPAGUS_G(session_mod) == PS(mod)) {
		return;
	}

	if (NULL == SNUFFLEUPAGUS_G(session_mod)) {
		SNUFFLEUPAGUS_G(session_mod) = emalloc(sizeof(ps_module));
		if (NULL == SNUFFLEUPAGUS_G(session_mod)) {
			return;
		}
	}

	SNUFFLEUPAGUS_G(session_mod_orig) = PS(mod);
	memcpy(SNUFFLEUPAGUS_G(session_mod), SNUFFLEUPAGUS_G(session_mod_orig),
			sizeof(ps_module));

	SNUFFLEUPAGUS_G(session_mod)->s_read = sp_hook_s_read;

	SNUFFLEUPAGUS_G(session_mod)->s_write = sp_hook_s_write;
	
	PS(mod) = SNUFFLEUPAGUS_G(session_mod);
}
