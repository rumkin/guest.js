#!/bin/bash

g++ -Os -I. -Isrc -Iinclude script.cc env.cc blank.cc src/plugins/ticker.cc src/plugins/net.cc lib/poe.a lib/libuv.a lib/cbor.a -o blank -std=c++11 -s -Wl,--start-group v8/obj.target/{tools/gyp/libv8_{base,libbase,snapshot,libplatform},third_party/icu/libicu{uc,i18n,data}}.a -Wl,--end-group -lrt -pthread