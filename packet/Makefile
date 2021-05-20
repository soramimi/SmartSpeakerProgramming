
OBJS = \
MQTTConnectClient.o \
MQTTConnectServer.o \
MQTTDeserializePublish.o \
MQTTFormat.o \
MQTTPacket.o \
MQTTSerializePublish.o \
MQTTSubscribeClient.o \
MQTTSubscribeServer.o \
MQTTUnsubscribeClient.o \
MQTTUnsubscribeServer.o

all: $(OBJS)

MQTTConnectClient.o: MQTTConnectClient.c
MQTTConnectServer.o: MQTTConnectServer.c
MQTTDeserializePublish.o: MQTTDeserializePublish.c
MQTTFormat.o: MQTTFormat.c
MQTTPacket.o: MQTTPacket.c
MQTTSerializePublish.o: MQTTSerializePublish.c
MQTTSubscribeClient.o: MQTTSubscribeClient.c
MQTTSubscribeServer.o: MQTTSubscribeServer.c
MQTTUnsubscribeClient.o: MQTTUnsubscribeClient.c
MQTTUnsubscribeServer.o: MQTTUnsubscribeServer.c

clean:
	rm -f *.o

