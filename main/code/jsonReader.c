#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include "cJSON.h"

int getDelay(char *text) {
	cJSON *json;
	int delay = -1;

	json = cJSON_Parse(text);
	if (!json) {
		syslog(LOG_ERR, "JSON Parsing error : [%s]\n", cJSON_GetErrorPtr());
	} else {
		delay = cJSON_GetObjectItem(json, "delay")->valueint;
		cJSON_Delete(json);
	}

	return delay;
}
