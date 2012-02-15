/*
 * This file is part of HiKoB Openlab.
 *
 * HiKoB Openlab is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, version 3.
 *
 * HiKoB Openlab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with HiKoB Openlab. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2011 HiKoB.
 */

/*
 * usb_hid.c
 *
 *  Created on: Aug 30, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdbool.h>
#include "platform.h"
#define NO_DEBUG_HEADER
#define LOG_LEVEL LOG_LEVEL_DEBUG
#include "printf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usb.h"
#include "hid.h"

static xTaskHandle vMouseTaskHandle;

static void vMouseTask(void *pvParameters);

int main()
{
    signed portBASE_TYPE ret;

    // Initialize the platform
    platform_init();

    log_info("USB HID test\r\n================\r\n");

    // Initialize USB
    usb_init(&hid);

#if PLATFORM == inemo
    // iNemo specific code
    gpio_enable(gpioA);
    gpio_set_output(gpioA, GPIO_PIN_10);
    gpio_config_output_type(gpioA, GPIO_PIN_10, GPIO_TYPE_PUSH_PULL);
    gpio_pin_set(gpioA, GPIO_PIN_10);
#endif

#if PLATFORM == agile-fox
    gpio_enable(gpioC);
    gpio_set_output(gpioC, GPIO_PIN_7);
    gpio_config_output_type(gpioC, GPIO_PIN_7, GPIO_TYPE_PUSH_PULL);
    gpio_pin_set(gpioC, GPIO_PIN_7);
#endif

    leds_on(LED_1);

    ret = xTaskCreate(vMouseTask, (signed char *)"Mouse", 4 * configMINIMAL_STACK_SIZE, NULL, 1, &vMouseTaskHandle);

    switch(ret)
    {
        case errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY:
            log_error("Could not allocate required memory");

            while(1);

            return 0;

        default:
            log_debug("Mouse task created successfully");
    }

    // Start the scheduler
    platform_run();

    return 0;
}

int8_t cos[] = {127, 126, 126, 126, 126, 126, 126, 126, 125, 125, 125, 124, 124, 123, 123, 122, 122, 121, 120, 120, 119, 118, 117, 116, 116, 115, 114, 113, 112, 111, 109, 108, 107, 106, 105, 104, 102, 101, 100, 98, 97, 95, 94, 92, 91, 89, 88, 86, 84, 83, 81, 79, 78, 76, 74, 72, 71, 69, 67, 65, 63, 61, 59, 57, 55, 53, 51, 49, 47, 45, 43, 41, 39, 37, 35, 32, 30, 28, 26, 24, 22, 19, 17, 15, 13, 11, 8, 6, 4, 2, 0, -2, -4, -6, -8, -11, -13, -15, -17, -19, -22, -24, -26, -28, -30, -32, -35, -37, -39, -41, -43, -45, -47, -49, -51, -53, -55, -57, -59, -61, -63, -65, -67, -69, -71, -72, -74, -76, -78, -79, -81, -83, -84, -86, -88, -89, -91, -92, -94, -95, -97, -98, -100, -101, -102, -104, -105, -106, -107, -108, -109, -111, -112, -113, -114, -115, -116, -116, -117, -118, -119, -120, -120, -121, -122, -122, -123, -123, -124, -124, -125, -125, -125, -126, -126, -126, -126, -126, -126, -126, -127, -126, -126, -126, -126, -126, -126, -126, -125, -125, -125, -124, -124, -123, -123, -122, -122, -121, -120, -120, -119, -118, -117, -116, -116, -115, -114, -113, -112, -111, -109, -108, -107, -106, -105, -104, -102, -101, -100, -98, -97, -95, -94, -92, -91, -89, -88, -86, -84, -83, -81, -79, -78, -76, -74, -72, -71, -69, -67, -65, -63, -61, -59, -57, -55, -53, -51, -49, -47, -45, -43, -41, -39, -37, -35, -32, -30, -28, -26, -24, -22, -19, -17, -15, -13, -11, -8, -6, -4, -2, 0, 2, 4, 6, 8, 11, 13, 15, 17, 19, 22, 24, 26, 28, 30, 32, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 72, 74, 76, 78, 79, 81, 83, 84, 86, 88, 89, 91, 92, 94, 95, 97, 98, 100, 101, 102, 104, 105, 106, 107, 108, 109, 111, 112, 113, 114, 115, 116, 116, 117, 118, 119, 120, 120, 121, 122, 122, 123, 123, 124, 124, 125, 125, 125, 126, 126, 126, 126, 126, 126, 126};
int8_t sin[] = {0, 2, 4, 6, 8, 11, 13, 15, 17, 19, 22, 24, 26, 28, 30, 32, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 72, 74, 76, 78, 79, 81, 83, 84, 86, 88, 89, 91, 92, 94, 95, 97, 98, 100, 101, 102, 104, 105, 106, 107, 108, 109, 111, 112, 113, 114, 115, 116, 116, 117, 118, 119, 120, 120, 121, 122, 122, 123, 123, 124, 124, 125, 125, 125, 126, 126, 126, 126, 126, 126, 126, 127, 126, 126, 126, 126, 126, 126, 126, 125, 125, 125, 124, 124, 123, 123, 122, 122, 121, 120, 120, 119, 118, 117, 116, 116, 115, 114, 113, 112, 111, 109, 108, 107, 106, 105, 104, 102, 101, 100, 98, 97, 95, 94, 92, 91, 89, 88, 86, 84, 83, 81, 79, 78, 76, 74, 72, 71, 69, 67, 65, 63, 61, 59, 57, 55, 53, 51, 49, 47, 45, 43, 41, 39, 37, 35, 32, 30, 28, 26, 24, 22, 19, 17, 15, 13, 11, 8, 6, 4, 2, 0, -2, -4, -6, -8, -11, -13, -15, -17, -19, -22, -24, -26, -28, -30, -32, -35, -37, -39, -41, -43, -45, -47, -49, -51, -53, -55, -57, -59, -61, -63, -65, -67, -69, -71, -72, -74, -76, -78, -79, -81, -83, -84, -86, -88, -89, -91, -92, -94, -95, -97, -98, -100, -101, -102, -104, -105, -106, -107, -108, -109, -111, -112, -113, -114, -115, -116, -116, -117, -118, -119, -120, -120, -121, -122, -122, -123, -123, -124, -124, -125, -125, -125, -126, -126, -126, -126, -126, -126, -126, -127, -126, -126, -126, -126, -126, -126, -126, -125, -125, -125, -124, -124, -123, -123, -122, -122, -121, -120, -120, -119, -118, -117, -116, -116, -115, -114, -113, -112, -111, -109, -108, -107, -106, -105, -104, -102, -101, -100, -98, -97, -95, -94, -92, -91, -89, -88, -86, -84, -83, -81, -79, -78, -76, -74, -72, -71, -69, -67, -65, -63, -61, -59, -57, -55, -53, -51, -49, -47, -45, -43, -41, -39, -37, -35, -32, -30, -28, -26, -24, -22, -19, -17, -15, -13, -11, -8, -6, -4, -2};

static void vMouseTask(void *pvParameters)
{
    uint16_t i = 0;

    while(1)
    {
        vTaskDelay(configTICK_RATE_HZ / 100);

        hid_send(cos[i] / 40, sin[i] / 40, true, false, false);

        i++;

        if(i >= 360)
        {
            i = 0;
        }
    }
}
