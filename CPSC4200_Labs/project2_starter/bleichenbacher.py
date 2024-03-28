#!/usr/bin/python3

# Run me like this:
# $ python3 bleichenbacher.py "coach+username+100.00"
# or select "Bleichenbacher" from the VS Code debugger

from roots import *

import hashlib
import sys


def main():
    if len(sys.argv) < 2:
        print(f"usage: {sys.argv[0]} MESSAGE", file=sys.stderr)
        sys.exit(-1)
    message = sys.argv[1]

    #
    # TODO: Forge a signature
    #

    # The sender’s public key is 2048 bits long and uses e = 3
    # e is the exponent from the key
    e = 3

    # 00 01 | FF | 00 | ASN.1 "magic" bytes denoting type of hash alg | SHA-256 digest | k/8 - 55 = 2048/8 − 54 =
    #                                                                                    202 bytes
    beginning = '0001'
    ff_bytes = 'ff'
    zeros = '00'
    # asn1 taken from the examle in the homework document
    asn1 = '3031300d060960864801650304020105000420'
    # the last 201 bytes are set to be 00
    end_garbage = '00' * 201

    #hash stuff (i.e. the digest of the target message)
    m = hashlib.sha256()
    m.update(message.encode())
    mhash = m.hexdigest()

    constructed_to_forge = beginning + ff_bytes + zeros + asn1 + mhash + end_garbage

    # take the cube root, rounding as appropriate
    forged = integer_nthroot(int(constructed_to_forge, 16), e)
    if (forged[1]):
        forged_signature = forged[0]
    else:
        forged_signature = forged[0] + 1 

    print(bytes_to_base64(integer_to_bytes(forged_signature, 256)))


if __name__ == '__main__':
    main()

