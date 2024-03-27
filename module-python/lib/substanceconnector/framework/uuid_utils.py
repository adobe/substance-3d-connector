""" Utility functions for converting Python UUID objects into data that the
    native layer will consume """

import uuid

def convert_uuid_to_integers(uuid_object):
    """ Converts a Python uuid object into an array of 32-bit integers that
        the native layer can use """
    result = []

    uuid_int = uuid_object.int
    result.append((uuid_int & (0xffffffff << 96)) >> 96)
    result.append((uuid_int & (0xffffffff << 64)) >> 64)
    result.append((uuid_int & (0xffffffff << 32)) >> 32)
    result.append(uuid_int & 0xffffffff)

    return result

def convert_integers_to_uuid(uuid_ints):
    """ Converts the 32-bit integers received from the native layer into a
        Python uuid object """
    uuid_int = uuid_ints[3]
    uuid_int |= (uuid_ints[2] << 32)
    uuid_int |= (uuid_ints[1] << 64)
    uuid_int |= (uuid_ints[0] << 96)

    result = uuid.UUID(int=uuid_int)

    return result
