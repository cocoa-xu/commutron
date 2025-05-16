#ifndef COMMUTRON_ENCODE_PYTHON_HPP
#define COMMUTRON_ENCODE_PYTHON_HPP

#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <optional>
#include <vector>
#include "commutron.hpp"

namespace commutron {

static std::optional<ERL_NIF_TERM> encode_python(ErlNifEnv *env, PyObject * dict);
static std::optional<ERL_NIF_TERM> encode_python_dict(ErlNifEnv *env, PyObject * dict);
static std::optional<ERL_NIF_TERM> encode_python_tuple(ErlNifEnv *env, PyObject * dict);
static std::optional<ERL_NIF_TERM> encode_python_list(ErlNifEnv *env, PyObject * list);

static std::optional<ERL_NIF_TERM> encode_python(ErlNifEnv *env, PyObject * val) {
  auto ret = std::nullopt;
  if (val == nullptr) return ret;
  if (val == Py_None) return commutron::k_atom_nil;
  if (val == Py_False) return commutron::k_atom_false;
  if (val == Py_True) return commutron::k_atom_true;
  if (PyLong_Check(val)) return commutron::commutron.enif_make_int64(commutron::commutron.env, PyLong_AsLong(val));
  if (PyFloat_Check(val)) return commutron::commutron.enif_make_double(commutron::commutron.env, PyFloat_AsDouble(val));
  if (PyUnicode_Check(val)) {
    Py_ssize_t size;
    const char * data = PyUnicode_AsUTF8AndSize(val, &size);
    if (data == nullptr) return ret;
    ERL_NIF_TERM string_val;
    unsigned char * ptr;
    if ((ptr = commutron::commutron.enif_make_new_binary(env, size, &string_val)) != nullptr) {
      strncpy((char *)ptr, data, size);
      return string_val;
    }
    return ret;
  }
  if (PyDict_Check(val)) return encode_python_dict(commutron::commutron.env, val);
  if (PyTuple_Check(val)) return encode_python_tuple(commutron::commutron.env, val);
  if (PyList_Check(val)) return encode_python_list(commutron::commutron.env, val);

  // PyObject* type_obj = PyObject_Type(val);
  // PyObject* type_name_obj = PyObject_GetAttrString(type_obj, "__name__");
  // Py_DECREF(type_obj);
  // Py_DECREF(type_name_obj);

  return k_atom_nil;
}

static std::optional<ERL_NIF_TERM> encode_python_dict(ErlNifEnv *env, PyObject * dict) {
  auto ret = std::nullopt;
  PyObject *keys = PyDict_Keys(dict);
  if (keys == nullptr) return ret;

  Py_ssize_t size = PyList_Size(keys);
  std::vector<ERL_NIF_TERM> erl_keys, erl_values;
  for (Py_ssize_t i = 0; i < size; ++i) {
    PyObject *key = PyList_GetItem(keys, i);
    auto key_term = encode_python(env, key);
    if (!key_term) return ret;
    erl_keys.emplace_back(key_term.value());

    auto val = PyDict_GetItem(dict, key);
    auto val_term = encode_python(env, val);
    if (!val_term) return ret;
    erl_values.emplace_back(val_term.value());
  }
  Py_DECREF(keys);

  ERL_NIF_TERM erl_dict;
  if (commutron::commutron.enif_make_map_from_arrays(env, erl_keys.data(), erl_values.data(), erl_keys.size(), &erl_dict)) {
    return erl_dict;
  }
  return ret;
}

static std::optional<ERL_NIF_TERM> encode_python_tuple(ErlNifEnv *env, PyObject * tuple) {
  auto ret = std::nullopt;
  Py_ssize_t size = PyTuple_Size(tuple);
  std::vector<ERL_NIF_TERM> erl_values;
  for (Py_ssize_t i = 0; i < size; ++i) {
    PyObject * item = PyTuple_GetItem(tuple, i);
    auto item_term = encode_python(env, item);
    if (!item_term) return ret;
    erl_values.emplace_back(item_term.value());
  }
  ERL_NIF_TERM erl_tuple = commutron::commutron.enif_make_tuple_from_array(env, erl_values.data(), erl_values.size());
  return erl_tuple;
}

static std::optional<ERL_NIF_TERM> encode_python_list(ErlNifEnv *env, PyObject * list) {
  auto ret = std::nullopt;
  Py_ssize_t size = PyList_Size(list);
  std::vector<ERL_NIF_TERM> erl_values;
  for (Py_ssize_t i = 0; i < size; ++i) {
    PyObject * item = PyList_GetItem(list, i);
    auto item_term = encode_python(env, item);
    if (!item_term) return ret;
    erl_values.emplace_back(item_term.value());
  }
  ERL_NIF_TERM erl_list = commutron::commutron.enif_make_list_from_array(env, erl_values.data(), erl_values.size());
  return erl_list;
}

}  // namespace commutron

#endif  // COMMUTRON_ENCODE_PYTHON_HPP
