#include "backend.hpp"

void lib_init(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
    backend::init(exports, module);
}

NODE_MODULE(backend, lib_init)
