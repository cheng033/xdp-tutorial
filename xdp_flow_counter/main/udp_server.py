import socket as s

u = s.socket(s.AF_INET, s.SOCK_DGRAM)
u.bind(("", 8080))
print("UDP 8080 started")
t = 0
while True:
    d, a = u.recvfrom(2048)
    t += 1
    print("pkts", t, "from", a)

