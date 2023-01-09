#!/bin/sh

RTE_SDK=/opt/dpdk-19.11

gcc -o wls_sample_phy wls_sample_phy.c intel/wls_lib_phy.c gSM_l1_l2_api.c -Wall -O3 -I./intel -DNR5G -I${RTE_SDK}/x86_64-native-linuxapp-gcc/include -lpthread -lwls -L${RTE_SDK}/x86_64-native-linuxapp-gcc/lib -L${RTE_SDK}/build/lib -L${RTE_SDK}/build/drivers -Wl,--whole-archive -Wl,-lrte_mempool_ring -Wl,-lrte_pci -Wl,-lrte_bus_pci -Wl,-lrte_bus_vdev -Wl,-lrte_net -Wl,-lrte_distributor -Wl,-lrte_reorder -Wl,-lrte_kni -Wl,-lrte_pipeline -Wl,-lrte_table -Wl,-lrte_timer -Wl,-lrte_hash -Wl,-lrte_jobstats -Wl,-lrte_lpm -Wl,-lrte_power -Wl,-lrte_acl -Wl,-lrte_meter -Wl,-lrte_sched -Wl,-lm -Wl,-lrt -Wl,-lrte_vhost -Wl,--start-group -Wl,-lrte_kvargs -Wl,-lrte_mbuf -Wl,-lrte_ip_frag -Wl,-lrte_ethdev -Wl,-lrte_cryptodev -Wl,-lrte_mempool -Wl,-lrte_ring -Wl,-lrte_eal -Wl,-lrte_cmdline -Wl,-lrte_cfgfile -Wl,-lrt -Wl,-lm -Wl,-ldl -Wl,--end-group -Wl,--no-whole-archive -Wl,-lnuma -lhugetlbfs
