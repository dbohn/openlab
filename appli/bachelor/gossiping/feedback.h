#ifndef GSP_FEEDBACK_H
#define GSP_FEEDBACK_H

#include "iotlab_uid.h"
#include "printf.h"
#include "config.h"

#if defined(DEBUG) && DEBUG == 1
#define DEBUG_MSG(fmt, ...) printf("%04x;%u;DBG;" fmt, iotlab_uid(), soft_timer_ticks_to_ms(soft_timer_time()), ##__VA_ARGS__)
#else
#define DEBUG_MSG(fmt, ...)
#endif

#define MESSAGE(fmt, ...) printf("%04x;%u;MSG;" fmt, iotlab_uid(), soft_timer_ticks_to_ms(soft_timer_time()), ##__VA_ARGS__)

#define ERROR(fmt, ...) printf("%04x;%u;ERR;" fmt, iotlab_uid(), soft_timer_ticks_to_ms(soft_timer_time()), ##__VA_ARGS__)

#endif