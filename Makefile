obj=main.o uart-eth.o
CC=arm-xilinx-linux-gnueabi-gcc
uart-eth:$(obj)
	$(CC) -o uart-eth $(obj) -Wall
	cp uart-eth /opt/nfsboot 
	cp uart-eth /home/zjc
clean:
	rm uart-eth $(obj)
