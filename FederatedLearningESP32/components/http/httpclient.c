#include "httpclient.h"
#include "esp_log.h"

#define TAG "HTTP_CLIENT"

int is_utf8(const char *str) {
    while (*str) {
        if ((*str & 0x80) == 0) {
            // Caractere ASCII de um byte
            str++;
        } else if ((*str & 0xE0) == 0xC0) {
            // Caractere multibyte de dois bytes
            if ((str[1] & 0xC0) != 0x80)
                return 0;
            str += 2;
        } else if ((*str & 0xF0) == 0xE0) {
            // Caractere multibyte de três bytes
            if ((str[1] & 0xC0) != 0x80 || (str[2] & 0xC0) != 0x80)
                return 0;
            str += 3;
        } else if ((*str & 0xF8) == 0xF0) {
            // Caractere multibyte de quatro bytes
            if ((str[1] & 0xC0) != 0x80 || (str[2] & 0xC0) != 0x80 || (str[3] & 0xC0) != 0x80)
                return 0;
            str += 4;
        } else {
            // Byte inválido
            return 0;
        }
    }
    return 1;
}


// Estrutura para armazenar os dados da requisição
cJSON *req;

// Função de manipulador de eventos para requisições GET
esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGE(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_DATA:
            req = cJSON_Parse((char *)evt->data);
            if (req == NULL) {
                const char *error_ptr = cJSON_GetErrorPtr();
                if (error_ptr != NULL) {
                    fprintf(stderr, "Errors in Json Analysis: %s\n", error_ptr);
                }
                printf("Received Json is null\n");
                return ESP_FAIL;
            }
            else{
                printf("Received Data:\n");
                char *jsonString = cJSON_Print(req);
                printf("JSON: %s\n", jsonString);
                free(jsonString);
            }
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
        default:
            break;
        }
    return ESP_OK;
}


esp_err_t client_event_post_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
            break;
        default:
            break;
        }
        
    return ESP_OK;
}


// Função para fazer uma requisição GET para um recurso específico e retornar os dados
void perform_get_request(const char *url)
{
    esp_http_client_config_t config_get = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler,
        .buffer_size = 4096,
        .skip_cert_common_name_check = true, 
    };

    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_err_t err = esp_http_client_perform(client);
    // while (err != ESP_OK) {
    //     err = esp_http_client_perform(client);
    //     vTaskDelay(750 / portTICK_PERIOD_MS);
    //     printf("perform while true ... \n");
    // }
    esp_http_client_cleanup(client);
}

void perform_post_request(const char *url, cJSON *json_data) {
    esp_http_client_config_t config_post = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .cert_pem = NULL,
        .buffer_size = 4096,
        .event_handler = client_event_post_handler
    };
        
    esp_http_client_handle_t client = esp_http_client_init(&config_post);

    char *post_data = cJSON_Print(json_data);
    // const char *post_data = "{\"field1\":\"value1\"}";
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "POST request sent successfully");
    } else {
        ESP_LOGE(TAG, "Failed to send POST request, error code: %d", err);
    }

    esp_http_client_cleanup(client);
    free(post_data);
    // free(post_data);
}

FederatedLearning * getglobalmodel() {
    perform_get_request(GET_GLOBAL_MODEL);
    FederatedLearning *FederatedLearningInstance = JSONToFederatedLearning(req);
    cJSON_Delete(req);
    return FederatedLearningInstance;
}

int getglobalmodelstatus() {
    perform_get_request(GET_GLOBAL_MODEL_STATUS);

    if (req == NULL) {
        printf("probably server went off!\n");
        return -1;
    } else{
        printf("req is not null\n");
    }

    cJSON *status_item = cJSON_GetObjectItem(req, "status");
    if (status_item == NULL) {
        printf("Error getting 'status' item.\n");
        cJSON_Delete(req);
        return -1;
    }

    if (!cJSON_IsNumber(status_item)) {
        printf("Value of 'status' is not a number.\n");
        cJSON_Delete(req);
        return -1;
    }

    int status = status_item->valueint;
    cJSON_Delete(req);

    return status;
}

void postglobalmodel(){
    // char *json_string = cJSON_Print(federatedLearningToJSON(getFederatedLearningInstance()));
    
    // // teste();
    FederatedLearning* FDI =  getFederatedLearningInstance();
    // PrintNeuralNetwork(FDI->neuralnetwork);
    cJSON *json_data = federatedLearningToJSON(FDI);
    // char *json_string = cJSON_Print(json_data);
    // // printf("%s\n", json_string);
    // // teste();
    perform_post_request(POST_GLOBAL_MODEL, json_data);
    cJSON_Delete(json_data);
    // free(json_string);
    // // teste();
}


void registernode() {
    perform_get_request(GET_NODE_REGISTER);
    cJSON_Delete(req);
}

bool isserverworking() {
    perform_get_request(GET_TEST_SERVER);
    bool is_running = req != NULL;
    cJSON_Delete(req);
    return is_running;
    // char* json_string = cJSON_Print(req);
    // return (strstr(json_string, "/api/testget") != NULL);
}