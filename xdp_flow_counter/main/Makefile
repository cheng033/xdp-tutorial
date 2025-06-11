CLANG      ?= clang

all: xdp_prog_kern.o xdp_load_and_stats flow_collector

xdp_prog_kern.o: xdp_prog_kern.c common_kern_user.h
	$(CLANG) -O2 -g -Wall -target bpf -c xdp_prog_kern.c -o xdp_prog_kern.o

xdp_load_and_stats: xdp_load_and_stats.c common_kern_user.h
	gcc -g -o xdp_load_and_stats xdp_load_and_stats.c -lbpf -lelf -lxdp

flow_collector: flow_collector.c common_kern_user.h
	gcc -g -o flow_collector flow_collector.c -lbpf

clean:
	rm -f *.o xdp_load_and_stats flow_collector

