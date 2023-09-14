'''
Toy Python implementation of C's inet_pton()
'''

def inet_pton(ip):
    '''
    Convert IPv4 IP to 32-bit integer in big-endian Network Byte Order (e.g.,
    "0xAA/0xBB/0xCC/0xDD" for IPv4 address 0xAA.0xBB.0xCC.0xDD)
    '''
    # split `ip` at dot, convert each int to 8-bit bytestring, and concatenate
    binstr = "0b" + "".join(["{:08b}".format(int(int8)) for int8 in ip.split('.')])
    # convert 32-bit bytestring to int
    int32 = int(binstr, 2)

    return binstr, int32

def display_pton(ip="8.8.8.8"):
    '''
    Pretty-print results of inet_pton()
    '''
    binstr, int32 = inet_pton(ip)
    print("{:<15} -> {} ({})".format(ip, binstr, int32))

def inet_pton_bitwise(ip):
    '''
    Bitwise implementation of converter from IPv4 IP to 32-bit integer in 
    big-endian Network Byte Order 
    '''

    num = 0
    for chunk in ip.split('.'):
        num <<= 8
        num |= int(chunk)

    print("{:<15} -> {} ({})".format(ip, bin(num), num))

    return num

if __name__ == '__main__':

    display_pton("8.8.8.8")
    display_pton("31.13.70.36")
    display_pton("172.217.14.78")

    inet_pton_bitwise("8.8.8.8")
    inet_pton_bitwise("31.13.70.36")
    inet_pton_bitwise("172.217.14.78")
