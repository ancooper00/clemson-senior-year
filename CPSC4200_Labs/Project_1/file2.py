#!/usr/bin/python3
# coding: latin-1
blob = """
                +w� X����!>�l<Y9�7$��;<U�H��� >�?�Kd��G:%EA�a)�g=�q���d��oy܂�-:���T�Lߡ�-��&`����:���zF�̿b��474l�(\�B�r�

"""
from hashlib import sha256
print(sha256(blob.encode("latin-1")).hexdigest())