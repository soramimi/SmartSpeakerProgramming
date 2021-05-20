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

//#define SERVER "mqtt.beebotte.com"
//#define USERNAME "token:token_ABCDEF0123456789"
//#define PASSWORD ""
#include "../server.h"

#define CHANNEL_RESOURCE "Tadaima/voice"

int mysock = 0;

int toStop = 0;

void cfinish(int sig)
{
	transport_close(mysock);
	toStop = 1;
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

	char *host = "mqtt.beebotte.com";
	int port = 1883;

	stop_init();

	mysock = transport_open(host, port);
	if (mysock < 0) return mysock;

	printf("Sending to hostname %s port %d\n", host, port);

	data.clientID.cstring = "";
	data.keepAliveInterval = 20;
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

	/* subscribe */
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

	{
		char *payload = "{\"data\":\"Tadaima\"}";
		int payloadlen = strlen(payload);
		len = MQTTSerialize_publish(buf, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
		rc = transport_sendPacketBuffer(mysock, buf, len);
	}

exit:
	transport_close(mysock);

	return 0;
}
