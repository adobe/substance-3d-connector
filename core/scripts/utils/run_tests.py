#!/usr/bin/env python

""" Utility script to run all of the substance connector tests
    that are built out. """

import sys
import os
import subprocess

def run_tests(root_dir, build_dir='build'):
    """ Run all of the substance connector test executables """
    _test_dir = os.path.join(root_dir, build_dir, 'bin', 'substance_connector')

    _targets = os.listdir(_test_dir)

    for _target in _targets:
        _path = os.path.join(_test_dir, _target)

        if os.path.isfile(_path):
            retcode = subprocess.call([_path])

            if retcode == 0:
                print("\033[92m" + '> Good: Test program {} succeeded.'.format(_target) + "\033[0m")
            else:
                print("\033[91m" + '>  Error: Test program {} failed.'.format(_target) + "\033[0m")

def main(argv):
    root_dir = os.getcwd()
    build_dir = 'build'

    for i in range(1, len(argv)):
        if (argv[i] == '--root' or argv[i] == '-r') and i < (len(argv) - 1):
            root_dir = argv[i+1]
            i = i + 1
        elif (argv[i] == '--build' or argv[i] == '-b') and i < (len(argv) - 1):
            build_dir = argv[i+1]
            i = i + 1

    run_tests(root_dir, build_dir)

if __name__ == '__main__':
    main(sys.argv)
