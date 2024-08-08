#include "pch.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <core/rtc2_text_ini.h>
#include "GlobalConfig.h"
#include <iostream>

TEST_CASE("GlobalConfig OpenFile Test") {
    GlobalConfig config;
    CHECK(config.OpenFile("./data/user.ini"));
    int a = config["graph"]["fullscreen"].at<int>();
    CHECK(a == 0);
}
