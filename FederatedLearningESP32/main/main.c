#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

#include "espconfiguration.h"
#include "httpclient.h"
#include "websocketclient.h"
#include "federatedlearning.h"
#include "trainingConfigs.h"
#include "JSONConverter.h"

#include "esp_spiffs.h"

//#include "esp_websocket_client.h"


void deep_learning(){
    // test if http server is on!
    while (! isserverworking()) {
        printf("http server is not running ....!!\n");
        vTaskDelay(750 / portTICK_PERIOD_MS);
    }

    // register client at server
    printf("Register node in server ...\n");
    registernode();
    vTaskDelay(500 / portTICK_PERIOD_MS);
    
    // run federated interactions
    while (1) {
        int globalstatus = getglobalmodelstatus();
        if (globalstatus == 1) {
            // new interaction has began and this client hasn't interacted yet
            FederatedLearning *globalmodel = getglobalmodel();
            if (globalmodel == NULL) {
                printf("received model is null, something is wrong ...\ntry again ...\n");
                continue;
            }
            // set received neural net as the global neural net and set its training counter to 0
            replaceNeuralNetwork(globalmodel);

            // get client federated learning instance and train model
            FederatedLearning *FDI = getFederatedLearningInstance();
            PrintNeuralNetwork(FDI->neuralnetwork);
            NeuralNetworkTraining();
            FDI->trainingscounter += 1;

            // check performance metrics of client trained model on training data
            printf("client model performance on training data: \n");
            PerformanceMetrics(FDI->neuralnetwork, DATA_COUNT, 0.5);

            // print trained model and send it to server
            char *json_string = cJSON_Print(federatedLearningToJSON(FDI));
            if (json_string == NULL) {
                fprintf(stderr, "Error in json string of client federated model.\n");
                return ;
            }
            printf("%s\n", json_string);
            free(json_string);

            websocket_send_local_model();
        }

        // wait till next interaction has begone
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void setupESPdevice(){
    UARTConfiguration();
    WIFIConfiguration();
    SPIFFSConfiguration();
    vTaskDelay(2000 / portTICK_PERIOD_MS);
}

void app_main(void){
    setupESPdevice();
    deep_learning();
}
