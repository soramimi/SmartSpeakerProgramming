
CFLAGS = -I ../ -I ../packet
CXXFLAGS = -I ../ -I ../packet

OBJS = \
	../packet/MQTTConnectClient.o \
	../packet/MQTTConnectServer.o \
	../packet/MQTTDeserializePublish.o \
	../packet/MQTTFormat.o \
	../packet/MQTTPacket.o \
	../packet/MQTTSerializePublish.o \
	../packet/MQTTSubscribeClient.o \
	../packet/MQTTSubscribeServer.o \
	../packet/MQTTUnsubscribeClient.o \
	../packet/MQTTUnsubscribeServer.o \
	../transport.o \
	main.o

all: okaeri

okaeri: $(OBJS)
	cd ../packet ; make
	g++ $^ -o $@ -lpthread

clean:
	rm -f *.o
	rm -f okaeri
