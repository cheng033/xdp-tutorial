/* SPDX-License-Identifier: GPL-2.0 */
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include "common_kern_user.h"
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/in.h>

// flow counter map
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __type(key, struct flow_key);
    __type(value, struct flow_val);
    __uint(max_entries, 65536);
} flows SEC(".maps");

SEC("xdp")
int xdp_flow_func(struct xdp_md *ctx)
{
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long) ctx->data;

    struct ethhdr *eth = data;
    if ((void *)(eth + 1) > data_end)
        return XDP_PASS;
    
    // ipv4
    if (eth->h_proto == __constant_htons(ETH_P_IP)) {
        struct iphdr *iph = data + sizeof(*eth);
        if ((void *)(iph + 1) > data_end)
            return XDP_PASS;
        
        struct flow_key key = {};
        key.src_ip = iph->saddr;
        key.dst_ip = iph->daddr;
        key.proto = iph->protocol;

        // tcp, udp
        if (iph->protocol == IPPROTO_TCP || iph->protocol == IPPROTO_UDP) {
            void *l4 = (void *) iph + iph->ihl * 4;
            if (iph->protocol == IPPROTO_TCP) {
                struct tcphdr *th = l4;
                if ((void *)(th + 1) > data_end)
                    return XDP_PASS;
                key.src_port = th->source;
                key.dst_port = th->dest;
            } else if (iph->protocol == IPPROTO_UDP) {
                struct udphdr *uh = l4;
                if ((void *)(uh + 1) > data_end)
                    return XDP_PASS;
                key.src_port = uh->source;
                key.dst_port = uh->dest;
            }
        }
        struct flow_val *val = bpf_map_lookup_elem(&flows, &key);
        if (!val) {
            struct flow_val zero = {};
            bpf_map_update_elem(&flows, &key, &zero, BPF_NOEXIST);
            val = bpf_map_lookup_elem(&flows, &key);
        }
        if (val) {
            val->packets += 1;
            val->bytes += data_end - data;
        }
    }
    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";

