#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <optional>
#include <vector>
#include "commutron.hpp"
#include "encode_python.hpp"

using commutron::ErlNifEnv;
using commutron::ErlNifPid;
using commutron::ERL_NIF_TERM;
using commutron::encode_python;

typedef struct {
  PyObject_HEAD
  ErlNifEnv* env;
} PyErlNifEnvObject;

static PyObject*
PyErlNifEnvObject_new(PyTypeObject* type, PyObject* args, PyObject* kwds) {
  PyErr_SetString(PyExc_TypeError,
                  "Cannot create instances of PyErlNifEnvObject directly from Python");
  return NULL;
}

static PyMethodDef PyErlNifEnvObject_methods[] = {
  {NULL}
};

static void PyErlNifEnvObject_dealloc(PyErlNifEnvObject* self);

static PyTypeObject PyErlNifEnvObjectType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "commutron.PyErlNifEnvObject",
  .tp_basicsize = sizeof(PyErlNifEnvObject),
  .tp_itemsize = 0,
  .tp_dealloc = (destructor)PyErlNifEnvObject_dealloc,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_doc = "Erlang/OTP ErlNifEnv object wrapper",
  .tp_methods = PyErlNifEnvObject_methods,
  .tp_init = NULL,
  .tp_new = PyErlNifEnvObject_new,
};

static void PyErlNifEnvObject_dealloc(PyErlNifEnvObject* self) {
  if (self->env) {
    commutron::commutron.enif_free_env(self->env);
    self->env = nullptr;
  }

  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* commutron_enif_alloc_env(PyObject* self, PyObject* args) {
  PyErlNifEnvObject* env = (PyErlNifEnvObject*)PyErlNifEnvObjectType.tp_alloc(&PyErlNifEnvObjectType, 0);
  if (!env) return nullptr;
  
  env->env = commutron::commutron.enif_alloc_env();
  return (PyObject*)env;
}

static PyObject* commutron_enif_free_env(PyObject* self, PyObject* args) {
  PyObject *object;
  if (!PyArg_ParseTuple(args, "O!", &object)) return nullptr;
  if (!PyObject_TypeCheck(object, &PyErlNifEnvObjectType)) {
    PyErr_SetString(PyExc_TypeError, "Expected a PyErlNifEnvObject");
    return nullptr;
  }
  PyErlNifEnvObject* env = (PyErlNifEnvObject*)object;
  if (env->env) {
    commutron::commutron.enif_free_env(env->env);
    env->env = nullptr;
  }
  Py_RETURN_NONE;
}

static PyObject* commutron_enif_send(PyObject* self, PyObject* args) {
  void* target_pid;
  Py_ssize_t length;
  PyObject* message;

  if (!PyArg_ParseTuple(args, "y#O", &target_pid, &length, &message)) return nullptr;

  auto msg = encode_python(commutron::commutron.env, message);
  if (!msg) {
    PyErr_SetString(PyExc_RuntimeError, "Failed to encode message");
    return nullptr;
  }
  commutron::commutron.enif_send(nullptr, (ErlNifPid*)target_pid, commutron::commutron.env, msg.value());
  Py_RETURN_NONE;
}

static PyObject* commutron_enif_whereis_pid(PyObject* self, PyObject* args) {
  PyObject *object;
  if (!PyArg_ParseTuple(args, "O", &object)) return nullptr;
  if (PyUnicode_Check(object)) {
    Py_ssize_t size;
    const char * data = PyUnicode_AsUTF8AndSize(object, &size);
    if (data == nullptr) {
      PyErr_SetString(PyExc_RuntimeError, "Failed to get name");
      return nullptr;
    }

    ERL_NIF_TERM name_term;
    if (!commutron::commutron.enif_make_existing_atom_len(commutron::commutron.env, data, size, &name_term, commutron::ERL_NIF_LATIN1)) {
      name_term = commutron::commutron.enif_make_atom_len(commutron::commutron.env, data, size);
    }

    auto size_of_erl_nif_pid = sizeof(ErlNifPid);
    ErlNifPid* pid = (ErlNifPid*)malloc(size_of_erl_nif_pid);
    if (!pid) {
      PyErr_SetString(PyExc_RuntimeError, "Failed to allocate ErlNifPid");
      return nullptr;
    }
    memset(pid, 0, size_of_erl_nif_pid);

    int result = commutron::commutron.enif_whereis_pid(nullptr, name_term, pid);
    if (result) {
      PyObject* py_pid = PyBytes_FromStringAndSize((const char*)pid, size_of_erl_nif_pid);
      free(pid);
      if (!py_pid) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create ErlNifPid");
        return nullptr;
      }
      return py_pid;
    } else {
      Py_RETURN_NONE;
    }
  }

  PyErr_SetString(PyExc_TypeError, "Expected a string");
  return nullptr;
}

static PyMethodDef methods[] = {
  {"enif_alloc_env",
    (PyCFunction)commutron_enif_alloc_env, 
    METH_VARARGS, 
    PyDoc_STR(
      "Allocates a new process independent environment. \
      The environment can be used to hold terms that are not bound to any \
      process. Such terms can later be copied to a process environment with \
      enif_make_copy or be sent to a process as a message with enif_send.")},
  {"enif_free_env", 
    (PyCFunction)commutron_enif_free_env, 
    METH_VARARGS, 
    PyDoc_STR("Frees an environment allocated with enif_alloc_env. All terms \
      created in the environment are freed as well.")},
  {"enif_send", 
    (PyCFunction)commutron_enif_send,
    METH_VARARGS,
    PyDoc_STR("Sends a message to a process.")},
  {"enif_whereis_pid", 
    (PyCFunction)commutron_enif_whereis_pid,
    METH_VARARGS,
    PyDoc_STR("Looks up a process by its registered name.")},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef module = {
  PyModuleDef_HEAD_INIT,
  "core",
  "Commutron between Python and Erlang/OTP based languages.",
  0,
  methods,
};

PyMODINIT_FUNC PyInit_core(void) {
  PyObject* m;
    
  if (PyType_Ready(&PyErlNifEnvObjectType) < 0) return nullptr;
  
  m = PyModule_Create(&module);
  if (!m) return nullptr;
  
  Py_INCREF(&PyErlNifEnvObjectType);
  if (PyModule_AddObject(m, "PyErlNifEnvObject", (PyObject*)&PyErlNifEnvObjectType) < 0) {
    Py_DECREF(&PyErlNifEnvObjectType);
    Py_DECREF(m);
    return nullptr;
  }

  return m;
}
