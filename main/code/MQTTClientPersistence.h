#if !defined(MQTTCLIENTPERSISTENCE_H)
#define MQTTCLIENTPERSISTENCE_H

#define MQTTCLIENT_PERSISTENCE_DEFAULT 0

#define MQTTCLIENT_PERSISTENCE_NONE 1

#define MQTTCLIENT_PERSISTENCE_USER 2

#define MQTTCLIENT_PERSISTENCE_ERROR -2

typedef int (*Persistence_open)(void **handle, char *clientID, char *serverURI, void *context);

typedef int (*Persistence_close)(void *handle);

typedef int (*Persistence_put)(void *handle, char *key, int bufcount, char *buffers[], int buflens[]);

typedef int (*Persistence_get)(void *handle, char *key, char **buffer, int *buflen);

typedef int (*Persistence_remove)(void *handle, char *key);


typedef int (*Persistence_keys)(void *handle, char ***keys, int *nkeys);

typedef int (*Persistence_clear)(void *handle);

typedef int (*Persistence_containskey)(void *handle, char *key);

typedef struct {

    void *context;

    Persistence_open popen;

    Persistence_close pclose;

    Persistence_put pput;

    Persistence_get pget;

    Persistence_remove premove;

    Persistence_keys pkeys;

    Persistence_clear pclear;

    Persistence_containskey pcontainskey;
} MQTTClient_persistence;

#endif
