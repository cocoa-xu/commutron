#ifndef COMMUTRON_MINI_OTP_H
#define COMMUTRON_MINI_OTP_H

#pragma once

#include <cstdint>
#include <cstdio>

namespace commutron {

typedef enum {
  ERL_NIF_LATIN1,
  ERL_NIF_UTF8,
} ErlNifCharEncoding;

typedef unsigned long ERL_NIF_TERM;
typedef void ErlNifEnv;
typedef struct {
  ERL_NIF_TERM pid;
} ErlNifPid;

#pragma push_macro("ERL_FUNC")
#undef ERL_FUNC
#define ERL_FUNC(return, name) typedef return (*name##_t)

ERL_FUNC(void*, enif_alloc_env)(void);
ERL_FUNC(void, enif_free_env)(ErlNifEnv* env);
ERL_FUNC(ERL_NIF_TERM, enif_make_atom)(ErlNifEnv* env, const char* atom);
ERL_FUNC(ERL_NIF_TERM, enif_make_atom_len)(ErlNifEnv* env, const char* atom, size_t len);
ERL_FUNC(ERL_NIF_TERM, enif_make_double)(ErlNifEnv* env, double value);
ERL_FUNC(ERL_NIF_TERM, enif_make_existing_atom)(ErlNifEnv* env, const char* atom, ERL_NIF_TERM* term, int flags);
ERL_FUNC(ERL_NIF_TERM, enif_make_existing_atom_len)(ErlNifEnv* env, const char* atom, size_t len, ERL_NIF_TERM* term, int flags);
ERL_FUNC(ERL_NIF_TERM, enif_make_int)(ErlNifEnv* env, int value);
ERL_FUNC(ERL_NIF_TERM, enif_make_int64)(ErlNifEnv* env, int64_t value);
ERL_FUNC(ERL_NIF_TERM, enif_make_list_from_array)(ErlNifEnv* env, const ERL_NIF_TERM* arr, int cnt);
ERL_FUNC(ERL_NIF_TERM, enif_make_pid)(ErlNifEnv* env, const ErlNifPid* pid);
ERL_FUNC(int, enif_make_map_from_arrays)(ErlNifEnv* env, const ERL_NIF_TERM* keys, const ERL_NIF_TERM* values, unsigned cnt, ERL_NIF_TERM* map_out);
ERL_FUNC(unsigned char*, enif_make_new_binary)(ErlNifEnv* env, size_t size, ERL_NIF_TERM* termp);
ERL_FUNC(ERL_NIF_TERM, enif_make_tuple_from_array)(ErlNifEnv* env, const ERL_NIF_TERM* arr, unsigned cnt);
ERL_FUNC(int, enif_send)(ErlNifEnv* caller_env, ErlNifPid* to_pid, ErlNifEnv* msg_env, ERL_NIF_TERM msg);
ERL_FUNC(int, enif_whereis_pid)(ErlNifEnv *caller_env, ERL_NIF_TERM name, ErlNifPid *pid);
ERL_FUNC(int, enif_snprintf)(char * buffer, size_t size, const char *format, ...);

#pragma pop_macro("ERL_FUNC")

}  // namespace commutron

#endif  // COMMUTRON_MINI_OTP_H
