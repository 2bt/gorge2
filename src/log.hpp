#pragma once

#ifdef ANDROID
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,  "FOOBAR", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN,  "FOOBAR", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "FOOBAR", __VA_ARGS__))
#else
#include <cstdio>
#define LOGI(fmt, ...) printf(            fmt "\n", ##__VA_ARGS__)
#define LOGW(fmt, ...) printf("warning: " fmt "\n", ##__VA_ARGS__)
#define LOGE(fmt, ...) printf("error: "   fmt "\n", ##__VA_ARGS__)
#endif
