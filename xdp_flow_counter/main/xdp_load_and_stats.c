/* SPDX-License-Identifier: GPL-2.0 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <bpf/bpf.h>
#include <bpf/libbpf.h>
#include <xdp/libxdp.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "common_kern_user.h"


int find_map_fd(struct bpf_object *bpf_obj, const char *mapname)
{
    struct bpf_map *map;
    int map_fd = -1;

    map = bpf_object__find_map_by_name(bpf_obj, mapname);
    if (!map) {
        fprintf(stderr, "ERR: cannot find map by name: %s\n", mapname);
        goto out;
    }
    
	map_fd = bpf_map__fd(map);
out:
    return map_fd;
}

// flow counter
void print_flows_map(int map_fd){
    struct flow_key key = {0}, next_key;
    struct flow_val val;
    char src_ip[16], dst_ip[16];
	//int count = 0;

    printf("src_ip:src_port  -> dst_ip:dst_port  proto  pkts   bytes\n");
	
	int ret = bpf_map_get_next_key(map_fd, NULL, &key);
    
	while(ret == 0){
        if (bpf_map_lookup_elem(map_fd, &key, &val) == 0){
            inet_ntop(AF_INET, &key.src_ip, src_ip, sizeof(src_ip));
            inet_ntop(AF_INET, &key.dst_ip, dst_ip, sizeof(dst_ip));
            printf("%s:%u -> %s:%u  proto:%u  pkts:%llu  bytes:%llu\n",
                     src_ip, ntohs(key.src_port),
                     dst_ip, ntohs(key.dst_port),
                     key.proto,
                     val.packets,
                     val.bytes);
        }
        ret = bpf_map_get_next_key(map_fd, &key, &next_key);
        key = next_key;
    }
}


int main(int argc, char **argv)
{
    struct xdp_program *program;
    int flows_map_fd;
    char ifname[IF_NAMESIZE] = "";
    int ifindex = -1;
    int opt;

    //
    while ((opt = getopt(argc, argv, "d:")) != -1) {
        switch (opt) {
            case 'd':
                strncpy(ifname, optarg, IF_NAMESIZE - 1);
                ifindex = if_nametoindex(ifname);
                break;
            default:
                fprintf(stderr, "Usage: %s -d <ifname>\n", argv[0]);
                return 1;
        }
    }
    if (ifindex == -1) {
        fprintf(stderr, "Usage: %s -d <ifname>\n", argv[0]);
        return 1;
    }
	//clean map
	unlink("/sys/fs/bpf/xdp/flows"); 
    //
    program = xdp_program__open_file("xdp_prog_kern.o", "xdp", NULL);
    if (!program) {
        fprintf(stderr, "Failed to load BPF object\n");
        return 1;
    }
    if (xdp_program__attach(program, ifindex, XDP_MODE_SKB, 0)) {
        fprintf(stderr, "Failed to attach XDP program to %s\n", ifname);
        xdp_program__close(program);
        return 1;
    }

    //
    flows_map_fd = find_map_fd(xdp_program__bpf_obj(program), "flows");
    if (flows_map_fd < 0) {
        fprintf(stderr, "ERR: cannot find flows map\n");
        xdp_program__detach(program, ifindex, XDP_MODE_SKB, 0);
        xdp_program__close(program);
        return 1;
    }

	//pin
	struct bpf_map *flows_map = bpf_object__find_map_by_name(xdp_program__bpf_obj(program), "flows");
	if (!flows_map) {
    	fprintf(stderr, "找不到 flows map\n");
    	return 1;
	}

	int ret = bpf_map__pin(flows_map, "/sys/fs/bpf/xdp/flows");
	if (ret && ret != -EEXIST) {          
    	fprintf(stderr, "pin map failed: %s\n", strerror(-ret));
    	return 1;
	}
	
	
	printf("等待 5 秒，讓 flows map 有流量進來...\n");
	sleep(5);

    print_flows_map(flows_map_fd);

    return 0;
}

