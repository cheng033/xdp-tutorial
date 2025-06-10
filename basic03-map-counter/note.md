# lab 3

## Assignment 1：Add bytes counter
\_\_u32 data 只是方便定義struct<br> 實際是用ptr運算 所以要轉型態
```void *data = (void *)(long)ctx->data;``` <br>atomic 加法 ```lock_xadd```, 避免race condition


## Assignment 2：Handle other XDP actions stats
XDP action 加入 不只 PASS，還有 DROP、TX、REDIRECT、ABORTED，方便精細分析流量走向。<br>確認 common_kern_user.h 裡有 ```#define XDP_ACTION_MAX (XDP_REDIRECT + 1)```<br>stats_collect 要用 loop 一次查每個 action 的 key，stats_print 也要 loop 多印出各 action。<br>stat struct 都需改成 stats[XDP_ACTION_MAX]，而非 stats[1]，避免超出範圍。

## Assignment 3：Per CPU stats

把 map 型態換成 BPF_MAP_TYPE_PERCPU_ARRAY，每核獨立累加統計，用來減少 atomic operation、提升效能。<br>（每核自己累加，不會互搶 memory）




