#if !defined(MQTTASYNC_H)
#define MQTTASYNC_H

#if defined(WIN32)
#define DLLImport __declspec(dllimport)
#define DLLExport __declspec(dllexport)
#else
#define DLLImport extern
#define DLLExport  __attribute__ ((visibility ("default")))
#endif

#include <stdio.h>

#if !defined(NO_PERSISTENCE)

#include "MQTTClientPersistence.h"

#endif


#define MQTTASYNC_SUCCESS 0

#define MQTTASYNC_FAILURE -1


#define MQTTASYNC_PERSISTENCE_ERROR -2

#define MQTTASYNC_DISCONNECTED -3

#define MQTTASYNC_MAX_MESSAGES_INFLIGHT -4

#define MQTTASYNC_BAD_UTF8_STRING -5

#define MQTTASYNC_NULL_PARAMETER -6

#define MQTTASYNC_TOPICNAME_TRUNCATED -7

#define MQTTASYNC_BAD_STRUCTURE -8

#define MQTTASYNC_BAD_QOS -9

#define MQTTASYNC_NO_MORE_MSGIDS -10


typedef void *MQTTAsync;

typedef int MQTTAsync_token;


typedef struct {

    char struct_id[4];

    int struct_version;

    int payloadlen;

    void *payload;

    int qos;

    int retained;

    int dup;

    int msgid;
} MQTTAsync_message;

#define MQTTAsync_message_initializer { {'M', 'Q', 'T', 'M'}, 0, 0, NULL, 0, 0, 0, 0 }

typedef int MQTTAsync_messageArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *message);


typedef void MQTTAsync_deliveryComplete(void *context, MQTTAsync_token token);


typedef void MQTTAsync_connectionLost(void *context, char *cause);


typedef struct {

    MQTTAsync_token token;

    int code;

    char *message;
} MQTTAsync_failureData;


typedef struct {

    MQTTAsync_token token;

    union {

        int qos;

        int *qosList;

        struct {
            MQTTAsync_message message;
            char *destinationName;
        } pub;
    } alt;
} MQTTAsync_successData;


typedef void MQTTAsync_onSuccess(void *context, MQTTAsync_successData *response);


typedef void MQTTAsync_onFailure(void *context, MQTTAsync_failureData *response);

typedef struct {

    char struct_id[4];

    int struct_version;

    MQTTAsync_onSuccess *onSuccess;

    MQTTAsync_onFailure *onFailure;

    void *context;
    MQTTAsync_token token;
} MQTTAsync_responseOptions;

#define MQTTAsync_responseOptions_initializer { {'M', 'Q', 'T', 'R'}, 0, NULL, NULL, 0, 0 }


DLLExport int MQTTAsync_setCallbacks(MQTTAsync handle, void *context, MQTTAsync_connectionLost *cl,
                                     MQTTAsync_messageArrived *ma, MQTTAsync_deliveryComplete *dc);


DLLExport int MQTTAsync_create(MQTTAsync *handle, char *serverURI, char *clientId,
                               int persistence_type, void *persistence_context);


typedef struct {

    char struct_id[4];

    int struct_version;

    char *topicName;

    char *message;

    int retained;

    int qos;
} MQTTAsync_willOptions;

#define MQTTAsync_willOptions_initializer { {'M', 'Q', 'T', 'W'}, 0, NULL, NULL, 0, 0 }


typedef struct {

    char struct_id[4];
    int struct_version;

    char *trustStore;

    char *keyStore;


    char *privateKey;

    char *privateKeyPassword;


    char *enabledCipherSuites;


    int enableServerCertAuth;

} MQTTAsync_SSLOptions;

#define MQTTAsync_SSLOptions_initializer { {'M', 'Q', 'T', 'S'}, 0, NULL, NULL, NULL, NULL, NULL, 1 }


typedef struct {

    char struct_id[4];

    int struct_version;

    int keepAliveInterval;

    int cleansession;

    int maxInflight;

    MQTTAsync_willOptions *will;

    char *username;

    char *password;

    int connectTimeout;

    int retryInterval;

    MQTTAsync_SSLOptions *ssl;

    MQTTAsync_onSuccess *onSuccess;

    MQTTAsync_onFailure *onFailure;

    void *context;

    int serverURIcount;

    char **serverURIs;
} MQTTAsync_connectOptions;


#define MQTTAsync_connectOptions_initializer { {'M', 'Q', 'T', 'C'}, 2, 60, 1, 10, NULL, NULL, NULL, 30, 20, NULL, NULL, NULL, NULL, 0, NULL}

DLLExport int MQTTAsync_connect(MQTTAsync handle, MQTTAsync_connectOptions *options);


typedef struct {

    char struct_id[4];

    int struct_version;

    int timeout;

    MQTTAsync_onSuccess *onSuccess;

    MQTTAsync_onFailure *onFailure;

    void *context;
} MQTTAsync_disconnectOptions;

#define MQTTAsync_disconnectOptions_initializer { {'M', 'Q', 'T', 'D'}, 0, 0, NULL, NULL, NULL }


DLLExport int MQTTAsync_disconnect(MQTTAsync handle, MQTTAsync_disconnectOptions *options);


DLLExport int MQTTAsync_isConnected(MQTTAsync handle);


DLLExport int MQTTAsync_subscribe(MQTTAsync handle, char *topic, int qos, MQTTAsync_responseOptions *response);


DLLExport int
MQTTAsync_subscribeMany(MQTTAsync handle, int count, char **topic, int *qos, MQTTAsync_responseOptions *response);


DLLExport int MQTTAsync_unsubscribe(MQTTAsync handle, char *topic, MQTTAsync_responseOptions *response);


DLLExport int MQTTAsync_unsubscribeMany(MQTTAsync handle, int count, char **topic, MQTTAsync_responseOptions *response);


DLLExport int
MQTTAsync_send(MQTTAsync handle, char *destinationName, int payloadlen, void *payload, int qos, int retained,
               MQTTAsync_responseOptions *response);


DLLExport int MQTTAsync_sendMessage(MQTTAsync handle, char *destinationName, MQTTAsync_message *msg,
                                    MQTTAsync_responseOptions *response);


DLLExport int MQTTAsync_getPendingTokens(MQTTAsync handle, MQTTAsync_token **tokens);


DLLExport void MQTTAsync_freeMessage(MQTTAsync_message **msg);


DLLExport void MQTTAsync_free(void *ptr);


DLLExport void MQTTAsync_destroy(MQTTAsync *handle);


enum MQTTASYNC_TRACE_LEVELS {
    MQTTASYNC_TRACE_MAXIMUM = 1,
    MQTTASYNC_TRACE_MEDIUM,
    MQTTASYNC_TRACE_MINIMUM,
    MQTTASYNC_TRACE_PROTOCOL,
    MQTTASYNC_TRACE_ERROR,
    MQTTASYNC_TRACE_SEVERE,
    MQTTASYNC_TRACE_FATAL,
};


DLLExport void MQTTAsync_setTraceLevel(enum MQTTASYNC_TRACE_LEVELS level);


typedef void MQTTAsync_traceCallback(enum MQTTASYNC_TRACE_LEVELS level, char *message);

DLLExport void MQTTAsync_setTraceCallback(MQTTAsync_traceCallback *callback);


typedef struct {
    const char *name;
    const char *value;
} MQTTAsync_nameValue;

DLLExport MQTTAsync_nameValue *MQTTAsync_getVersionInfo();


#endif
