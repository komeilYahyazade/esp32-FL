#ifndef _httpclient
#define _httpclient

#include "esp_http_client.h"
#include "federatedlearning.h"
#include "JSONConverter.h"

#define GET_GLOBAL_MODEL "http://217.60.251.10:8888/api/getglobalmodel"
#define GET_GLOBAL_MODEL_STATUS "http://217.60.251.10:8888/api/checkglobalmodel"
#define POST_GLOBAL_MODEL "http://217.60.251.10:8888/api/postglobalmodel"
#define GET_NODE_REGISTER "http://217.60.251.10:8888/api/noderegister"
#define GET_MODEL_STATUS "http://217.60.251.10:8888/api/checkglobalmodel"
#define GET_TEST_SERVER "http://217.60.251.10:8888/api/testget"

FederatedLearning* getglobalmodel();
int getglobalmodelstatus();
void postglobalmodel();
void registernode();
bool isserverworking();

#endif
