#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <signal.h>
#include "iot.h"
#include "MQTTAsync.h"
#include <syslog.h>

char configFile[50] = "/etc/iotsample-raspberrypi/device.cfg";
float PI = 3.1415926;
float MIN_VALUE = -1.0;
float MAX_VALUE = 1.0;

char clientId[MAXBUF];
char publishTopic[MAXBUF] = "iot-2/evt/status/fmt/json";
char subscribeTopic[MAXBUF] = "iot-2/cmd/reboot/fmt/json";

int isRegistered = 0;

MQTTAsync client;

struct config {
	char org[MAXBUF];
	char type[MAXBUF];
	char id[MAXBUF];
	char authmethod[MAXBUF];
	char authtoken[MAXBUF];
};

int get_config(char* filename, struct config * configstr);
void getClientId(struct config * configstr, char* mac_address);
float sineVal(float minValue, float maxValue, float duration, float count);
void sig_handler(int signo);
int reconnect_delay(int i);

float getCPUTemp();
float GetCPULoad();

char *getmac(char *iface);

char * generateJSON(JsonMessage passedrpi);


int init_mqtt_connection(MQTTAsync* client, char *address, int isRegistered,
		char* client_id, char* username, char* passwd);
int publishMQTTMessage(MQTTAsync* client, char *topic, char *payload);
int subscribe(MQTTAsync* client, char *topic);
int disconnect_mqtt_client(MQTTAsync* client);
int reconnect(MQTTAsync* client, int isRegistered,
		char* username, char* passwd);

int main(int argc __attribute__((unused)),
	 char **argv __attribute__((unused))) {

	char* json;


	int res;
	int sleepTimeout;
	struct config configstr;

	char *passwd;
	char *username;
	char msproxyUrl[MAXBUF];

	setlogmask(LOG_UPTO(LOGLEVEL));
	openlog("iot", LOG_PID | LOG_CONS, LOG_USER);
	syslog(LOG_INFO, "**** IoT Raspberry Pi Sample has started ****");

	if (signal(SIGUSR1, sig_handler) == SIG_ERR)
		syslog(LOG_CRIT, "Not able to register the signal handler\n");
	if (signal(SIGINT, sig_handler) == SIG_ERR)
		syslog(LOG_CRIT, "Not able to register the signal handler\n");

	isRegistered = get_config(configFile, &configstr);

	if (isRegistered) {
		syslog(LOG_INFO, "Running in Registered mode\n");
		sprintf(msproxyUrl, "ssl://%s.messaging.internetofthings.ibmcloud.com:8883", configstr.org);
		if(strcmp(configstr.authmethod ,"token") != 0) {
			syslog(LOG_ERR, "Detected that auth-method is not token. Currently other authentication mechanisms are not supported, IoT process will exit.");
			syslog(LOG_INFO, "**** IoT Raspberry Pi Sample has ended ****");
				closelog();
				exit(1);
		} else {
			username = "use-token-auth";
			passwd = configstr.authtoken;
		}
	} else {
		syslog(LOG_INFO, "Running in Quickstart mode\n");
		strcpy(msproxyUrl,"tcp://quickstart.messaging.internetofthings.ibmcloud.com:1883");
	}

	char* mac_address = getmac("eth0");
	getClientId(&configstr, mac_address);
	int connDelayTimeout = 1;
	int retryAttempt = 0;

	init_mqtt_connection(&client, msproxyUrl, isRegistered, clientId, username, passwd);
	while (!MQTTAsync_isConnected(client)) {
		connDelayTimeout = 1;
		if (connected == -1) {
			connDelayTimeout = reconnect_delay(++retryAttempt);
			syslog(LOG_ERR,
					"Failed connection attempt #%d. Will try to reconnect "
							"in %d seconds\n", retryAttempt, connDelayTimeout);
			connected = 0;
			init_mqtt_connection(&client, msproxyUrl, isRegistered, clientId, username,
					passwd);
		}
		fflush(stdout);
		sleep(connDelayTimeout);
	}

	connDelayTimeout = 1;
	retryAttempt = 0;

	int count = 1;
	sleepTimeout = EVENTS_INTERVAL;

	if (isRegistered) {
		subscribe(&client, subscribeTopic);
	}
	while (1) {
		JsonMessage json_message = { DEVICE_NAME, getCPUTemp(), sineVal(
				MIN_VALUE, MAX_VALUE, 16, count), GetCPULoad() };
		json = generateJSON(json_message);
		res = publishMQTTMessage(&client, publishTopic, json);
		syslog(LOG_DEBUG, "Posted the message with result code = %d\n", res);
		if (res == -3) {
			connected = -1;
			while (!MQTTAsync_isConnected(client)) {
				if (connected == -1) {
					connDelayTimeout = reconnect_delay(++retryAttempt);
					syslog(LOG_ERR, "Failed connection attempt #%d. "
							"Will try to reconnect in %d "
							"seconds\n", retryAttempt, connDelayTimeout);
					sleep(connDelayTimeout);
					connected = 0;
					reconnect(&client, isRegistered, username,passwd);
				}
				fflush(stdout);
				sleep(1);
			}
			connDelayTimeout = 1;
			retryAttempt = 0;
		}
		fflush(stdout);
		free(json);
		count++;
		sleep(sleepTimeout);
	}

	return 0;
}

void getClientId(struct config * configstr, char* mac_address) {

	char *orgId;
	char *typeId;
	char *deviceId;

	if (isRegistered) {

		orgId = configstr->org;
		typeId = configstr->type;
		deviceId = configstr->id;

	} else {

		orgId = "quickstart";
		typeId = "iotsample-raspberrypi";
		deviceId = mac_address;

	}
	sprintf(clientId, "d:%s:%s:%s", orgId, typeId, deviceId);
}

float sineVal(float minValue, float maxValue, float duration, float count) {
	float sineValue;
	sineValue = sin(2.0 * PI * count / duration) * (maxValue - minValue) / 2.0;
	return sineValue;
}

void sig_handler(int signo __attribute__((unused))) {
	syslog(LOG_INFO, "Received the signal to terminate the IoT process. \n");
	syslog(LOG_INFO,
			"Trying to end the process gracefully. Closing the MQTT connection. \n");
	int res = disconnect_mqtt_client(&client);

	syslog(LOG_INFO, "Disconnect finished with result code : %d\n", res);
	syslog(LOG_INFO, "Shutdown of the IoT process is complete. \n");
	syslog(LOG_INFO, "**** IoT Raspberry Pi Sample has ended ****");
	closelog();
	exit(1);
}

int reconnect_delay(int i) {
	if (i < 10) {
		return 3;
	}
	if (i < 20)
		return 60;

	return 600;
}

char *trim(char *str) {
	size_t len = 0;
	char *frontp = str - 1;
	char *endp = NULL;

	if (str == NULL)
		return NULL;

	if (str[0] == '\0')
		return str;

	len = strlen(str);
	endp = str + len;

	while (isspace(*(++frontp)))
		;
	while (isspace(*(--endp)) && endp != frontp)
		;

	if (str + len - 1 != endp)
		*(endp + 1) = '\0';
	else if (frontp != str && endp == frontp)
		*str = '\0';

	endp = str;
	if (frontp != str) {
		while (*frontp)
			*endp++ = *frontp++;
		*endp = '\0';
	}

	return str;
}



int get_config(char * filename, struct config * configstr) {

	FILE* prop;
	prop = fopen(filename, "r");
	if (prop == NULL) {
		syslog(LOG_INFO,"Config file not found. Going to Quickstart mode\n");
		return 0;
	}
	char line[256];
	int linenum = 0;
	while (fgets(line, 256, prop) != NULL) {
		char* prop;
		char* value;

		linenum++;
		if (line[0] == '#')
			continue;

		prop = strtok(line, "=");
		prop = trim(prop);
		value = strtok(NULL, "=");
		value = trim(value);
		if (strcmp(prop, "org") == 0)
			strncpy(configstr->org, value, MAXBUF);
		else if (strcmp(prop, "type") == 0)
			strncpy(configstr->type, value, MAXBUF);
		else if (strcmp(prop, "id") == 0)
			strncpy(configstr->id, value, MAXBUF);
		else if (strcmp(prop, "auth-token") == 0)
			strncpy(configstr->authtoken, value, MAXBUF);
		else if (strcmp(prop, "auth-method") == 0)
					strncpy(configstr->authmethod, value, MAXBUF);
	}

	return 1;
}
