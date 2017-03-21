#include "backend.hpp"

void lib_init(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
    Backend::init(exports, module);
}

NODE_MODULE(Backend, lib_init)
