#!/usr/bin/python3

# Run me like this:
# $ python3 padding_oracle.py "http://cpsc4200.mpese.com/username/paddingoracle/verify" "5a7793d3..."
# or select "Padding Oracle" from the VS Code debugger

import json
import sys
import time
from typing import Union, Dict, List

import requests

# Imported libraries from code in assignment doc 
from Crypto.Cipher import AES
from Crypto.Hash import HMAC, SHA256
from Crypto.Random import get_random_bytes

# Create one session for each oracle request to share. This allows the
# underlying connection to be re-used, which speeds up subsequent requests!
s = requests.session()

#starter code beginning 
#return valid if padding is valid
#returns "invalid_encoding" if padding is wrong 
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
        except json.JSONDecodeError as e:
            sys.stderr.write("It's possible that the oracle server is overloaded right now, or that provided URL is wrong.\n")
            sys.stderr.write("If this keeps happening, check the URL. Perhaps your uniqname is not set.\n")
            sys.stderr.write("Retrying in 10 seconds...\n\n")
            time.sleep(10)
            continue

def bxor(b1, b2):
    result = b""
    for b1, b2 in zip(b1, b2):
       result += bytes([b1 ^ b2])
    return result

def decrypt(oracle_url, message) -> bytes:
    #get ciphertext into byte array
    encryptedMessage = bytearray(message)
    #determine message length and number of blocks
    mesLen = len(encryptedMessage)
    numBlocks = mesLen // AES.block_size
    #array to store decoded bytes
    decodedMes = bytearray(mesLen)

    #for all the blocks in the message
    for i in range(1, numBlocks + 1):
        print("at block ", i)
        #set starting point for block we are trying to decrypt
        curEncrypBlockEndIndex = mesLen - (i * AES.block_size) + AES.block_size - 1

        # check if at first byte of the block, if so use IV if known 
        if (i == numBlocks):
            break
        
        #reset padding to zero
        padding = 0
        #set changing block (previous block to what we are trying to decrypt)
        attackBlockIndex = mesLen - (i * AES.block_size) - 1
        #for all bytes in the block 
        for j in range(0, AES.block_size):
            print("at block index ", j)
            padding += 1
            #keep track of original byte in ciphertext
            oldCipherByte = encryptedMessage[attackBlockIndex - j] # C 
            #try byte values by brute force
            for newByteVal in range(0,256):
                print("val tried is: ", newByteVal)
                #CREATE NEW ATTACK BYTE NEED TO FIGURE THIS OUT
                intermediary = newByteVal ^ padding 
                attackByte = intermediary ^ oldCipherByte
                print("attack byte = ", attackByte)
                #replace original byte in ciphertext with new value
                encryptedMessage[attackBlockIndex - j] =  attackByte # C'
                #create section of message to send to oracle (exclude blocks we have already decrypted)
                oracleMessage = encryptedMessage[:curEncrypBlockEndIndex + 1]
                print("oracle replaced byte is ", encryptedMessage[attackBlockIndex - j] )
                #check with oracle, if padding is valid, invalid max exception will raise
                oracleStatus = oracle(oracle_url, [oracleMessage])[0]["status"]
                #if oracle(oracle_url, [oracleMessage])[0]["status"] == "invalid_mac":
                if oracleStatus == "invalid_mac":
                    print('oracle returned invalid mac')
                    # valid padding, solve for p ; P = padding ^ C' ^ C
                    decodedByte = padding ^ attackByte ^ oldCipherByte
                    #prepend decoded byte to list of decoded message 
                    decodedMes[curEncrypBlockEndIndex - j] = decodedByte

                    #set up padding for next bytes
                    encryptedMessage[attackBlockIndex - j] = oldCipherByte # put original cipher byte back
                    for k in range(0, padding):
                        print("set up new padding")
                        p = decodedMes[curEncrypBlockEndIndex - k] #decoded value for padding we are trying to set
                        c = encryptedMessage[attackBlockIndex - k] # get original cipher byte so we can do padding calc
                        #set previous solved bytes to be padded (C") ; C" = C ^ P ^ padding we want next 
                        paddingSetUpByte = c ^ p ^ (padding + 1)
                        #set changed byte to where it gives us the padding we want
                        encryptedMessage[attackBlockIndex - k] = paddingSetUpByte #C"
                        #set padding of final byte 
                        encryptedMessage[curEncrypBlockEndIndex - k] = padding+1 #padding we want next
                    break
                else:
                    #reset cipher byte and continue trying new values
                    print(oracleStatus)
                    encryptedMessage[attackBlockIndex - j] = oldCipherByte

    return decodedMes

def main():
    if len(sys.argv) != 3:
        print(f"usage: {sys.argv[0]} ORACLE_URL CIPHERTEXT_HEX", file=sys.stderr)
        sys.exit(-1)
    oracle_url, message = sys.argv[1], bytes.fromhex(sys.argv[2])

    if oracle(oracle_url, [message])[0]["status"] != "valid":
        print("Message invalid", file=sys.stderr)

    #
    # TODO: Decrypt the message
    #
    decrypted = decrypt(oracle_url, message)

    #remove padding and mac from decrypted message 

    print(decrypted.decode('utf-8'))


if __name__ == '__main__':
    main()

