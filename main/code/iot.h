#ifndef IOT_H_
#define IOT_H_


struct json {
    char myname[100];
    float cputemp;
    float sine;
    float cpuload;
};

typedef struct json JsonMessage;


#define MAXBUF 100


#define MSPROXY_URL "tcp://46.16.189.243:1883"
#define MSPROXY_URL_SSL "ssl://46.16.189.242:8883"
#define EVENTS_INTERVAL 1
#define DEVICE_NAME "myPi"

#define LOGLEVEL 6

extern int connected;

#endif
