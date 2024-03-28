import json
import sys
import time
from typing import Union, Dict, List

import requests

def bxor(b1, b2):
    result = b""
    for b1, b2 in zip(b1, b2):
       result += bytes([b1 ^ b2])
    return result

def main():
    b1 = bytes([0x45])
    b2 = bytes([0x47])
    b3 = bytes([0x54])

    xor1 = bxor(b1, b2)
    xor2 = bxor(xor1, b3)

    print(xor2)
    print(xor2.decode('utf-8'))

if __name__ == '__main__':
    main()