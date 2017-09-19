KERN_DIR=/home/my/Projects/ltib/rpm/BUILD/linux-3.0.35
# ./home/my/Projects/ltib/rpm/BUILD/linux-3.0.35

all:
	make -C $(KERN_DIR) M=`pwd` modules
clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m += my_buzzer.o
