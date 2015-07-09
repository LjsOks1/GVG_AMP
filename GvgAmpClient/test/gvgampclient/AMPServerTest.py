# Echo server program
import socket

HOST = ''                 # Symbolic name meaning the local host
PORT = 3811              # Arbitrary non-privileged port
ACK = "1001"
NACK = "1111"
l=""
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST, PORT))
s.listen(1)
conn, addr = s.accept()
print 'Connected by', addr
try:
    while 1:
        ch = conn.recv(1)
        if ch=="\n":
            print("Received {0}. ".format(l))
            if l[:4]=="CRAT":
                conn.send(ACK)
                print("Sent ACK.")
            if l=="CMDS0004a02a":  # ListFirstFolder
                conn.send("822a0008000634427269616eda") #return: 4Brian
                print("Sent 822a0008000634427269616eda")
            if l=="STOP0000":
                conn.send(ACK)
                print("Sent ACK.")
            if l=="CMDS0004a02b":  # ListNextFolders
                conn.send("822b003c0003443130000764656661756c7400054a616d657300076a6a617465737400056d6f68697400034d504700036e6577000b52656379636c6520426969")
                print("Sent 822b003c0003443130000764656661756c7400054a616d657300076a6a617465737400056d6f68697400034d504700036e6577000b52656379636c6520426969")
            if l=="CMDS0008a2140000":   # ListFirstID - Extended
                conn.send("8a14000c000a6175726f72612d64313030")
                print("Sent 8a14000c000a6175726f72612d64313030\n")
            if l=="CMDS0006a11505":      # ListNextID - Extended
                conn.send("8a14003500066431305f6b32000b4b3220ebb094ebb3b45f3100084b32ebb094ebb3b4000ce88d89e8b0b7e585ace58fb80006eca08420434dba")
                print("Sent 8a14003500066431305f6b32000b4b3220ebb094ebb3b45f3100084b32ebb094ebb3b4000ce88d89e8b0b7e585ace58fb80006eca08420434dba\n")
            if l=="CMDS0004a00f":       #GetWorkingFolder
                conn.send("820f0009000764656661756c7486")
                print("Sent  820f0009000764656661756c7486\n") #default
            if l=="CMDS0004a20e0009000764656661756c74":  #SetWorkingFolder to default
                conn.send("100111")
                print("Sent 100111\n") 
            l=""
        else:
            l=l+ch
except KeyboardInterrupt:
    conn.close()
