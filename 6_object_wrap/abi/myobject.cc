#include "myobject.h"

napi_persistent MyObject::constructor;

MyObject::MyObject(double value) : value_(value) {
}

MyObject::~MyObject() {
}

void MyObject::Destructor(void* nativeObject) {
  ((MyObject*) nativeObject)->~MyObject();
}

void MyObject::Init(napi_env env, napi_value exports) {
  napi_method_descriptor methods[] = {
    { GetValue, "napi_value" },
    { PlusOne, "plusOne" },
    { Multiply, "multiply" },
  };

  napi_value function = napi_create_constructor_for_wrap_with_methods(
          env, New, "MyObject", 3, methods);

  constructor = napi_create_persistent(env, function);

  napi_set_property(env, exports, napi_property_name(env, "MyObject"),
                        function);
}

void MyObject::New(napi_env env, napi_func_cb_info info) {
  if (napi_is_construct_call(env, info)) {
    // Invoked as constructor: `new MyObject(...)`
    napi_value args[1];
    napi_get_cb_args(env, info, args, 1);
    double value = 0;
    if (napi_get_undefined_(env) != args[0]) {
      value = napi_get_number_from_value(env, args[0]);
    }
    MyObject* obj = new MyObject(value);
    napi_value jsthis = napi_get_cb_this(env, info);
    napi_wrap(env, jsthis, (void*) obj, MyObject::Destructor, nullptr);
    napi_set_return_value(env, info, jsthis);
  } else {
    // Invoked as plain function `MyObject(...)`, turn into construct call.
    napi_value args[1];
    napi_get_cb_args(env, info, args, 1);
    const int argc = 1;
    napi_value argv[argc] = { args[0] };
    napi_value cons = napi_get_persistent_value(env, constructor);
    napi_set_return_value(env, info, napi_new_instance(env, cons, argc, argv));
  }
}

void MyObject::GetValue(napi_env env, napi_func_cb_info info) {
  MyObject* obj = (MyObject*) napi_unwrap(env, napi_get_cb_this(env, info));
  napi_set_return_value(env, info, napi_create_number(env, obj->value_));
}

void MyObject::PlusOne(napi_env env, napi_func_cb_info info) {
  MyObject* obj = (MyObject*) napi_unwrap(env, napi_get_cb_this(env, info));
  obj->value_ += 1;
  napi_set_return_value(env, info, napi_create_number(env, obj->value_));
}

void MyObject::Multiply(napi_env env, napi_func_cb_info info) {
  napi_value args[1];
  napi_get_cb_args(env, info, args, 1);

  double multiple = 1;
  if (napi_get_undefined_(env) != args[0]) {
    multiple = napi_get_number_from_value(env, args[0]);
  }

  MyObject* obj = (MyObject*) napi_unwrap(env, napi_get_cb_this(env, info));

  napi_value cons = napi_get_persistent_value(env, constructor);
  const int argc = 1;
  napi_value argv[argc] = { napi_create_number(env, obj->value_ * multiple) };
  napi_set_return_value(env, info, napi_new_instance(env, cons, argc, argv));
}
