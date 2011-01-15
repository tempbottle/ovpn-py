/*
 * OpenVPN embedded Python V1 plug-in
 *
 * Author: Ivan Nudzik <ivan.nudzik@gmail.com>
 * License: GPLv3
 *
 * Feel comfortable! Write OpenVPN plugins in the Python!
 */

#include <stdio.h>
#include <dlfcn.h>
#include <Python.h>
#include "openvpn-plugin.h"

struct plugin_context {
	void *pydlh;
	const PyObject *module;
	const PyObject *plugin_up;
	const PyObject *plugin_down;
	const PyObject *route_up;
	const PyObject *ipchange;
	const PyObject *tls_verify;
	const PyObject *auth_user_pass_verify;
	const PyObject *client_connect;
	const PyObject *client_disconnect;
	const PyObject *learn_address;
	const PyObject *tls_final;
	const PyObject *enable_pf;
};

OPENVPN_EXPORT openvpn_plugin_handle_t openvpn_plugin_open_v1(
		unsigned int *type_mask, const char *argv[], const char *envp[]) {
	PyObject *tmp;
	int asz;
	struct plugin_context *context;

	*type_mask = OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_UP)
			| OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_DOWN);

	context
			= (struct plugin_context *) calloc(1, sizeof(struct plugin_context));
	context->pydlh = dlopen("libpython2.6.so", RTLD_NOW | RTLD_GLOBAL);
	if (context->pydlh == NULL) {
		fprintf(stderr, dlerror());
		return NULL;
	}

	Py_Initialize();
	for (asz = 0; argv[asz] != NULL; ++asz);
	if (asz < 2)
		return NULL;
	PySys_SetArgv(asz, argv);
	if (asz > 2)
		PySys_SetPath(argv[2]);
	tmp = PyString_FromString(argv[1]);
	context->module = PyImport_Import(tmp);
	if (context->module == NULL) {
		PyErr_Print();
		return NULL;
	}
	Py_DECREF(tmp);
	tmp = PyModule_GetDict((PyObject*) context->module);
	context->plugin_up = PyDict_GetItemString(tmp, "plugin_up");
	if (context->plugin_up == NULL || !PyCallable_Check(
			(PyObject*) context->plugin_up)) {
		PyErr_Print();
		Py_DECREF(tmp);
		return NULL;
	}
	context->plugin_down = PyDict_GetItemString(tmp, "plugin_down");
	if (context->plugin_down == NULL || !PyCallable_Check(
			(PyObject*) context->plugin_down)) {
		PyErr_Print();
		Py_DECREF(tmp);
		return NULL;
	}
	context->route_up = PyDict_GetItemString(tmp, "route_up");
	if (context->route_up && PyCallable_Check((PyObject*) context->route_up)) {
		*type_mask |= OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_ROUTE_UP);
	}
	context->ipchange = PyDict_GetItemString(tmp, "ipchange");
	if (context->ipchange && PyCallable_Check((PyObject*) context->ipchange)) {
		*type_mask |= OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_IPCHANGE);
	}
	context->tls_verify = PyDict_GetItemString(tmp, "tls_verify");
	if (context->tls_verify
			&& PyCallable_Check((PyObject*) context->tls_verify)) {
		*type_mask |= OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_TLS_VERIFY);
	}
	context->auth_user_pass_verify = PyDict_GetItemString(tmp,
			"auth_user_pass_verify");
	if (context->auth_user_pass_verify && PyCallable_Check(
			(PyObject*) context->auth_user_pass_verify)) {
		*type_mask |= OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY);
	}
	context->client_connect = PyDict_GetItemString(tmp, "client_connect");
	if (context->client_connect && PyCallable_Check(
			(PyObject*) context->client_connect)) {
		*type_mask |= OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_CLIENT_CONNECT_V2);
	}
	context->client_disconnect = PyDict_GetItemString(tmp, "client_disconnect");
	if (context->client_disconnect && PyCallable_Check(
			(PyObject*) context->client_disconnect)) {
		*type_mask |= OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_CLIENT_DISCONNECT);
	}
	context->learn_address = PyDict_GetItemString(tmp, "learn_address");
	if (context->learn_address && PyCallable_Check(
			(PyObject*) context->learn_address)) {
		*type_mask |= OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_LEARN_ADDRESS);
	}
	context->tls_final = PyDict_GetItemString(tmp, "tls_final");
	if (context->tls_final && PyCallable_Check((PyObject*) context->tls_final)) {
		*type_mask |= OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_TLS_FINAL);
	}
	context->enable_pf = PyDict_GetItemString(tmp, "enable_pf");
	if (context->enable_pf && PyCallable_Check((PyObject*) context->enable_pf)) {
		*type_mask |= OPENVPN_PLUGIN_MASK(OPENVPN_PLUGIN_ENABLE_PF);
	}
	Py_DECREF(tmp);
	return (openvpn_plugin_handle_t) context;
}

OPENVPN_EXPORT int openvpn_plugin_func_v1(openvpn_plugin_handle_t handle,
		const int type, const char *argv[], const char *envp[]) {
	PyObject *vals = NULL, *retval = NULL, *env = NULL;
	int size = 0, cnt;
	struct plugin_context *context = (struct plugin_context *) handle;
	char *tmpstr;

	for (size = 0; argv[size] != NULL; ++size);
	vals = PyTuple_New(size);
	env = PyDict_New();
	for (cnt = 0; envp[cnt] != NULL; ++cnt) {
		int esz = strlen(envp[cnt]);
		tmpstr = calloc(1, (esz + 2));
		strncpy(tmpstr, envp[cnt], esz);
		char *spos = strchr(tmpstr, '=');
		*spos = 0;
		PyDict_SetItemString(env, tmpstr, PyString_FromString(spos + 1));
		free(tmpstr);
	}
	PyTuple_SetItem(vals, 0, env);
	for (cnt = 1; argv[cnt] != NULL; ++cnt) {
		PyTuple_SetItem(vals, cnt, PyString_FromString(argv[cnt]));
	}
	switch (type) {
	case OPENVPN_PLUGIN_UP:
		retval = PyObject_CallObject((PyObject*) context->plugin_up, vals);
		break;
	case OPENVPN_PLUGIN_DOWN:
		retval = PyObject_CallObject((PyObject*) context->plugin_down, vals);
		break;
	case OPENVPN_PLUGIN_ROUTE_UP:
		retval = PyObject_CallObject((PyObject*) context->route_up, vals);
		break;
	case OPENVPN_PLUGIN_IPCHANGE:
		retval = PyObject_CallObject((PyObject*) context->ipchange, vals);
		break;
	case OPENVPN_PLUGIN_TLS_VERIFY:
		retval = PyObject_CallObject((PyObject*) context->tls_verify, vals);
		break;
	case OPENVPN_PLUGIN_AUTH_USER_PASS_VERIFY:
		retval = PyObject_CallObject(
				(PyObject*) context->auth_user_pass_verify, vals);
		break;
	case OPENVPN_PLUGIN_CLIENT_CONNECT_V2:
		retval = PyObject_CallObject((PyObject*) context->client_connect, vals);
		break;
	case OPENVPN_PLUGIN_CLIENT_DISCONNECT:
		retval = PyObject_CallObject((PyObject*) context->client_disconnect,
				vals);
		break;
	case OPENVPN_PLUGIN_LEARN_ADDRESS:
		retval = PyObject_CallObject((PyObject*) context->learn_address, vals);
		break;
	case OPENVPN_PLUGIN_TLS_FINAL:
		retval = PyObject_CallObject((PyObject*) context->tls_final, vals);
		break;
	case OPENVPN_PLUGIN_ENABLE_PF:
		retval = PyObject_CallObject((PyObject*) context->enable_pf, vals);
		break;
	default:
		printf("OPENVPN_PLUGIN_?\n");
		break;
	}
	Py_DECREF(env);
	Py_DECREF(vals);
	if (retval == NULL) {
		PyErr_Print();
		return OPENVPN_PLUGIN_FUNC_ERROR;
	}
	if (PyObject_IsTrue(retval)) {
		Py_DECREF(retval);
		return OPENVPN_PLUGIN_FUNC_SUCCESS;
	} else {
		Py_DECREF(retval);
		return OPENVPN_PLUGIN_FUNC_ERROR;
	}
}

OPENVPN_EXPORT void openvpn_plugin_close_v1(openvpn_plugin_handle_t handle) {
	struct plugin_context *context = (struct plugin_context *) handle;
	Py_DECREF((PyObject*) context->module);
	Py_Finalize();
	dlclose(context->pydlh);
	free(context);
}

OPENVPN_EXPORT int openvpn_plugin_select_initialization_point_v1() {
	return OPENVPN_PLUGIN_INIT_PRE_DAEMON;
}
