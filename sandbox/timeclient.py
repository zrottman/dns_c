'''
Adapted from Beej's Guide to Network Concepts
https://beej.us/guide/bgnet0/
'''

import socket, datetime

def system_seconds_since_1900():
    seconds_delta = 2208988800
    seconds_since_unix_epoch = int(datetime.datetime.now().strftime("%s"))
    seconds_since_1900_epoch = seconds_since_unix_epoch + seconds_delta
    return seconds_since_1900_epoch

# open socket
s = socket.socket()

# connect to time.nist.gov on port 37
s.connect(("time.nist.gov", 37))

# receive 4 bytes of TCP data
d = s.recv(4)

print("data received: {}".format(d))
print("NIST time    : {}".format(int.from_bytes(d, byteorder="big")))
print("System time  : {}".format(system_seconds_since_1900()))

# close socket
s.close()
