# XDP Flow Counter

根據 xdp-tutorial/basice03 原始碼修改成輕量級的 XDP 五元組流量統計。<br>

## 編譯 kernel object 和 user-space 可執行檔
```
make
```
## XDP 程式 attach 到網卡並列印flow 統計
```
sudo ./xdp_load_and_stats -d <ifname>
```
### ifname 請用 ip link 查詢，例如 eth0, wlo1
```
sudo ./xdp_load_and_stats -d wlo1
```
## 範例輸出
```
src_ip:src_port  ->  dst_ip:dst_port    proto     pkts    bytes
192.168.68.1:57680 -> 224.0.0.251:5353  proto:17  pkts:1  bytes:86
```

