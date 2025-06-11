#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <bpf/bpf.h>
#include <bpf/libbpf.h>
#include "common_kern_user.h" // 你的 flow_key, flow_val struct

int main(int argc, char **argv) {
    // 1. 開啟 BPF map
    int map_fd = bpf_obj_get("/sys/fs/bpf/xdp/flows");  // 改成你 loader pin map 的位置
    if (map_fd < 0) {
        perror("bpf_obj_get");
        return 1;
    }

    struct flow_key key, next_key;
    struct flow_val val;
    char src_ip[INET_ADDRSTRLEN], dst_ip[INET_ADDRSTRLEN];

    printf("src_ip:src_port -> dst_ip:dst_port proto pkts bytes\n");

    // 2. 走訪 map
    memset(&key, 0, sizeof(key));
    while (bpf_map_get_next_key(map_fd, &key, &next_key) == 0) {
        if (bpf_map_lookup_elem(map_fd, &next_key, &val) == 0) {
            inet_ntop(AF_INET, &next_key.src_ip, src_ip, sizeof(src_ip));
            inet_ntop(AF_INET, &next_key.dst_ip, dst_ip, sizeof(dst_ip));
            printf("%s:%u -> %s:%u proto:%u pkts:%llu bytes:%llu\n",
                src_ip, ntohs(next_key.src_port),
                dst_ip, ntohs(next_key.dst_port),
                next_key.proto, val.packets, val.bytes);
        }
        key = next_key;
    }
    return 0;
}

