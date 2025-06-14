/* This common_kern_user.h is used by kernel side BPF-progs and
 * userspace programs, for sharing common struct's and DEFINEs.
 */
#ifndef __COMMON_KERN_USER_H
#define __COMMON_KERN_USER_H
#include <linux/types.h> 

/* This is the data record stored in the map 
struct datarec {
	__u64 rx_packets;
    __u64 rx_bytes;
};
*/

struct flow_key {
    __u32 src_ip;
    __u32 dst_ip;
    __u16 src_port;
    __u16 dst_port;
    __u8 proto;
}__attribute__((packed));

struct flow_val{
    __u64 packets;
    __u64 bytes;
}__attribute__((packed));

//#ifndef XDP_ACTION_MAX
//#define XDP_ACTION_MAX (XDP_REDIRECT + 1)
//#endif

#endif /* __COMMON_KERN_USER_H */
