import socket, time, os

u=socket.socket(socket.AF_INET,socket.SOCK_DGRAM); u.bind(('',8080))
cnt,t=0,time.time()
print("UDP 8080 started, pid",os.getpid())

while True:
    u.recvfrom(2048); cnt+=1
    if time.time()-t>1:
        print("pkts/s",cnt); cnt=0; t=time.time()
