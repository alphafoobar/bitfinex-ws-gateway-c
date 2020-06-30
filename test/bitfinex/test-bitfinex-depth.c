#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>

#include "bitfinex/bitfinex-depth.h"
#include "../file.h"

static void
test_parse_info() {
    char *depth_json = read_file("resources/bitfinex-info.json", 103);
    OrderBookLevel2 bookLevel2;
    OrderBookLevel2 *actual = bitfinex_parse_depth_update(depth_json, &bookLevel2);
    assert_null(actual);

    free(depth_json);
//    order_book_delete(actual);
}

static void
test_parse_subbed() {
    char *depth_json = read_file("resources/bitfinex-subbed.json", 125);
    OrderBookLevel2 bookLevel2;
    OrderBookLevel2 *actual = bitfinex_parse_depth_update(depth_json, &bookLevel2);

    assert_null(actual);

    free(depth_json);
//    order_book_delete(actual);
}

static void
test_parse_depth_snapshot_btcusd() {
    char *depth_json = read_file("resources/bitfinex-snapshot.json", 973);
    OrderBookLevel2 bookLevel2;
    bookLevel2.exchange = strndup("bitfinex", 20);
    bookLevel2.market_name = strndup("btc/usd", 20);
    OrderBookLevel2 *actual = bitfinex_parse_depth_update(depth_json, &bookLevel2);
    assert_string_equal(actual->exchange, "bitfinex");
    assert_string_equal(actual->market_name, "btc/usd");
    assert_int_equal(actual->bids_length, 25);
    assert_int_equal(actual->asks_length, 25);
    assert_float_equal(actual->bids[0].price, 10196, 0.01);
    assert_float_equal(actual->bids[0].amount, 8.27552856, 0.01);
    assert_float_equal(actual->bids[1].price, 10195, 0.01);
    assert_float_equal(actual->bids[1].amount, 0.001, 0.01);
    assert_float_equal(actual->asks[0].price, 10197, 0.01);
    assert_float_equal(actual->asks[0].amount, 0.0053093, 0.01);

    free(depth_json);
//    order_book_delete(actual);
}

static void
test_mid() {
    char *depth_json = read_file("resources/bitfinex-snapshot.json", 973);
    OrderBookLevel2 bookLevel2;
    OrderBookLevel2 *actual = bitfinex_parse_depth_update(depth_json, &bookLevel2);

    order_book_calculate_mid(actual);
    assert_float_equal(actual->mid, 10197.995, 0.001);

    free(depth_json);
//    order_book_delete(actual);
}

static void
test_parse_depth_update_btcusd() {
    char *snapshot_json = read_file("resources/bitfinex-snapshot.json", 973);
    OrderBookLevel2 bookLevel2;
    bookLevel2.exchange = strndup("bitfinex", 20);
    bookLevel2.market_name = strndup("btc/usd", 20);
    bookLevel2.asks_length = 0;
    bookLevel2.bids_length = 0;

    // Check snapshot.
    OrderBookLevel2 *initial = bitfinex_parse_depth_update(snapshot_json, &bookLevel2);
    order_book_calculate_mid(initial);
    assert_float_equal(initial->mid, 10197.995, 0.001);

    char *depth_json = read_file("resources/bitfinex-update.json", 22);
    OrderBookLevel2 *actual = bitfinex_parse_depth_update(depth_json, &bookLevel2);

    order_book_calculate_mid(actual);
    assert_float_equal(actual->mid, 10197.995, 0.001);
    assert_string_equal(actual->exchange, "bitfinex");
    assert_string_equal(actual->market_name, "btc/usd");
    // Update is applied on top of previous snapshots.
    assert_int_equal(actual->bids_length, 25);
    assert_int_equal(actual->asks_length, 24);
    assert_float_equal(actual->asks[0].price, 10197, 0.01);
    assert_float_equal(actual->asks[0].amount, 0.0053093, 0.01);
    assert_float_equal(actual->bids[0].price, 10196, 0.01);
    assert_float_equal(actual->bids[0].amount, 8.27552856, 0.01);
    assert_float_equal(actual->asks[23].price, 10221, 0.01);
    assert_float_equal(actual->asks[23].amount, 2.69390221, 0.01);

    free(snapshot_json);
    free(depth_json);
//    order_book_delete(actual);
}

static void
test_parse_depth_update_btcusd_inside() {
    char *snapshot_json = read_file("resources/bitfinex-snapshot.json", 973);
    OrderBookLevel2 bookLevel2;
    bookLevel2.exchange = strndup("bitfinex", 20);
    bookLevel2.market_name = strndup("btc/usd", 20);
    bookLevel2.asks_length = 0;
    bookLevel2.bids_length = 0;

    // Check snapshot.
    OrderBookLevel2 *initial = bitfinex_parse_depth_update(snapshot_json, &bookLevel2);
    order_book_calculate_mid(initial);
    assert_float_equal(initial->mid, 10197.995, 0.0001);

    OrderBookLevel2 *actual = bitfinex_parse_depth_update("[453097,[10197,0,-1]]", &bookLevel2);

    order_book_calculate_mid(actual);
    assert_float_equal(actual->mid, 10197.999, 0.0001);
    assert_string_equal(actual->exchange, "bitfinex");
    assert_string_equal(actual->market_name, "btc/usd");
    // Update is applied on top of previous snapshots.
    assert_int_equal(actual->bids_length, 25);
    assert_int_equal(actual->asks_length, 24);
    assert_float_equal(actual->asks[0].price, 10198, 0.01);
    assert_float_equal(actual->asks[0].amount, 0.001, 0.01);
    assert_float_equal(actual->asks[23].price, 10222, 0.01);
    assert_float_equal(actual->asks[23].amount, 5.95598538, 0.01);

    free(snapshot_json);
//    order_book_delete(actual);
}

static void
test_parse_depth_update_btcusd_inside_bids() {
    char *snapshot_json = read_file("resources/bitfinex-snapshot.json", 973);
    OrderBookLevel2 bookLevel2;
    bookLevel2.exchange = strndup("bitfinex", 20);
    bookLevel2.market_name = strndup("btc/usd", 20);
    bookLevel2.asks_length = 0;
    bookLevel2.bids_length = 0;

    // Check snapshot.
    OrderBookLevel2 *initial = bitfinex_parse_depth_update(snapshot_json, &bookLevel2);
    order_book_calculate_mid(initial);
    assert_float_equal(initial->mid, 10197.995, 0.001);

    OrderBookLevel2 *actual = bitfinex_parse_depth_update("[453097,[10195,0,1]]", &bookLevel2);

    order_book_calculate_mid(actual);
    assert_float_equal(actual->mid, 10197.995, 0.0001);
    assert_string_equal(actual->exchange, "bitfinex");
    assert_string_equal(actual->market_name, "btc/usd");
    // Update is applied on top of previous snapshots.
    assert_int_equal(actual->bids_length, 24);
    assert_int_equal(actual->asks_length, 25);
    assert_float_equal(actual->bids[0].price, 10196, 0.01);
    assert_float_equal(actual->bids[0].amount, 8.27552856, 0.01);
    assert_float_equal(actual->bids[23].price, 10172, 0.01);
    assert_float_equal(actual->bids[23].amount, 0.976, 0.01);
    assert_float_equal(actual->asks[0].price, 10197, 0.01);
    assert_float_equal(actual->asks[0].amount, 0.001, 0.01);
    assert_float_equal(actual->asks[24].price, 10222, 0.01);
    assert_float_equal(actual->asks[24].amount, 5.95598538, 0.01);

    free(snapshot_json);
//    order_book_delete(actual);
}

static void
test_remove_best_bid() {
    char *snapshot_json = read_file("resources/bitfinex-snapshot.json", 973);
    OrderBookLevel2 bookLevel2;
    bookLevel2.exchange = strndup("bitfinex", 20);
    bookLevel2.market_name = strndup("btc/usd", 20);
    bookLevel2.asks_length = 0;
    bookLevel2.bids_length = 0;

    // Check snapshot.
    OrderBookLevel2 *initial = bitfinex_parse_depth_update(snapshot_json, &bookLevel2);
    order_book_calculate_mid(initial);
    assert_float_equal(initial->mid, 10197.995, 0.001);

    OrderBookLevel2 *actual = bitfinex_parse_depth_update("[453097,[10196,0,1]]", &bookLevel2);

    order_book_calculate_mid(actual);
    assert_float_equal(actual->mid, 10196.166, 0.001);
    assert_string_equal(actual->exchange, "bitfinex");
    assert_string_equal(actual->market_name, "btc/usd");
    // Update is applied on top of previous snapshots.
    assert_int_equal(actual->bids_length, 24);
    assert_int_equal(actual->asks_length, 25);
    assert_float_equal(actual->bids[0].price, 10195, 0.01);
    assert_float_equal(actual->bids[0].amount, 0.001, 0.01);
    assert_float_equal(actual->bids[23].price, 10172, 0.01);
    assert_float_equal(actual->bids[23].amount, 0.976, 0.01);
    assert_float_equal(actual->asks[0].price, 10197, 0.01);
    assert_float_equal(actual->asks[0].amount, 0.0053093, 0.01);
    assert_float_equal(actual->asks[24].price, 10222, 0.01);
    assert_float_equal(actual->asks[24].amount, 5.95598538, 0.01);

    free(snapshot_json);
//    order_book_delete(actual);
}

static void
test_remove_best_ask() {
    char *snapshot_json = read_file("resources/bitfinex-snapshot.json", 973);
    OrderBookLevel2 bookLevel2 = {
            .exchange = "bitfinex",
            .market_name = "btc/usd",
            .bids_length = 0,
            .asks_length = 0,
            .bids = {},
            .asks = {},
    };

    // Check snapshot.
    OrderBookLevel2 *initial = bitfinex_parse_depth_update(snapshot_json, &bookLevel2);
    order_book_calculate_mid(initial);
    assert_float_equal(initial->mid, 10197.995, 0.001);

    OrderBookLevel2 *actual = bitfinex_parse_depth_update("[453097,[10197,0,-1]]", &bookLevel2);

    order_book_calculate_mid(actual);
    assert_float_equal(actual->mid, 10197.999, 0.001);
    assert_string_equal(actual->exchange, "bitfinex");
    assert_string_equal(actual->market_name, "btc/usd");
    // Update is applied on top of previous snapshots.
    assert_int_equal(actual->bids_length, 25);
    assert_int_equal(actual->asks_length, 24);
    assert_float_equal(actual->asks[0].price, 10198, 0.01);
    assert_float_equal(actual->asks[0].amount, 0.001, 0.01);
    assert_float_equal(actual->asks[23].price, 10222, 0.01);
    assert_float_equal(actual->asks[23].amount, 5.95598538, 0.01);

    free(snapshot_json);
//    order_book_delete(actual);
}


static void
test_update_sequence_bids() {

    char *snapshot_json = read_file("resources/bitfinex-snapshot-20200629.json", 1000);
    OrderBookLevel2 bookLevel2 = {
            .exchange = "bitfinex",
            .market_name = "btc/usd",
            .bids_length = 0,
            .asks_length = 0,
            .bids = {},
            .asks = {},
    };

    // Subscribed.
    bitfinex_parse_depth_update(
            " {\"event\":\"subscribed\",\"channel\":\"book\",\"chanId\":482136,\"symbol\":\"tBTCUSD\",\"prec\":\"P0\",\"freq\":\"F0\",\"len\":\"25\",\"pair\":\"BTCUSD\"}",
            &bookLevel2);

    // Check snapshot.
    OrderBookLevel2 *initial = bitfinex_parse_depth_update(snapshot_json, &bookLevel2);
    order_book_calculate_mid(initial);
    bitfinex_parse_depth_update("[482136,[9114.3,0,1]]", &bookLevel2);
    bitfinex_parse_depth_update("[482136,[9114.4,2,0.06701262]]", &bookLevel2);
    OrderBookLevel2 *actual = bitfinex_parse_depth_update("[482136,[9110.8,1,0.32927]]", &bookLevel2);
    assert_int_equal(actual->bids_length, 25);
    assert_float_equal(actual->bids[24].price, 9110.8, 0.01);

    bitfinex_parse_depth_update("[482136,[9111.5,0,1]]", &bookLevel2);
    assert_int_equal(actual->bids_length, 24);
    assert_float_equal(actual->bids[23].price, 9110.8, 0.01);
    bitfinex_parse_depth_update("[482136,[9121.6,0,-1]]", &bookLevel2);
    bitfinex_parse_depth_update("[482136,[9114.4,1,0.05349945]]", &bookLevel2);
    assert_int_equal(actual->bids_length, 24);
    assert_float_equal(actual->bids[23].price, 9110.8, 0.01);
    bitfinex_parse_depth_update("[482136,[9114.2,2,0.01831317]]", &bookLevel2);
    assert_int_equal(actual->bids_length, 24);
    assert_float_equal(actual->bids[23].price, 9110.8, 0.01);
    bitfinex_parse_depth_update("[482136,[9111.7,1,1.25]]", &bookLevel2);
    assert_int_equal(actual->bids_length, 25);
    assert_float_equal(actual->bids[24].price, 9110.8, 0.01);
    bitfinex_parse_depth_update("[482136,[9119.9,38,-0.63704329]]", &bookLevel2);
    bitfinex_parse_depth_update("[482136,[9120,9,-0.1699]]", &bookLevel2);
    bitfinex_parse_depth_update("[482136,[9120.3,34,-0.021343]]", &bookLevel2);
    bitfinex_parse_depth_update("[482136,[9120.4,1,-0.05349945]]", &bookLevel2);
    assert_int_equal(actual->bids_length, 25);
    assert_float_equal(actual->bids[24].price, 9110.8, 0.01);
    bitfinex_parse_depth_update("[482136,[9114.2,1,0.01351317]]", &bookLevel2);
    assert_int_equal(actual->bids_length, 25);
    assert_float_equal(actual->bids[24].price, 9110.8, 0.01);
    bitfinex_parse_depth_update("[482136,[9114.1,2,0.5048]]", &bookLevel2);
    assert_int_equal(actual->bids_length, 25);
    assert_float_equal(actual->bids[24].price, 9110.8, 0.01);
    bitfinex_parse_depth_update("[482136,[9114.2,1,0.01366145]]", &bookLevel2);
    assert_int_equal(actual->bids_length, 25);
    assert_float_equal(actual->bids[24].price, 9110.8, 0.01);
    bitfinex_parse_depth_update("[482136,[9119.9,38,-0.63719157]]", &bookLevel2);
    bitfinex_parse_depth_update("[482136,[9114.2,0,1]]", &bookLevel2);
    assert_int_equal(actual->bids_length, 24);
    assert_float_equal(actual->bids[23].price, 9110.8, 0.01);
    bitfinex_parse_depth_update("[482136,[9120.4,0,-1]]", &bookLevel2);
    assert_int_equal(actual->bids_length, 24);
    assert_float_equal(actual->bids[23].price, 9110.8, 0.01);
    bitfinex_parse_depth_update("[482136,[9114.4,2,0.0671609]]", &bookLevel2);
    assert_float_equal(actual->bids[23].price, 9110.8, 0.01);
    bitfinex_parse_depth_update("[482136,[9110.4,1,0.75]]", &bookLevel2);
    assert_int_equal(actual->bids_length, 25);
    assert_float_equal(actual->bids[23].price, 9110.8, 0.01);
    assert_float_equal(actual->bids[24].price, 9110.4, 0.01);
    bitfinex_parse_depth_update("[482136,[9120.3,35,-0.07484245]]", &bookLevel2);
    bitfinex_parse_depth_update("[482136,[9121.6,24,-0.01584]]", &bookLevel2);
    bitfinex_parse_depth_update("[482136,[9110.4,0,1]]", &bookLevel2);
    assert_int_equal(actual->bids_length, 24);
    assert_float_equal(actual->bids[23].price, 9110.8, 0.01);
    assert_float_equal(actual->bids[24].price, 0.0, 0.01);
    bitfinex_parse_depth_update("[482136,[9121.6,0,-1]]", &bookLevel2);
    bitfinex_parse_depth_update("[482136,[9114.4,1,0.05349945]]", &bookLevel2);
    assert_int_equal(actual->bids_length, 24);
    assert_float_equal(actual->bids[23].price, 9110.8, 0.01);
    assert_float_equal(actual->bids[24].price, 0.0, 0.01);
    bitfinex_parse_depth_update("[482136,[9114.2,1,0.01366145]]", &bookLevel2);
    assert_int_equal(actual->bids_length, 25);
    assert_float_equal(actual->bids[24].price, 9110.8, 0.01);
    assert_float_equal(actual->bids[25].price, 0.0, 0.01);
    bitfinex_parse_depth_update("[482136,[9120.3,34,-0.021343]]", &bookLevel2);
    bitfinex_parse_depth_update("[482136,[9120.4,1,-0.05349945]]", &bookLevel2);
    bitfinex_parse_depth_update("[482136,[9110.8,0,1]]", &bookLevel2);
    assert_int_equal(actual->bids_length, 24);
    assert_float_equal(actual->bids[23].price, 9110.9, 0.01);
    assert_float_equal(actual->bids[24].price, 0.0, 0.01);
    assert_float_equal(actual->bids[25].price, 0.0, 0.01);

    free(snapshot_json);
}

static void
test_update_sequence_for_crossed_vwap() {
    char *snapshot_json = read_file("resources/bitfinex-snapshot-20200629-2.json", 996);
    OrderBookLevel2 bookLevel2 = {
            .exchange = "bitfinex",
            .market_name = "btc/usd",
            .bids_length = 0,
            .asks_length = 0,
            .bids = {},
            .asks = {},
    };

    // Subscribed.
    bitfinex_parse_depth_update(
            " {\"event\":\"subscribed\",\"channel\":\"book\",\"chanId\":453593,\"symbol\":\"tBTCUSD\",\"prec\":\"P0\",\"freq\":\"F0\",\"len\":\"25\",\"pair\":\"BTCUSD\"}",
            &bookLevel2);

    // Check snapshot.
    OrderBookLevel2 *initial = bitfinex_parse_depth_update(snapshot_json, &bookLevel2);
    order_book_calculate_mid(initial);

    bitfinex_parse_depth_update("[453593,[9124.2,0,1]]", &bookLevel2);
    order_book_calculate_mid(initial);
    bitfinex_parse_depth_update("[453593,[9125.4,1,0.33601551]]", &bookLevel2);
    order_book_calculate_mid(initial);
    bitfinex_parse_depth_update("[453593,[9124.4,1,0.2]]", &bookLevel2);
    order_book_calculate_mid(initial);
    bitfinex_parse_depth_update("[453593,[9128.8,8,-5.91109863]]", &bookLevel2);
    order_book_calculate_mid(initial);
    bitfinex_parse_depth_update("[453593,[9130.5,1,-0.07440842]]", &bookLevel2);
    order_book_calculate_mid(initial);
    bitfinex_parse_depth_update("[453593,[9130.6,2,-0.65981443]]", &bookLevel2);
    order_book_calculate_mid(initial);
    bitfinex_parse_depth_update("[453593,[9124.3,0,1]]", &bookLevel2);
    order_book_calculate_mid(initial);
    bitfinex_parse_depth_update("[453593,[9123.8,0,1]]", &bookLevel2);
    order_book_calculate_mid(initial);
    bitfinex_parse_depth_update("[453593,[9130.2,0,-1]]", &bookLevel2);
    order_book_calculate_mid(initial);


    assert_float_equal(initial->mid, 9127.418, 0.001);
}

int
main() {
    int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;
    lws_set_log_level(logs, NULL);

    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_parse_info),
            cmocka_unit_test(test_parse_subbed),
            cmocka_unit_test(test_parse_depth_snapshot_btcusd),
            cmocka_unit_test(test_parse_depth_update_btcusd_inside),
            cmocka_unit_test(test_parse_depth_update_btcusd_inside_bids),
            cmocka_unit_test(test_remove_best_bid),
            cmocka_unit_test(test_remove_best_ask),
            cmocka_unit_test(test_mid),
            cmocka_unit_test(test_parse_depth_update_btcusd),
            cmocka_unit_test(test_update_sequence_bids),
            cmocka_unit_test(test_update_sequence_for_crossed_vwap),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
