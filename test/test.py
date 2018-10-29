#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import tempfile
import unittest
import shutil
import os

import sandbox


class SandboxTest(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.mkdtemp()

    def tearDown(self):
        shutil.rmtree(self.directory, ignore_errors=True)

    def test_success(self):
        result = sandbox.run('test_success')
        self.assertEqual(result["result"], sandbox.RESULT_SUCCESS)

    def test_stdout(self):
        stdout = os.path.join(self.directory, 'stdout.txt')
        result = sandbox.run('test_stdout', output_path=stdout)

        with open(stdout) as f:
            self.assertEqual(f.read(), 'stdout message')

    def test_stderr(self):
        stderr = os.path.join(self.directory, 'stderr.txt')
        result = sandbox.run('test_stderr', error_path=stderr)

        with open(stderr) as f:
            self.assertEqual(f.read(), 'stderr message')

    def test_cpu_timeout(self):
        result = sandbox.run('test_cpu_timeout', max_cpu_time=1000)
        self.assertEqual(result["result"], sandbox.RESULT_TIME_LIMIT_EXCEEDED)
        self.assertGreaterEqual(result["cpu_time"], 1000)

    def test_real_timeout(self):
        result = sandbox.run('test_real_timeout', max_real_time=1000)
        self.assertEqual(result["result"], sandbox.RESULT_REAL_TIME_LIMIT_EXCEEDED)
        self.assertGreaterEqual(result["real_time"], 1000)

    def test_memory_limit_1(self):
        result = sandbox.run('test_memory_limit_1',
            max_memory=1048576*16,
            memory_check_only=True)
        self.assertEqual(result["result"], sandbox.RESULT_MEMORY_EXCEEDED)
        self.assertGreaterEqual(result["memory"], 1048576 * 16)

    def test_output_limit(self):
        stdout = os.path.join(self.directory, 'stdout.txt')
        result = sandbox.run('test_output_limit',
            max_output_size=1048576,
            output_path=stdout)
        self.assertEqual(result["result"], sandbox.RESULT_RUNTIME_ERROR)

    def test_runtime_error(self):
        result = sandbox.run('test_runtime_error')
        self.assertEqual(result["result"], sandbox.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["exit_code"], 1)

    def test_deny_system_call(self):
        result = sandbox.run('system_call')
        self.assertEqual(result["result"], sandbox.RESULT_SUCCESS)

        result = sandbox.run('system_call', seccomp_rule=sandbox.RULE_CLIKE)
        self.assertEqual(result["result"], sandbox.RESULT_RUNTIME_ERROR)
        self.assertEqual(result["signal"], 31)  # SIGSYS


if __name__ == '__main__':
    unittest.main()
    # unittest.main(verbosity=2)
