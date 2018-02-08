#include "php_snuffleupagus.h"

#ifdef HAVE_PHP_SESSION

ZEND_DECLARE_MODULE_GLOBALS(snuffleupagus)

static int (*session_startup_func_orig)(INIT_FUNC_ARGS) = NULL; 
static ZEND_INI_MH((*OnUpdateSaveHandler_orig)) = NULL;

static int sp_hook_s_read(PS_READ_ARGS) {
	return SNUFFLEUPAGUS_G(session_mod_orig)->s_read(mod_data, key, val, maxlifetime);
}
static int sp_hook_s_write(PS_WRITE_ARGS) {
	return SNUFFLEUPAGUS_G(session_mod_orig)->s_write(mod_data, key, val, maxlifetime);
}

void hook_session_internal() {
	if (PS(mod) == NULL || SNUFFLEUPAGUS_G(session_mod) == PS(mod)) {
		return;
	}

	if (NULL == SNUFFLEUPAGUS_G(session_mod)) {
		SNUFFLEUPAGUS_G(session_mod) = emalloc(sizeof(ps_module));
		if (NULL == SNUFFLEUPAGUS_G(session_mod)) {
			return;
		}
	}
	memcpy(SNUFFLEUPAGUS_G(session_mod), PS(mod), sizeof(ps_module));
	SNUFFLEUPAGUS_G(session_mod_orig) = PS(mod);
	SNUFFLEUPAGUS_G(session_mod)->s_read = sp_hook_s_read;
	SNUFFLEUPAGUS_G(session_mod)->s_write = sp_hook_s_write;
	
	PS(mod) = SNUFFLEUPAGUS_G(session_mod);
}

static PHP_INI_MH(sp_OnUpdateSaveHandler) {
	if (stage == PHP_INI_STAGE_RUNTIME
			&& PS(session_status) == php_session_none
			&& SNUFFLEUPAGUS_G(session_mod_orig)
		  && zend_string_equals_literal(new_value, "user") == 0
			&& 0 == strcmp(((ps_module*)SNUFFLEUPAGUS_G(session_mod_orig))->s_name, "user")) {
		return SUCCESS;	
	}

	PS(mod) = SNUFFLEUPAGUS_G(session_mod_orig);

	int ret = OnUpdateSaveHandler_orig(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);

	hook_session_internal();

	return ret;
}

static int sp_session_startup_func(INIT_FUNC_ARGS) {
	return session_startup_func_orig(INIT_FUNC_ARGS_PASSTHRU);
}

void hook_session() {
	zend_module_entry *session_module = zend_hash_str_find_ptr(&module_registry, ZEND_STRL("session"));
	
	if (NULL == session_module) {
		return;
	}

	if (NULL != OnUpdateSaveHandler_orig) {
		return;
	}
	
	session_startup_func_orig = session_module->request_startup_func;
	session_module->request_startup_func = sp_session_startup_func;
	
	zend_ini_entry *ini_entry = zend_hash_str_find_ptr(EG(ini_directives), ZEND_STRL("session.save_handler"));
	if (ini_entry != NULL) {
		OnUpdateSaveHandler_orig = ini_entry->on_modify;
		ini_entry->on_modify = sp_OnUpdateSaveHandler;
	}

	SNUFFLEUPAGUS_G(session_mod_orig) = NULL;
	hook_session_internal();
}
#endif // HAVE_PHP_SESSION
