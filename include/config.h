#ifndef CONFIG_H
#define CONFIG_H
#define RXD2 16
#define TXD2 17

/*==============================
        PROJECT
==============================*/

#define PROJECT_NAME "Smart Energy Meter"

/*==============================
        OLED
==============================*/

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define OLED_ADDRESS 0x3C

#define OLED_SDA 21
#define OLED_SCL 22

/*==============================
        PZEM
==============================*/

#define PZEM_RX 16
#define PZEM_TX 17

/*==============================
        UPDATE
==============================*/

#define SENSOR_INTERVAL 500

#define OLED_INTERVAL 3000

#define GRAPH_INTERVAL 1000

/*==============================
        LIMIT
==============================*/

#define DEFAULT_POWER_LIMIT 700.0

/*==============================
        WIFI
==============================*/

#define AP_NAME "SmartEnergyMeter"

#define AP_PASSWORD "12345678"

#endif