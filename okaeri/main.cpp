/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - clarifications and/or documentation extension
 *******************************************************************************/

// Modified by S.Fuchita (@soramimi_jp) 2021-05-20

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "MQTTPacket.h"
#include "transport.h"

/* This is in order to get an asynchronous signal to stop the sample,
as the code loops waiting for msgs on the subscribed topic.
Your actual code will depend on your hw and approach*/
#include <signal.h>

#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "../common/jstream.h"

//#define SERVER "mqtt.beebotte.com"
//#define USERNAME "token:token_ABCDEF0123456789"
//#define PASSWORD ""
#include "../server.h"

#define CHANNEL_RESOURCE "Tadaima/voice"

int mysock = 0;

int toStop = 0;

std::thread th;
std::mutex mutex;
std::condition_variable cv;

void cfinish(int sig)
{
	transport_close(mysock);
	toStop = 1;

	cv.notify_all();
	if (th.joinable()) {
		th.join();
	}
}

void stop_init(void)
{
	signal(SIGINT, cfinish);
	signal(SIGTERM, cfinish);
}

int main()
{
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	int rc = 0;
	unsigned char buf[200];
	int buflen = sizeof(buf);
	int msgid = 1;
	MQTTString topicString = MQTTString_initializer;
	int req_qos = 0;
	int len = 0;

	char *host = SERVER;
	int port = 1883;

	stop_init();

	mysock = transport_open(host, port);
	if(mysock < 0) return mysock;

	printf("Sending to hostname %s port %d\n", host, port);

	data.clientID.cstring = "";
	data.keepAliveInterval = 70;
	data.cleansession = 1;
	data.username.cstring = USERNAME;
	data.password.cstring = PASSWORD;

	len = MQTTSerialize_connect(buf, buflen, &data);
	rc = transport_sendPacketBuffer(mysock, buf, len);

	if (MQTTPacket_read(buf, buflen, transport_getdata) == CONNACK) { // wait for connack
		unsigned char sessionPresent, connack_rc;
		if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) != 1 || connack_rc != 0) {
			printf("Unable to connect, return code %d\n", connack_rc);
			goto exit;
		}
	} else {
		goto exit;
	}

	// subscribe
	topicString.cstring = CHANNEL_RESOURCE;
	len = MQTTSerialize_subscribe(buf, buflen, 0, msgid, 1, &topicString, &req_qos);

	rc = transport_sendPacketBuffer(mysock, buf, len);
	if (MQTTPacket_read(buf, buflen, transport_getdata) == SUBACK) { // wait for suback
		unsigned short submsgid;
		int subcount;
		int granted_qos;
		rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, buf, buflen);
		if (granted_qos != 0) {
			printf("granted qos != 0, %d\n", granted_qos);
			goto exit;
		}
	} else {
		goto exit;
	}


	th = std::thread([](){
		while (1) {
			{
				std::unique_lock<std::mutex> lock(mutex);
				cv.wait_for(lock, std::chrono::seconds(60));
				if (toStop) {
					break;
				}
			}
			unsigned char tmp[2];
			int len = MQTTSerialize_pingreq(tmp, 2);
			transport_sendPacketBuffer(mysock, tmp, len);
		}
	});

	// loop getting msgs on subscribed topic
	while (!toStop) {
		// transport_getdata() has a built-in 1 second timeout, your mileage will vary
		if (MQTTPacket_read(buf, buflen, transport_getdata) == PUBLISH) {
			unsigned char dup;
			int qos;
			unsigned char retained;
			unsigned short msgid;
			MQTTString receivedTopic;
			unsigned char *payload_in;
			int payloadlen_in;

			int rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic, &payload_in, &payloadlen_in, buf, buflen);
			printf("message arrived %.*s\n", payloadlen_in, payload_in);

			std::string data;

			jstream::Reader r((char const *)payload_in, (char const *)payload_in + payloadlen_in);
			while (r.next()) {
				if (r.match("{data")) {
					data = r.string();
				}
			}

			if (data == "Tadaima") {
				system("mpg123 -q okaeri.mp3");
			}
		} else {
//			putc('!', stderr);
		}
	}

	printf("disconnecting\n");
	len = MQTTSerialize_disconnect(buf, buflen);
	rc = transport_sendPacketBuffer(mysock, buf, len);

exit:
	transport_close(mysock);

	return 0;
}
