#!/usr/bin/python3
# coding: latin-1
blob = """
                +w� X����!>�l<Y9?7$��;<U�H��� >�?�Kd���9%EA�a)��=�q���d��oy܂�-:���T3Lߡ�-��&`����:���zF��?b��474l�(\���r�

"""
from hashlib import sha256
print(sha256(blob.encode("latin-1")).hexdigest())