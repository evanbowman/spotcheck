#pragma once

#include <functional>
#include <tuple>
#include <v8.h>
#include <uv.h>

using js_persist_cb = v8::Persistent<v8::Function>;
using js_cb = v8::Local<v8::Function>;

namespace detail {
	template <typename F, typename Tuple, bool Done, int Total, int... N>
	struct apply_impl {
		static void apply(F f, Tuple && t) {
			apply_impl<F,
                      Tuple,
                      Total == 1 + sizeof...(N),
                      Total, N..., sizeof...(N)>::apply(f,
                                                       std::forward<Tuple>(t));
		}
	};

	template <typename F, typename Tuple, int Total, int... N>
	struct apply_impl<F, Tuple, true, Total, N...> {
		static void apply(F f, Tuple && t) {
			f(std::get<N>(std::forward<Tuple>(t))...);
		}
	};
}

template <typename F, typename Tuple>
void apply(F f, Tuple && t) {
	typedef typename std::decay<Tuple>::type ttype;
	detail::apply_impl<F,
                      Tuple,
                      0 == std::tuple_size<ttype>::value,
                      std::tuple_size<ttype>::value>::
        apply(f, std::forward<Tuple>(t));
}

struct async {
    template <typename F, typename... Args>
    struct work {
        work(F && work_routine, Args && ...args) :
            m_work_routine(std::forward<F>(work_routine)),
            m_work_args(std::forward<Args>(args)...)
        {
            m_request.data = this;
        }
        uv_work_t m_request;
        js_persist_cb m_callback;
        F m_work_routine;
        std::tuple<Args...> m_work_args;
    };
    template <typename F, typename... Args>
    static void start(const js_cb & callback, F && work_routine, Args && ...args) {
        auto isolate = v8::Isolate::GetCurrent();
        auto task =
            new work<F, Args...>(std::forward<F>(work_routine), args...);
        task->m_callback.Reset(isolate, callback);
        uv_queue_work(uv_default_loop(), &task->m_request,
                      [](uv_work_t * req) {
                          auto task =
                              static_cast<work<F, Args...> *>(req->data);
                          apply(task->m_work_routine, task->m_work_args);
                      },
                      [](uv_work_t * req, int status) {
                          v8::Isolate * isolate = v8::Isolate::GetCurrent();
                          static const unsigned argc = 0;
                          v8::Handle<v8::Value> * argv = nullptr;
                          auto task = static_cast<work<F, Args...> *>(req->data);
                          v8::Local<v8::Function>::New(isolate, task->m_callback)->
                              Call(isolate->GetCurrentContext()->Global(),
                                   argc, argv);
                          task->m_callback.Reset();
                          delete task;
                      });
    }
};
