/*
 * File:   main.c
 * Author: nick
 *
 * Created on 28 Сентябрь 2010 г., 20:52
 */



#include <stdio.h>
#include <stdlib.h>
#include <python2.6/Python.h>
#include <dlfcn.h>
#include <stdlib.h>
#ifdef __i386__
#include "agava/linux2.6.x-i386-32/agava.h"
#else
#include "agava/linux2.6.x-i386-64/agava.h"
#endif



#define True    1
#define False   0


static PyObject *Error;

static struct Agava_GOST_28147_Params params;

inline void *_gost3411(PyObject *self, PyObject *args, int hex) {
    const char *in = NULL;
    if (!PyArg_ParseTuple(args, "s", &in)) {
        PyErr_SetString(PyExc_TypeError, "args must be string");
        return NULL;
    }

    struct Agava_Random_State rnd;
    char key[32] = {0};
    struct Agava_GOST_R3411 hash={0};
    unsigned char hash_buf[32] = {0};
    unsigned char hash_buf_hex[64] = {0};
    int i = 0;

    Agava_Random(&rnd, key, 32);
    Agava_GOST_R3411_init(&hash, &params, &rnd, NULL);
    Agava_GOST_R3411_update(&hash, in, strlen(in));
    Agava_GOST_R3411_final(&hash, hash_buf);
    Agava_GOST_R3411_clear(&hash);
    if (hex) {
        for (i = 0; i <= 32; i++) {
            hash_buf_hex[i * 2] = (hash_buf[i] >> 4 << 4) >> 4;
            hash_buf_hex[i * 2 + 1] = hash_buf[i] >> 4 << 4^hash_buf[i];
            hash_buf_hex[i * 2] += hash_buf_hex[i * 2] < 10 ? 0x30 : 0x57;
            hash_buf_hex[i * 2 + 1] += hash_buf_hex[i * 2 + 1] < 10 ? 0x30 : 0x57;
        }
        return Py_BuildValue("s#", hash_buf_hex, 64);
    } else {
        return Py_BuildValue("s#", hash_buf, 32);
    }

}

static PyObject *gost3411_hex(PyObject *self, PyObject *args) {
    return ((PyObject *) _gost3411(self, args, True));
}

static PyObject *gost3411(PyObject *self, PyObject *args) {
    return ((PyObject *) _gost3411(self, args, False));
}


static PyMethodDef agava_methods[] = {
    {"gost3411_hex", gost3411_hex, METH_VARARGS, "return hexifly hash string"},
    {"gost3411", gost3411, METH_VARARGS, "return binary hash"},
    {NULL, NULL, 0, NULL}
};

void initagava() {
    PyObject *module;

    unsigned char _GostR341194_CryptoProParamSet[64] = {
        0xA5, 0x74, 0x77, 0xD1, 0x4F, 0xFA, 0x66, 0xE3,
        0x54, 0xC7, 0x42, 0x4A, 0x60, 0xEC, 0xB4, 0x19,
        0x82, 0x90, 0x9D, 0x75, 0x1D, 0x4F, 0xC9, 0x0B,
        0x3B, 0x12, 0x2F, 0x54, 0x79, 0x08, 0xA0, 0xAF,
        0xD1, 0x3E, 0x1A, 0x38, 0xC7, 0xB1, 0x81, 0xC6,
        0xE6, 0x56, 0x05, 0x87, 0x03, 0x25, 0xEB, 0xFE,
        0x9C, 0x6D, 0xF8, 0x6D, 0x2E, 0xAB, 0xDE, 0x20,
        0xBA, 0x89, 0x3C, 0x92, 0xF8, 0xD3, 0x53, 0xBC
    };
    memset(&params, 0, sizeof (struct Agava_GOST_28147_Params));
    Agava_GOST_28147_params_init(&params, _GostR341194_CryptoProParamSet);


    module = Py_InitModule("agava", agava_methods);
    Error = PyErr_NewException("agava.error", NULL, NULL);
    Py_INCREF(Error);
    PyModule_AddObject(module, "error", Error);

}
