#ifndef BITFINEX_GATEWAY_BITFINEX_DEPTH_H
#define BITFINEX_GATEWAY_BITFINEX_DEPTH_H

#include <stdbool.h>

#define FP_EPSILON 0.04

#include "cjson/cJSON.h"
#include "../depth.h"
#include "../ws/client-server-protocol.h"

OrderBookLevel2 *
bitfinex_parse_depth_update(const char *json_string, OrderBookLevel2 *ptr);

bool
fp_equals(double a, double b, double epsilon);

#endif //BITFINEX_GATEWAY_BITFINEX_DEPTH_H
