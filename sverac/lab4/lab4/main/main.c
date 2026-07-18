#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "sdkconfig.h"

#define SERVICE_UUID 0x6602
#define CHAR1_UUID 0x6603
#define CHAR2_UUID 0x6604
#define CHAR3_UUID 0x6605

static uint32_t counterBLE = 0;
static uint32_t readBLE = 36546602;
static uint8_t writeBLE = 1;

static uint16_t connection_handle = 0;
static uint16_t char1_handle;

char *TAG = "BLE-Server";
uint8_t ble_addr_type;

static bool connected = false;

void ble_app_advertise(void);

static int counter_access(uint16_t connection_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg){
    char buffer[16];
    int length = snprintf(buffer, sizeof(buffer), "%lu", counterBLE);
    os_mbuf_append(ctxt->om, buffer, length);
    return 0;
}

static int read_access(uint16_t connection_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    char buffer[16];
    int length = snprintf(buffer, sizeof(buffer), "%lu", readBLE);
    os_mbuf_append(ctxt->om, buffer, length);
    return 0;
}

static int write_access(uint16_t connection_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg){
    if(ctxt->om->om_len == 1){
        uint8_t value = ctxt->om->om_data[0];
        if(value >= 1 && value <= 10){
            writeBLE = value;
            ESP_LOGI(TAG, "New BLE value: %d", writeBLE);
        }
    }
    return 0;
}

static const struct ble_gatt_svc_def gatt_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(SERVICE_UUID),

        .characteristics = (struct ble_gatt_chr_def[])
        {
            {
                .uuid = BLE_UUID16_DECLARE(CHAR1_UUID),
                .access_cb = counter_access,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &char1_handle
            },
            {
                .uuid = BLE_UUID16_DECLARE(CHAR2_UUID),
                .access_cb = read_access,
                .flags = BLE_GATT_CHR_F_READ
            },
            {
                .uuid = BLE_UUID16_DECLARE(CHAR3_UUID),
                .access_cb = write_access,
                .flags = BLE_GATT_CHR_F_WRITE
            },
            {0}
        }
    },{0}
};

// BLE event handling
static int ble_gap_event(struct ble_gap_event *event, void *arg){
    switch (event->type){ 
        // Advertise if connected
        case BLE_GAP_EVENT_CONNECT:
            ESP_LOGI("GAP", "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
            if (event->connect.status == 0){
                connection_handle = event->connect.conn_handle;
                ESP_LOGI(TAG, "Connected");
            }else{
                ble_app_advertise();
            }
            connected = true;
            break;

        // Advertise again after completion of the event
        case BLE_GAP_EVENT_ADV_COMPLETE:
            ESP_LOGI("GAP", "BLE GAP EVENT");
            ble_app_advertise();
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            ESP_LOGI(TAG, "Disconnected");
            connection_handle = 0;
            ble_app_advertise();
            connected = false;
            break;

        default:
            break;
    }
    return 0;
}

void counter_task(void* arg){
    while(1){
        counterBLE += writeBLE;
        if(connected && char1_handle != 0){
            ble_gatts_chr_updated(char1_handle);
        }

        ESP_LOGI(TAG,"counter: %lu",counterBLE);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Define the BLE connection
void ble_app_advertise(void)
{
    struct ble_hs_adv_fields fields;
    memset(&fields, 0, sizeof(fields));

    const char *name = ble_svc_gap_device_name();

    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    fields.name = (uint8_t *)name;
    fields.name_len = strlen(name);
    fields.name_is_complete = 1;

    ble_gap_adv_set_fields(&fields);

    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));

    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER,
                      &adv_params, ble_gap_event, NULL);
}

void ble_app_on_sync(void){
    ble_hs_id_infer_auto(0, &ble_addr_type); // Determines the best address type automatically
    ble_app_advertise(); // Define the BLE connection
}

void host_task(void *param){
    nimble_port_run();
}

void app_main(){
    nvs_flash_init();

    nimble_port_init();

    ble_svc_gap_init();
    ble_svc_gatt_init();
    ble_svc_gap_device_name_set("IvanVjekoslavRodak");
    ESP_LOGI(TAG, "Name: %s", ble_svc_gap_device_name());

    int rc;

    rc = ble_gatts_count_cfg(gatt_svcs);
    ESP_LOGI(TAG, "count_cfg rc=%d", rc);

    rc = ble_gatts_add_svcs(gatt_svcs);
    ESP_LOGI(TAG, "add_svcs rc=%d", rc);
    
    ESP_LOGI(TAG, "char1_handle=%d", char1_handle);
    ble_hs_cfg.sync_cb = ble_app_on_sync;

    nimble_port_freertos_init(host_task);

    xTaskCreate(counter_task, "counter_task", 4096, NULL, 5, NULL);
}