/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Santiago Lizardo <santiagolizardo@php.net>                  |
  |          Remi Collet <remi@php.net>                                  |
  +----------------------------------------------------------------------+
*/

/* $Id$ */ 

/*
  +----------------------------------------------------------------------+
  | wrapper for SDL2/SDL_video.h (only SDL_GLContext)                    |
  +----------------------------------------------------------------------+
  | class SDL_GLContext                                                  |
  +----------------------------------------------------------------------+
*/

/* Functions not wrapped

 extern DECLSPEC int SDLCALL SDL_GL_LoadLibrary(const char *path);
 extern DECLSPEC void *SDLCALL SDL_GL_GetProcAddress(const char *proc);
 extern DECLSPEC void SDLCALL SDL_GL_UnloadLibrary(void);
*/

#include "php_sdl.h"
#include "glcontext.h"
#include "window.h"

static zend_class_entry *php_sdl_glcontext_ce;
static zend_object_handlers php_sdl_glcontext_handlers;
struct php_sdl_glcontext {
	zend_object   zo;
	SDL_GLContext glcontext;
	Uint32        flags;
};


/* {{{ get_php_sdl_glcontext_ce */
zend_class_entry *get_php_sdl_glcontext_ce(void)
{
	return php_sdl_glcontext_ce;
}
/* }}} */


#define FETCH_GLCONTEXT(__ptr, __id, __check) \
{ \
        intern = (struct php_sdl_glcontext *)zend_object_store_get_object(__id TSRMLS_CC);\
        __ptr = intern->glcontext; \
        if (__check && !__ptr) {\
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid %s object", intern->zo.ce->name);\
                RETURN_FALSE;\
        }\
}

/* {{{ sdl_glcontext_to_zval */
zend_bool sdl_glcontext_to_zval(SDL_GLContext glcontext, zval *z_val, Uint32 flags, char *buf TSRMLS_DC)
{
	if (glcontext) {
		struct php_sdl_glcontext *intern;

		object_init_ex(z_val, php_sdl_glcontext_ce);
		intern = (struct php_sdl_glcontext *)zend_object_store_get_object(z_val TSRMLS_CC);
		intern->glcontext = glcontext;
		intern->flags     = flags;

		return 1;
	}
	ZVAL_NULL(z_val);
	return 0;
}
/* }}} */


/* {{{ zval_to_sdl_glcontext */
SDL_GLContext zval_to_sdl_glcontext(zval *z_val TSRMLS_DC)
{
	struct php_sdl_glcontext *intern;

	if (Z_TYPE_P(z_val) == IS_OBJECT && Z_OBJCE_P(z_val) == php_sdl_glcontext_ce) {
		intern = (struct php_sdl_glcontext *)zend_object_store_get_object(z_val TSRMLS_CC);
		return intern->glcontext;
	}
	return NULL;
}
/* }}} */


/* {{{ php_sdl_glcontext_free
	 */
static void php_sdl_glcontext_free(void *object TSRMLS_DC)
{
	struct php_sdl_glcontext *intern = (struct php_sdl_glcontext *) object;

	if (intern->glcontext) {
		if (!(intern->flags & SDL_DONTFREE)) {
			SDL_GL_DeleteContext(intern->glcontext);
		}
	}

	zend_object_std_dtor(&intern->zo TSRMLS_CC);
	efree(intern);
}
/* }}} */


/* {{{ php_sdl_glcontext_new
 */
static zend_object_value php_sdl_glcontext_new(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	struct php_sdl_glcontext *intern;

	intern = emalloc(sizeof(*intern));
	memset(intern, 0, sizeof(*intern));

	zend_object_std_init(&intern->zo, class_type TSRMLS_CC);
	object_properties_init(&intern->zo, class_type);

	intern->glcontext = NULL;

	retval.handle = zend_objects_store_put(intern, NULL, php_sdl_glcontext_free, NULL TSRMLS_CC);
	retval.handlers = (zend_object_handlers *) &php_sdl_glcontext_handlers;

	return retval;
}
/* }}} */


ZEND_BEGIN_ARG_INFO_EX(arginfo_SDL_GL_ExtensionSupported, 0, 0, 1)
       ZEND_ARG_INFO(0, extension)
ZEND_END_ARG_INFO()

/* {{{ proto bool SDL_GL_ExtensionSupported(string extension)

 *  \brief Return true if an OpenGL extension is supported for the current
 *         context.
 extern DECLSPEC SDL_bool SDLCALL SDL_GL_ExtensionSupported(const char
                                                            *extension);
 */
static PHP_FUNCTION(SDL_GL_ExtensionSupported)
{
	char *ext;
	int  *ext_len;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &ext, &ext_len)) {
		return;
	}
	RETVAL_BOOL(SDL_GL_ExtensionSupported(ext) == SDL_TRUE);
}
/* }}} */


ZEND_BEGIN_ARG_INFO_EX(arginfo_SDL_GL_SetAttribute, 0, 0, 2)
       ZEND_ARG_INFO(0, attr)
       ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

/* {{{ proto int SDL_GL_SetAttribute(int attr, int value)

 *  \brief Set an OpenGL window attribute before window creation.
 extern DECLSPEC int SDLCALL SDL_GL_SetAttribute(SDL_GLattr attr, int value);
 */
static PHP_FUNCTION(SDL_GL_SetAttribute)
{
	long attr, value;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &attr, &value)) {
		return;
	}
	RETVAL_LONG(SDL_GL_SetAttribute((SDL_GLattr)attr, (int)value));
}
/* }}} */


ZEND_BEGIN_ARG_INFO_EX(arginfo_SDL_GL_GetAttribute, 0, 0, 2)
       ZEND_ARG_INFO(0, attr)
       ZEND_ARG_INFO(1, value)
ZEND_END_ARG_INFO()

/* {{{ proto int SDL_GL_GetAttribute(int attr, int &value)

 *  \brief Get the actual value for an attribute from the current context.
 extern DECLSPEC int SDLCALL SDL_GL_GetAttribute(SDL_GLattr attr, int *value);
 */
static PHP_FUNCTION(SDL_GL_GetAttribute)
{
	long attr;
	zval *z_value;
	int ret, value;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lz", &attr, &z_value)) {
		return;
	}
	ret = SDL_GL_GetAttribute((SDL_GLattr)attr, &value);
	if (ret == 0) {
		zval_dtor(z_value);
		ZVAL_LONG(z_value, value);
	}
	RETVAL_LONG(ret);
}
/* }}} */


ZEND_BEGIN_ARG_INFO_EX(arginfo_SDL_GLContext__construct, 0, 0, 1)
       ZEND_ARG_OBJ_INFO(0, window, SDL_Window, 0)
ZEND_END_ARG_INFO()

/* {{{ proto SDL_GLContext, __construct(void) */
static PHP_METHOD(SDL_GLContext, __construct)
{
	struct php_sdl_glcontext *intern;
	zend_error_handling error_handling;
	zval *z_window;
	SDL_Window *window;

	intern = (struct php_sdl_glcontext *)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &z_window, get_php_sdl_window_ce())) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	window = zval_to_sdl_window(z_window TSRMLS_CC);
	if (window) {
		intern->glcontext = SDL_GL_CreateContext(window);
		if (intern->glcontext) {
			intern->flags = 0;
		} else {
			zend_throw_exception(zend_exception_get_default(TSRMLS_C), SDL_GetError(), 0 TSRMLS_CC);
		}
	} else {
		zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Invalid SDL_Window object", 0 TSRMLS_CC);
	}
}
/* }}} */


/* {{{ proto SDL_GLContext SDLCALL SDL_GL_CreateContext(SDL_Window window)

 *  \brief Create an OpenGL context for use with an OpenGL window, and make it
 *         current.
 *
 *  \sa SDL_GL_DeleteContext()
 extern DECLSPEC SDL_GLContext SDLCALL SDL_GL_CreateContext(SDL_Window *
                                                            window);
 */
static PHP_FUNCTION(SDL_GL_CreateContext)
{
	SDL_GLContext context;
	zval *z_window;
	SDL_Window *window;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &z_window, get_php_sdl_window_ce())) {
		return;
	}
	window = zval_to_sdl_window(z_window TSRMLS_CC);
	if (window) {
		context = SDL_GL_CreateContext(window);
		sdl_glcontext_to_zval(context, return_value, 0, NULL TSRMLS_CC);
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid SDL_Window object");
	}
}
/* }}} */


/* {{{ proto void SDLCALL SDL_GL_DeleteContext(SDL_GLContext context)

  *  \brief Delete an OpenGL context.
 *
 *  \sa SDL_GL_CreateContext()
 extern DECLSPEC void SDLCALL SDL_GL_DeleteContext(SDL_GLContext context);
 */
static PHP_FUNCTION(SDL_GL_DeleteContext)
{
	struct php_sdl_glcontext *intern;
	zval *z_context;
	SDL_GLContext context;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &z_context, php_sdl_glcontext_ce) == FAILURE) {
		return;
	}
	FETCH_GLCONTEXT(context, z_context, 1);

	SDL_GL_DeleteContext(intern->glcontext);
	intern->glcontext = NULL;
}
/* }}} */


ZEND_BEGIN_ARG_INFO_EX(arginfo_none, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_SDL_GLContext, 0, 0, 1)
       ZEND_ARG_OBJ_INFO(0, GLcontext, SDL_GLContext, 0)
ZEND_END_ARG_INFO()

/* {{{ sdl_glcontext_methods[] */
static const zend_function_entry php_sdl_glcontext_methods[] = {
	PHP_ME(SDL_GLContext,       __construct,                arginfo_SDL_GLContext__construct,     ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)

	PHP_FALIAS(Delete,          SDL_GL_DeleteContext,       arginfo_none)
	ZEND_FE_END
};
/* }}} */


/* {{{ sdl_glcontext_functions[] */
zend_function_entry sdl_glcontext_functions[] = {
	ZEND_FE(SDL_GL_ExtensionSupported,                   arginfo_SDL_GL_ExtensionSupported)
	ZEND_FE(SDL_GL_SetAttribute,                         arginfo_SDL_GL_SetAttribute)
	ZEND_FE(SDL_GL_GetAttribute,                         arginfo_SDL_GL_GetAttribute)
	ZEND_FE(SDL_GL_CreateContext,                        arginfo_SDL_GLContext__construct)
	ZEND_FE(SDL_GL_DeleteContext,                        arginfo_SDL_GLContext)
	ZEND_FE_END
};
/* }}} */

#define REGISTER_GL_CLASS_CONST_LONG(const_name, value) \
	REGISTER_LONG_CONSTANT("SDL_GL_" const_name, value, CONST_CS | CONST_PERSISTENT); \
	zend_declare_class_constant_long(php_sdl_glcontext_ce, const_name, sizeof(const_name)-1, value TSRMLS_CC)

#define REGISTER_GLCONTEXT_CLASS_CONST_LONG(const_name, value) \
	REGISTER_LONG_CONSTANT("SDL_GL_CONTEXT_" const_name, value, CONST_CS | CONST_PERSISTENT); \
	zend_declare_class_constant_long(php_sdl_glcontext_ce, const_name, sizeof(const_name)-1, value TSRMLS_CC)


/* {{{ MINIT */
PHP_MINIT_FUNCTION(sdl_glcontext)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "SDL_GLContext", php_sdl_glcontext_methods);
	ce.create_object = php_sdl_glcontext_new;
	php_sdl_glcontext_ce = zend_register_internal_class(&ce TSRMLS_CC);
	memcpy(&php_sdl_glcontext_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	/* typedef enum SDL_GLattr; */
	REGISTER_GL_CLASS_CONST_LONG("RED_SIZE",                       SDL_GL_RED_SIZE);
	REGISTER_GL_CLASS_CONST_LONG("GREEN_SIZE",                     SDL_GL_GREEN_SIZE);
	REGISTER_GL_CLASS_CONST_LONG("BLUE_SIZE",                      SDL_GL_BLUE_SIZE);
	REGISTER_GL_CLASS_CONST_LONG("ALPHA_SIZE",                     SDL_GL_ALPHA_SIZE);
	REGISTER_GL_CLASS_CONST_LONG("BUFFER_SIZE",                    SDL_GL_BUFFER_SIZE),
	REGISTER_GL_CLASS_CONST_LONG("DOUBLEBUFFER",                   SDL_GL_DOUBLEBUFFER);
	REGISTER_GL_CLASS_CONST_LONG("DEPTH_SIZE",                     SDL_GL_DEPTH_SIZE);
	REGISTER_GL_CLASS_CONST_LONG("STENCIL_SIZE",                   SDL_GL_STENCIL_SIZE);
	REGISTER_GL_CLASS_CONST_LONG("ACCUM_RED_SIZE",                 SDL_GL_ACCUM_RED_SIZE);
	REGISTER_GL_CLASS_CONST_LONG("ACCUM_GREEN_SIZE",               SDL_GL_ACCUM_GREEN_SIZE);
	REGISTER_GL_CLASS_CONST_LONG("ACCUM_BLUE_SIZE",                SDL_GL_ACCUM_BLUE_SIZE);
	REGISTER_GL_CLASS_CONST_LONG("ACCUM_ALPHA_SIZE",               SDL_GL_ACCUM_ALPHA_SIZE);
	REGISTER_GL_CLASS_CONST_LONG("STEREO",                         SDL_GL_STEREO);
	REGISTER_GL_CLASS_CONST_LONG("MULTISAMPLEBUFFERS",             SDL_GL_MULTISAMPLEBUFFERS);
	REGISTER_GL_CLASS_CONST_LONG("MULTISAMPLESAMPLES",             SDL_GL_MULTISAMPLESAMPLES),
	REGISTER_GL_CLASS_CONST_LONG("ACCELERATED_VISUAL",             SDL_GL_ACCELERATED_VISUAL);
	REGISTER_GL_CLASS_CONST_LONG("RETAINED_BACKING",               SDL_GL_RETAINED_BACKING);
	REGISTER_GL_CLASS_CONST_LONG("SHARE_WITH_CURRENT_CONTEXT",     SDL_GL_SHARE_WITH_CURRENT_CONTEXT);
	REGISTER_GL_CLASS_CONST_LONG("FRAMEBUFFER_SRGB_CAPABLE",       SDL_GL_FRAMEBUFFER_SRGB_CAPABLE);
	REGISTER_GLCONTEXT_CLASS_CONST_LONG("MAJOR_VERSION",           SDL_GL_CONTEXT_MAJOR_VERSION);
	REGISTER_GLCONTEXT_CLASS_CONST_LONG("MINOR_VERSION",           SDL_GL_CONTEXT_MINOR_VERSION);
	REGISTER_GLCONTEXT_CLASS_CONST_LONG("EGL",                     SDL_GL_CONTEXT_EGL);
	REGISTER_GLCONTEXT_CLASS_CONST_LONG("FLAGS",                   SDL_GL_CONTEXT_FLAGS);
	REGISTER_GLCONTEXT_CLASS_CONST_LONG("PROFILE_MASK",            SDL_GL_CONTEXT_PROFILE_MASK);

	/* typedef enum SDL_GLprofile; */
	REGISTER_GLCONTEXT_CLASS_CONST_LONG("PROFILE_CORE",            SDL_GL_CONTEXT_PROFILE_CORE);
	REGISTER_GLCONTEXT_CLASS_CONST_LONG("PROFILE_COMPATIBILITY",   SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	REGISTER_GLCONTEXT_CLASS_CONST_LONG("PROFILE_ES",              SDL_GL_CONTEXT_PROFILE_ES);

	/* typedef enum SDL_GLcontextFlag; */
	REGISTER_GLCONTEXT_CLASS_CONST_LONG("DEBUG_FLAG",              SDL_GL_CONTEXT_DEBUG_FLAG);
	REGISTER_GLCONTEXT_CLASS_CONST_LONG("FORWARD_COMPATIBLE_FLAG", SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	REGISTER_GLCONTEXT_CLASS_CONST_LONG("ROBUST_ACCESS_FLAG",      SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG);
	REGISTER_GLCONTEXT_CLASS_CONST_LONG("RESET_ISOLATION_FLAG",    SDL_GL_CONTEXT_RESET_ISOLATION_FLAG);

	return (zend_register_functions(NULL, sdl_glcontext_functions, NULL, MODULE_PERSISTENT TSRMLS_CC));
}
