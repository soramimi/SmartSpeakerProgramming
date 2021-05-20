TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++1z

INCLUDEPATH += $$PWD/packet

DESTDIR = $$PWD/_bin

LIBS += -lpthread

SOURCES += \
	okaeri/main.cpp \
        packet/MQTTConnectClient.c \
        packet/MQTTConnectServer.c \
        packet/MQTTDeserializePublish.c \
        packet/MQTTFormat.c \
        packet/MQTTPacket.c \
        packet/MQTTSerializePublish.c \
        packet/MQTTSubscribeClient.c \
        packet/MQTTSubscribeServer.c \
        packet/MQTTUnsubscribeClient.c \
        packet/MQTTUnsubscribeServer.c \
	transport.c

HEADERS += \
	packet/MQTTConnect.h \
	packet/MQTTFormat.h \
	packet/MQTTPacket.h \
	packet/MQTTPublish.h \
	packet/MQTTSubscribe.h \
	packet/MQTTUnsubscribe.h \
	packet/StackTrace.h \
	transport.h
