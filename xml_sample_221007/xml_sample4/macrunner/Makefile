## RTE path setting

RTE_SDK=/opt/dpdk-19.11

## RTE params

RTE_INC = -I${RTE_SDK}/x86_64-native-linuxapp-gcc/include -I${RTE_SDK}/build/include
RTE_LIBS = -lpthread -lwls -L${RTE_SDK}/x86_64-native-linuxapp-gcc/lib -L${RTE_SDK}/build/lib -L${RTE_SDK}/build/drivers -Wl,--whole-archive -Wl,-lrte_mempool_ring -Wl,-lrte_pci -Wl,-lrte_bus_pci -Wl,-lrte_bus_vdev -Wl,-lrte_net -Wl,-lrte_distributor -Wl,-lrte_reorder -Wl,-lrte_kni -Wl,-lrte_pipeline -Wl,-lrte_table -Wl,-lrte_timer -Wl,-lrte_hash -Wl,-lrte_jobstats -Wl,-lrte_lpm -Wl,-lrte_power -Wl,-lrte_acl -Wl,-lrte_meter -Wl,-lrte_sched -Wl,-lm -Wl,-lrt -Wl,-lrte_vhost -Wl,--start-group -Wl,-lrte_kvargs -Wl,-lrte_mbuf -Wl,-lrte_ip_frag -Wl,-lrte_ethdev -Wl,-lrte_cryptodev -Wl,-lrte_mempool -Wl,-lrte_ring -Wl,-lrte_eal -Wl,-lrte_cmdline -Wl,-lrte_cfgfile -Wl,-lrt -Wl,-lm -Wl,-ldl -Wl,--end-group -Wl,--no-whole-archive -Wl,-lnuma -lhugetlbfs

## base settings

CC = gcc
AR = ar
CFLAGS = -Wall -O3

SRC_DIR = ./src/
INC_DIR = ./include/ 
OUT_DIR = ./lib/
BIN_DIR = ./bin/

TARGET = $(BIN_DIR)macrunner
INC = -I$(INC_DIR) -I./intel -I/usr/include/libxml2 $(RTE_INC)

LFLAGS = -lxml2 $(RTE_LIBS) -L$(OUT_DIR) 
SRCS = $(wildcard $(SRC_DIR)*.c)
OBJS = $(patsubst %.c,$(OUT_DIR)%.o,$(notdir $(SRCS)))

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $(TARGET) ./intel/wls_lib_mac.c $(LFLAGS) $(CFLAGS) $(OBJS)

$(OUT_DIR)%.o : $(SRC_DIR)%.c
	$(CC) -c $< -o $@ $(INC) $(CFLAGS)

clean :
	rm -f $(TARGET)
	rm -f $(OBJS)

