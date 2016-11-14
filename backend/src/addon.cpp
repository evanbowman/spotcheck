#include "backend.hpp"

void lib_init(v8::Local<v8::Object> exports) { backend::init(exports); }

NODE_MODULE(backend, lib_init)
