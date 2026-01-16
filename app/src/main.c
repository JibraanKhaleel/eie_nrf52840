/*
 * main.c
 */

#include <stdio.h>
#include <string.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/printk.h>
#include "LED.h"
#include "BTN.h"



#define SLEEP_MS 1


//UUID for service and characteristics
#define UUID_SERVICE BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0)
#define UUID_CHARACTERISTIC BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef2) //changed last digit to 2 to make it different
#define UUID_CHARACTERISTIC_NEW BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef3) //new characteristic 

//variables
static const struct bt_data ble_advertising_data[] = {
  BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
  BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME)),
};

static uint8_t ble_custom_characteristic_user_data[20] = {};

//read callback
static ssize_t ble_custom_characteristic_read_cb(struct bt_conn* conn, const struct bt_gatt_attr* attr,
                                                 void* buf, uint16_t len, uint16_t offset) {
  const char* value = attr->user_data;
  return bt_gatt_attr_read(conn, attr, buf, len, offset, value, strlen(value));
}


//write callback
static ssize_t ble_custom_characteristic_write_cb(struct bt_conn* conn, const struct bt_gatt_attr* attr,
                                                  const void* buf, uint16_t len, uint16_t offset,
                                                  uint8_t flags) {
  uint8_t* value_ptr = attr->user_data;

  if (offset + len > 20) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
  }

  memcpy(value_ptr + offset, buf, len);
  value_ptr[offset + len] = 0;

  return len;
}

//service and characteristic description
static const struct bt_uuid_128 ble_custom_service_uuid = BT_UUID_INIT_128(UUID_SERVICE);
static const struct bt_uuid_128 ble_custom_characteristic_uuid = BT_UUID_INIT_128(UUID_CHARACTERISTIC);
static const struct bt_uuid_128 ble_custom_characteristic_new_uuid = BT_UUID_INIT_128(UUID_CHARACTERISTIC_NEW);
//service definition macro

BT_GATT_SERVICE_DEFINE(
    ble_custom_service,  // Name of the struct that will store the config for this service
    BT_GATT_PRIMARY_SERVICE(&ble_custom_service_uuid),  // Setting the service UUID
    
    BT_GATT_CHARACTERISTIC(
        &ble_custom_characteristic_uuid.uuid,  // Setting the characteristic UUID
        BT_GATT_CHRC_WRITE | BT_GATT_CHRC_READ,  // Possible operations
        BT_GATT_PERM_READ,  // Permissions that connecting devices have
        ble_custom_characteristic_read_cb,     // Callback for when this characteristic is read from
        ble_custom_characteristic_write_cb,    // Callback for when this characteristic is written to
        ble_custom_characteristic_user_data    // Initial data stored in this characteristic
        ),

    BT_GATT_CHARACTERISTIC(
      &ble_custom_characteristic_new_uuid.uuid,
      BT_GATT_CHRC_WRITE | BT_GATT_CHRC_READ,
      BT_GATT_PERM_WRITE,
      ble_custom_characteristic_read_cb,
      ble_custom_characteristic_write_cb,
      ble_custom_characteristic_user_data
    ),
);

int main(void) {

  if (0 > BTN_init()) {
    return 0;
  }
  if (0 > LED_init()) {
    return 0;
  }


  //enable + check
  int bt_en_rv = bt_enable(NULL);

  if(bt_en_rv != 0)
    printk("error in bt_en_rv: %d",bt_en_rv);

  int bt_le_adv_start_rv = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, ble_advertising_data, ARRAY_SIZE(ble_advertising_data), NULL, 0);
  if(bt_le_adv_start_rv != 0)
    printk("error in bt_le_adv_start: %d", bt_en_rv);

  

  while(1) {
    
    k_msleep(SLEEP_MS);
  }
	return 0;
}
