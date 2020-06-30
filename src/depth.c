#include <stdlib.h>
#include <libwebsockets.h>
#include "depth.h"

double
_calculate_vwap(Order order_levels[], unsigned int length) {
    double size = 0;
    double vwap = 0;
    for (unsigned int i = 0; size < 1.0 && i < length; i++) {
        size += order_levels[i].amount;
        vwap += order_levels[i].amount * order_levels[i].price;
    }
    if (size == 0) {
        return 0;
    }
    return vwap / size;
}

void
order_book_calculate_mid(OrderBookLevel2 *order_book) {
    if (order_book->bids_length < 1 || order_book->asks_length < 1) {
        order_book->mid = 0.0;
        lwsl_warn("%s: ZERO length bids=%d, asks=%d\n", __func__, order_book->bids_length,
                  order_book->asks_length);
        return;
    }
    double bid = _calculate_vwap(order_book->bids, order_book->bids_length);
    double ask = _calculate_vwap(order_book->asks, order_book->asks_length);

    if (order_book->asks[0].price == 0) {
        lwsl_err("%s: Found ZERO level ASK price=%.1f, amount=%.1f\n", __func__, order_book->asks[0].price,
                 order_book->asks[0].amount);
        exit(1);
    }
    if (order_book->bids[0].price == 0) {
        lwsl_err("%s: Found ZERO level BID price=%.1f, amount=%.1f\n", __func__, order_book->bids[0].price,
                 order_book->bids[0].amount);
        exit(1);
    }

    if (order_book->bids[0].price > order_book->asks[0].price) {
        lwsl_warn("%s: Found best BID crossed best ASK bid=%.1f, ask=%.1f\n", __func__, order_book->bids[0].price,
                  order_book->asks[0].price);
    }
    if (bid > ask) {
        lwsl_err("%s: Found CROSSED levels bid=%f, ask=%f, best_bid=%.1f, best_ask=%.1f\n", __func__, bid, ask, order_book->bids[0].price,
                 order_book->asks[0].price);
        exit(1);
    }
    if (ask > 0 && bid > 0) {
        order_book->mid = (ask + bid) / 2;
    }
//    lwsl_user("bid=%f, best_bid=%f, mid=%f, best_ask=%f, ask=%f\n", bid, order_book->bids[0].price, order_book->mid,
//              order_book->asks[0].price, ask);
}

void
order_book_delete(OrderBookLevel2 *order_book) {
    if (order_book == NULL) {
        return;
    }
    free(order_book->exchange);
    free(order_book->market_name);
    free(order_book);
}
