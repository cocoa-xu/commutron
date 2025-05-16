#ifndef COMMUTRON_HPP
#define COMMUTRON_HPP

#include <dlfcn.h>
#include "mini-otp.h"

namespace commutron {

#pragma push_macro("ERL_FUNC")
#undef ERL_FUNC
#define ERL_FUNC(name) name##_t name;

static struct {
  ErlNifEnv* env;

  ERL_FUNC(enif_alloc_env);
  ERL_FUNC(enif_free_env);
  ERL_FUNC(enif_make_atom);
  ERL_FUNC(enif_make_atom_len);
  ERL_FUNC(enif_make_double);
  ERL_FUNC(enif_make_existing_atom);
  ERL_FUNC(enif_make_existing_atom_len);
  ERL_FUNC(enif_make_int);
  ERL_FUNC(enif_make_int64);
  ERL_FUNC(enif_make_list_from_array);
  ERL_FUNC(enif_make_pid);
  ERL_FUNC(enif_make_map_from_arrays);
  ERL_FUNC(enif_make_new_binary);
  ERL_FUNC(enif_make_tuple_from_array);
  ERL_FUNC(enif_send);
  ERL_FUNC(enif_whereis_pid);
  ERL_FUNC(enif_snprintf);
} commutron = {};

#pragma pop_macro("ERL_FUNC")

#pragma push_macro("STATIC_ATOM")
#define STATIC_ATOM(name) static ERL_NIF_TERM k_atom_##name

STATIC_ATOM(ok);
STATIC_ATOM(nil);
STATIC_ATOM(true);
STATIC_ATOM(false);
STATIC_ATOM(error);
STATIC_ATOM(struct);
STATIC_ATOM(type);
STATIC_ATOM(value);
STATIC_ATOM(traceback);

#pragma pop_macro("STATIC_ATOM")

void __attribute__((constructor)) init_commutron(void) {
#pragma push_macro("ERL_FUNC")
#undef ERL_FUNC
#define ERL_FUNC(name) commutron.name = (name##_t)dlsym(RTLD_DEFAULT, #name)
  ERL_FUNC(enif_alloc_env);
  ERL_FUNC(enif_free_env);
  ERL_FUNC(enif_make_atom);
  ERL_FUNC(enif_make_atom_len);
  ERL_FUNC(enif_make_double);
  ERL_FUNC(enif_make_existing_atom);
  ERL_FUNC(enif_make_existing_atom_len);
  ERL_FUNC(enif_make_int);
  commutron.enif_make_int64 = (enif_make_int64_t)dlsym(RTLD_DEFAULT, "enif_make_long");
  ERL_FUNC(enif_make_list_from_array);
  ERL_FUNC(enif_make_pid);
  ERL_FUNC(enif_make_map_from_arrays);
  ERL_FUNC(enif_make_new_binary);
  ERL_FUNC(enif_make_tuple_from_array);
  ERL_FUNC(enif_send);
  ERL_FUNC(enif_whereis_pid);
  ERL_FUNC(enif_snprintf);

  if (commutron.enif_alloc_env && commutron.enif_make_atom) {
    ErlNifEnv * env = commutron.env = commutron.enif_alloc_env();
    if (env) {
#pragma push_macro("STATIC_ATOM")
#define STATIC_ATOM(name) k_atom_##name = commutron.enif_make_atom(env, #name)
      STATIC_ATOM(ok);
      STATIC_ATOM(nil);
      STATIC_ATOM(true);
      STATIC_ATOM(false);
      STATIC_ATOM(error);
      STATIC_ATOM(struct);
      STATIC_ATOM(type);
      STATIC_ATOM(value);
      STATIC_ATOM(traceback);
#pragma pop_macro("STATIC_ATOM")
    }
  }
}

}  // namespace commutron

#endif  // COMMUTRON_HPP
