#include <Python.h>

#include "src/exception.h"
#include "src/runner.h"

SandboxConfig gen_defualt_config() {
  SandboxConfig config;

  config.max_cpu_time = UNLIMITED;
  config.max_real_time = UNLIMITED;
  config.max_memory = UNLIMITED;
  config.max_stack = UNLIMITED;
  config.max_process_number = UNLIMITED;
  config.max_output_size = UNLIMITED;
  config.memory_check_only = false;
  config.target = nullptr;
  config.stdin = nullptr;
  config.stdout = nullptr;
  config.stderr = nullptr;
  config.chroot = nullptr;
  config.seccomp_rule = kNoneRule;
  config.uid = 0;
  config.gid = 0;
  for (int i=0; i < ARGS_MAX_NUMBER; i++)
    config.args[i] = nullptr;

  return config;
}

static PyObject* sandbox_run(PyObject *self, PyObject *args, PyObject *keywds) {
  const char* kwlist[] = {
    "target",
    "max_cpu_time",
    "max_real_time",
    "max_memory",
    "max_stack",
    "max_process_number",
    "max_output_size",
    "memory_check_only",
    "stdin",
    "stdout",
    "stderr",
    "chroot",
    "args",
    "seccomp_rule",
    "uid",
    "gid",
    nullptr
  };

  SandboxConfig config = gen_defualt_config();
  PyObject *listObj = nullptr;
  unsigned rule;

  if (!PyArg_ParseTupleAndKeywords(args, keywds, "s|$IIIIIIpssssOIii",
      const_cast<char**>(kwlist),
      &config.target,
      &config.max_cpu_time,
      &config.max_real_time,
      &config.max_memory,
      &config.max_stack,
      &config.max_process_number,
      &config.max_output_size,
      &config.memory_check_only,
      &config.stdin,
      &config.stdout,
      &config.stderr,
      &config.chroot,
      &listObj,
      &rule,
      &config.uid,
      &config.gid)) {
    return NULL;
  }

  config.args[0] = config.target;

  if (listObj) {
    unsigned length = PyList_Size(listObj);
    for (unsigned i=0; i < length; i++) {
      PyObject *temp = PyList_GetItem(listObj, i);
      config.args[i+1] = PyUnicode_AsUTF8(temp);
    }
  }

  config.seccomp_rule = static_cast<SeccompRule>(rule);

  SandboxResult result;
  try {
    result = run(config);
  }
  catch (SandboxException &e) {
    result.result = kSystemError;
    result.error = e.error_code_;
  }

  PyObject *dictObj = PyDict_New();
  PyDict_SetItem(dictObj,
      PyUnicode_FromString("cpu_time"),
      Py_BuildValue("I", result.cpu_time));
  PyDict_SetItem(dictObj,
      PyUnicode_FromString("real_time"),
      Py_BuildValue("I", result.real_time));
  PyDict_SetItem(dictObj,
      PyUnicode_FromString("memory"),
      Py_BuildValue("I", result.memory));
  PyDict_SetItem(dictObj,
      PyUnicode_FromString("signal"),
      Py_BuildValue("i", result.signal));
  PyDict_SetItem(dictObj,
      PyUnicode_FromString("exit_code"),
      Py_BuildValue("i", result.exit_code));
  PyDict_SetItem(dictObj,
      PyUnicode_FromString("result"),
      Py_BuildValue("I", result.result));
  PyDict_SetItem(dictObj,
      PyUnicode_FromString("error"),
      Py_BuildValue("I", result.error));

  return dictObj;
}

static PyMethodDef ExportMethods[] = {
  {"run", (PyCFunction)sandbox_run, METH_KEYWORDS | METH_VARARGS,
      "Run application in sandbox"},
  {nullptr, nullptr, 0, nullptr}
};

static PyModuleDef ModuleDef = {
  PyModuleDef_HEAD_INIT,
  "sandbox",
  "Run application in X sandbox",
  -1,
  ExportMethods
};

PyMODINIT_FUNC PyInit_sandbox() {
  PyObject *module = PyModule_Create(&ModuleDef);

  PyModule_AddIntConstant(module, "RULE_NONE", kNoneRule);
  PyModule_AddIntConstant(module, "RULE_CLIKE", kClikeRule);

  PyModule_AddIntConstant(module, "RESULT_SUCCESS", kSuccess);
  PyModule_AddIntConstant(module, "RESULT_TIME_LIMIT_EXCEEDED", kTimeLimitExceeded);
  PyModule_AddIntConstant(module, "RESULT_REAL_TIME_LIMIT_EXCEEDED", kRealTimeLimitExceeded);
  PyModule_AddIntConstant(module, "RESULT_MEMORY_EXCEEDED", kMemoryExceeded);
  PyModule_AddIntConstant(module, "RESULT_RUNTIME_ERROR", kRuntimeError);
  PyModule_AddIntConstant(module, "RESULT_SYSTEM_ERROR", kSystemError);

  PyModule_AddIntConstant(module, "ERROR_DEFAULT", kDefaultError);
  PyModule_AddIntConstant(module, "ERROR_ROOT_REQUIRED", kRootRequired);
  PyModule_AddIntConstant(module, "ERROR_FORK_FAILED", kForkFailed);
  PyModule_AddIntConstant(module, "ERROR_SETRLIMIT_FAILED", kSetuidFailed);
  PyModule_AddIntConstant(module, "ERROR_DUP2_FAILED", kDup2Failed);
  PyModule_AddIntConstant(module, "ERROR_SETUID_FAILED", kSetuidFailed);
  PyModule_AddIntConstant(module, "ERROR_CHROOT_FAILED", kChrootFailed);
  PyModule_AddIntConstant(module, "ERROR_LOAD_SECCOMP_FAILED", kLoadSeccompFailed);
  PyModule_AddIntConstant(module, "ERROR_EXECVE_FAILED", kExecveFailed);
  PyModule_AddIntConstant(module, "ERROR_PTHREAD_FAILED", kPthreadFailed);
  PyModule_AddIntConstant(module, "ERROR_WAIT4_FAILED", kWaitFailed);

  return module;
}
