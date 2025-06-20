#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <bpf/bpf.h>
#include <bpf/libbpf.h>
#include "common_kern_user.h" // flow_key, flow_val struct

int main(int argc, char **argv) {
    // 開啟輸出檔案 flows.csv，模式為寫入("w")
    FILE *fp = fopen("flows.csv", "w");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    // 1. 開啟 BPF map
    int map_fd = bpf_obj_get("/sys/fs/bpf/xdp/flows");  // pin map 的位置
    if (map_fd < 0) {
        perror("bpf_obj_get");
        fclose(fp);
        return 1;
    }

    struct flow_key key, next_key;
    struct flow_val val;
    char src_ip[INET_ADDRSTRLEN], dst_ip[INET_ADDRSTRLEN];
    int count = 0;   // 控制最多印幾筆
    int max_count = 5; // 只印 5 筆

    // 印出欄位標題到 flows.csv
    fprintf(fp, "src_ip,src_port,dst_ip,dst_port,proto,pkts,bytes\n");

    // 先找 map 的第一個 key
    int ret = bpf_map_get_next_key(map_fd, NULL, &key);
    while (ret == 0 && count < max_count) {
        if (bpf_map_lookup_elem(map_fd, &key, &val) == 0) {
            inet_ntop(AF_INET, &key.src_ip, src_ip, sizeof(src_ip));
            inet_ntop(AF_INET, &key.dst_ip, dst_ip, sizeof(dst_ip));
            // 寫到 flows.csv
            fprintf(fp, "%s,%u,%s,%u,%u,%llu,%llu\n",
                src_ip, ntohs(key.src_port),
                dst_ip, ntohs(key.dst_port),
                key.proto, val.packets, val.bytes);
            count++;
        }
        // 刪掉剛剛那個 key
        bpf_map_delete_elem(map_fd, &key);

        // 取得下一個 key
        ret = bpf_map_get_next_key(map_fd, &key, &next_key);
        key = next_key;
    }

    fclose(fp); // 關閉檔案
    return 0;
}

