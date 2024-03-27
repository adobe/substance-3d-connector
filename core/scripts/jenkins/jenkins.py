#!/usr/bin/env python

import sys
import os
import subprocess
import shutil
import platform

import connector.build.cmake

from connector.build.arg_process import process_single_arguments

def _build_connector(parameters):
    """ Build the project with the current configuration """
    config = 'Release'
    platform = ''

    current_dir = os.getcwd()

    # Make the build directory and enter it
    build_directory = os.path.join(current_dir, 'build')

    if os.path.isdir(build_directory):
        shutil.rmtree(build_directory)

    os.makedirs(build_directory)
    os.chdir(build_directory)

    if 'config' in parameters:
        config = parameters['config']

    if 'platform' in parameters:
        platform = parameters['platform']
    else:
        print('Failed - no platform specified')
        sys.exit(1)

    if 'generator' in parameters:
        generator = parameters['generator']
    else:
        generator = connector.build.cmake.get_generator(platform)

    # Execute cmake with the proper arguments
    cmake_args = ['-G', generator, '--config',
                  '-DSUBSTANCE_CONNECTOR_BUILD_TESTS=1', config, current_dir]
    result = connector.build.cmake.build(cmake_args)

    if result != 0:
        print('Failed - Building project Substance Connector failed.')
        sys.exit(1)

    # Restore the directory to the original
    os.chdir(current_dir)

def _is_executable(path):
    """ Returns true if the file is an executable, false otherwise """
    result = False

    result = os.access(path, os.X_OK)

    if 'Windows' in platform.system():
        # If windows, then the file should end with .exe
        name, extension = os.path.splitext(path)

        result = result and extension == '.exe'

    return result

def _run_recursive_test(path):
    """ Runs tests recursively through a directory """
    result = True

    subpaths = os.listdir(path)

    for element in subpaths:
        location = os.path.join(path, element)

        if os.path.isdir(location):
            result = result and _run_recursive_test(location)
        elif os.path.isfile(location) and _is_executable(location):
            return_value = subprocess.call([location])

            success = (return_value == 0)

            if success:
                print('Running test executable {} succeeded.'.format(location))
            else:
                print('Error - Running test executable {} failed.'.format(location))

            result = (result and success)

    return result

def _run_tests():
    """ Build and run the testing code """
    result = False

    # Acquire the build directory
    current_dir = os.getcwd()
    build_directory = os.path.join(current_dir, 'build')

    # Run the cmake build for the test targets
    test_target = 'substance_connector_all_tests'
    cmake_args = ['--build', build_directory,
                  '--target', test_target]

    result = (subprocess.call(['cmake'] + cmake_args) == 0)

    # Get all of the executables
    if result:
        test_directory = os.path.join(build_directory, 'bin', 'substance_connector')

        result = _run_recursive_test(test_directory)

    return result

def main(argv):
    """ Main function entry point """
    # List of tuples containing the flag and the alias
    arg_map = {}

    _single_arguments = [
        ('--config', 'config'),
        ('--platform', 'platform'),
        ('--generator', 'generator'),
    ]

    process_single_arguments(argv, _single_arguments, arg_map)

    # Compile the module
    _build_connector(arg_map)

    # Run all of the tests and ensure that they pass
    result = _run_tests()

    if not result:
        print('Failed to run tests.')
        sys.exit(1)

if __name__ == '__main__':
    main(sys.argv)
