//
// Copyright (c) 2022 ZettaScale Technology
//
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// http://www.eclipse.org/legal/epl-2.0, or the Apache License, Version 2.0
// which is available at https://www.apache.org/licenses/LICENSE-2.0.
//
// SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
//
// Contributors:
//   ZettaScale Zenoh Team, <zenoh@zettascale.tech>
//

#ifndef ZENOH_PICO_SYSTEM_ESPIDF_TYPES_H
#define ZENOH_PICO_SYSTEM_ESPIDF_TYPES_H

#include <driver/uart.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include "zenoh-pico/config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if Z_FEATURE_MULTI_THREAD == 1
#include <pthread.h>

typedef struct {
    const char *name;
    UBaseType_t priority;
    size_t stack_depth;
#if (configSUPPORT_STATIC_ALLOCATION == 1)
    bool static_allocation;
    StackType_t *stack_buffer;
    StaticTask_t *task_buffer;
#endif /* SUPPORT_STATIC_ALLOCATION */
} z_task_attr_t;
typedef struct {
    TaskHandle_t handle;
    EventGroupHandle_t join_event;
} _z_task_t;
/*
 * FreeRTOS semaphores rather than pthreads, so that the objects can be allocated where the
 * application says: Z_ESPIDF_SYNC_CAPS, PSRAM by default when the board has it. ESP-IDF's
 * pthread_mutex_init() always takes its FreeRTOS mutex from the internal heap, and a
 * micro-ROS session over rmw_zenoh_pico makes ~150 of them -- ~15 KB of internal DRAM on
 * a board where that is the scarce pool. No ISR touches these, and no task uses them while
 * a flash operation has the cache off (the other core is stalled, this one has the
 * scheduler suspended), which are the two conditions IDF puts on PSRAM-resident objects.
 */
typedef struct {
    SemaphoreHandle_t handle;
} _z_mutex_t;
typedef struct {
    SemaphoreHandle_t handle;
} _z_mutex_rec_t;
typedef struct {
    SemaphoreHandle_t lock;  // guards `waiters`
    SemaphoreHandle_t sem;   // one count per waiter woken
    int waiters;
} _z_condvar_t;
typedef TaskHandle_t _z_task_id_t;
#endif  // Z_FEATURE_MULTI_THREAD == 1

typedef struct timespec z_clock_t;
typedef struct timeval z_time_t;

typedef struct {
    union {
#if defined(ZP_PLATFORM_SOCKET_LINKS_ENABLED)
        int _fd;
#endif
#if Z_FEATURE_LINK_SERIAL == 1
        uart_port_t _serial;
#endif
    };
} _z_sys_net_socket_t;

typedef struct {
    union {
#if defined(ZP_PLATFORM_SOCKET_LINKS_ENABLED)
        struct addrinfo *_iptcp;
#endif
    };
} _z_sys_net_endpoint_t;

#if Z_FEATURE_LINK_BLUETOOTH == 1
#error "Bluetooth not supported yet on ESP-IDF port of Zenoh-Pico"
#endif

#if Z_FEATURE_RAWETH_TRANSPORT == 1
#error "Raw ethernet transport not supported yet on ESP-IDF port of Zenoh-Pico"
#endif

#ifdef __cplusplus
}
#endif

#endif /* ZENOH_PICO_SYSTEM_ESPIDF_TYPES_H */
