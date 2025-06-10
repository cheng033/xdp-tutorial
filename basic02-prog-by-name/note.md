# 筆記
## Assignment 1
建立好測試環境，和主機網路分開<br>
使用 ```xdp_loader``` 操作與觀察

## Assignment 2
加入新function ```xdp_abort``` <br>
原terminal用ping, 另一個terminal用perf觀察exception
```
           ping6   17227 [007] 19328.613436: xdp:xdp_excep>
           ping6   17227 [007] 19329.637240: xdp:xdp_excep>
           ping6   17227 [007] 19330.661305: xdp:xdp_excep>
           ping6   17227 [007] 19331.685326: xdp:xdp_excep>
```


