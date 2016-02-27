/*
  Copyright (C) 2016 by Syohei YOSHIDA

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <emacs-module.h>

#include <libmemcached/memcached.h>

int plugin_is_GPL_compatible;

static void
el_memcached_free(void *data)
{
	memcached_st *mst = (memcached_st*)data;
	memcached_free(mst);
}

static char*
retrieve_string(emacs_env *env, emacs_value str, ptrdiff_t *size)
{
	*size = 0;

	env->copy_string_contents(env, str, NULL, size);
	char *p = malloc(*size);
	if (p == NULL) {
		*size = 0;
		return NULL;
	}
	env->copy_string_contents(env, str, p, size);

	return p;
}

static emacs_value
Fmemcached_init(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	emacs_value servers = args[0];
	ptrdiff_t size;

	char *servers_str = retrieve_string(env, servers, &size);
	if (servers_str == NULL)
		return env->intern(env, "nil");

	memcached_st *mst = memcached_create(NULL);
	memcached_server_st *msv = memcached_servers_parse(servers_str);
	memcached_return mrt = memcached_server_push(mst, msv);

	free(servers_str);

	if (mrt != MEMCACHED_SUCCESS) {
		return env->intern(env, "nil");
	}
	memcached_server_list_free(msv);

	return env->make_user_ptr(env, el_memcached_free, mst);
}

static emacs_value
Fmemcached_close(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	memcached_st *mst = env->get_user_ptr(env, args[0]);
	memcached_free(mst);
	return env->intern(env, "t");
}

static emacs_value
el_memcached_set_common(emacs_env *env, memcached_st *mst,
			emacs_value key, emacs_value val, emacs_value expire, bool is_add)
{
	ptrdiff_t key_size;
	char *key_str = retrieve_string(env, key, &key_size);
	if (key_str == NULL)
		return env->intern(env, "nil");

	ptrdiff_t val_size = 0;
	char *val_str = retrieve_string(env, val, &val_size);
	if (val_str == NULL)
		return env->intern(env, "nil");

	time_t expr = (time_t)env->extract_integer(env, expire);
	memcached_return mrt;
	if (is_add) {
		mrt = memcached_add(mst, key_str, key_size-1, val_str, val_size-1, expr, 0);
	} else {
		mrt = memcached_set(mst, key_str, key_size-1, val_str, val_size-1, expr, 0);
	}

	free(key_str);
	free(val_str);

	if (mrt != MEMCACHED_SUCCESS && mrt != MEMCACHED_BUFFERED) {
		return env->intern(env, "nil");
	}

	return env->make_integer(env, (intmax_t)mrt);
}

static emacs_value
Fmemcached_set(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	memcached_st *mst = env->get_user_ptr(env, args[0]);
	emacs_value key = args[1];
	emacs_value value = args[2];
	emacs_value expire = args[3];

	return el_memcached_set_common(env, mst, key, value, expire, false);
}

static emacs_value
Fmemcached_add(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	memcached_st *mst = env->get_user_ptr(env, args[0]);
	emacs_value key = args[1];
	emacs_value value = args[2];
	emacs_value expire = args[3];

	return el_memcached_set_common(env, mst, key, value, expire, true);
}

static emacs_value
Fmemcached_get(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	memcached_st *mst = env->get_user_ptr(env, args[0]);
	emacs_value key = args[1];
	ptrdiff_t key_size;
	char *key_str = retrieve_string(env, key, &key_size);

	size_t ret_size;
	uint32_t flags;
	memcached_return mrt;

	char *val = memcached_get(mst, key_str, key_size-1, &ret_size, &flags, &mrt);
	free(key_str);

	if (mrt != MEMCACHED_SUCCESS && mrt != MEMCACHED_BUFFERED) {
		return env->intern(env, "nil");
	}

	emacs_value ret = env->make_string(env, val, ret_size);
	free(val);

	return ret;
}

static emacs_value
Fmemcached_delete(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	memcached_st *mst = env->get_user_ptr(env, args[0]);
	emacs_value key = args[1];

	ptrdiff_t key_size;
	char *key_str = retrieve_string(env, key, &key_size);

	time_t expire = (time_t)env->extract_integer(env, args[2]);
	memcached_return mrt = memcached_delete(mst, key_str, key_size-1, expire);

	free(key_str);

	if (mrt != MEMCACHED_SUCCESS && mrt != MEMCACHED_BUFFERED) {
		return env->intern(env, "intern");
	}

	return env->intern(env, "t");
}

static emacs_value
Fmemcached_flush(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	memcached_st *mst = env->get_user_ptr(env, args[0]);
	time_t expire = env->extract_integer(env, args[2]);

	memcached_return mrt = memcached_flush(mst, expire);
	if (mrt != MEMCACHED_SUCCESS)
		return env->intern(env, "nil");

	return env->intern(env, "t");
}

static emacs_value
Fmemcached_flush_buffers(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	memcached_st *mst = env->get_user_ptr(env, args[0]);

	memcached_return mrt = memcached_flush_buffers(mst);
	if (mrt != MEMCACHED_SUCCESS)
		return env->intern(env, "nil");

	return env->intern(env, "t");
}

static void
bind_function(emacs_env *env, const char *name, emacs_value Sfun)
{
	emacs_value Qfset = env->intern(env, "fset");
	emacs_value Qsym = env->intern(env, name);
	emacs_value args[] = { Qsym, Sfun };

	env->funcall(env, Qfset, 2, args);
}

static void
provide(emacs_env *env, const char *feature)
{
	emacs_value Qfeat = env->intern(env, feature);
	emacs_value Qprovide = env->intern (env, "provide");
	emacs_value args[] = { Qfeat };

	env->funcall(env, Qprovide, 1, args);
}

int
emacs_module_init(struct emacs_runtime *ert)
{
	emacs_env *env = ert->get_environment(ert);

#define DEFUN(lsym, csym, amin, amax, doc, data) \
	bind_function (env, lsym, env->make_function(env, amin, amax, csym, doc, data))

	DEFUN("memcached-core-init", Fmemcached_init, 1, 1, "Initialize memcached", NULL);
	DEFUN("memcached-close", Fmemcached_close, 1, 1, "Close memcached connection", NULL);
	DEFUN("memcached-core-set",  Fmemcached_set, 4, 4, "Set value", NULL);
	DEFUN("memcached-core-add",  Fmemcached_add, 4, 4, "Add value", NULL);
	DEFUN("memcached-core-get",  Fmemcached_get, 2, 2, "Get value", NULL);
	DEFUN("memcached-core-delete",  Fmemcached_delete, 3, 3, "Delete value", NULL);
	DEFUN("memcached-core-flush",  Fmemcached_flush, 2, 2, "Flush memcached", NULL);
	DEFUN("memcached-flush-buffers",  Fmemcached_flush_buffers, 1, 1,
	      "Flush memcached buffers", NULL);

#undef DEFUN

	provide(env, "memcached-core");
	return 0;
}

/*
  Local Variables:
  c-basic-offset: 8
  indent-tabs-mode: t
  End:
*/
