/*
* Copyright (c) 2017 Agora.io
* All rights reserved.
* Proprietary and Confidential -- Agora.io
*/

/*
*  Created by Wang Yongli, 2017
*/
#ifndef AGORA_NODE_NAPI_API_H
#define AGORA_NODE_NAPI_API_H

/**
 * The file defines facilities used to implement node ADDON. 
 */

#include <node.h>
#include <array>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <thread>
#include <memory>
#include <nan.h>
using Nan::Persistent;
using v8::Function;
using v8::Local;
using v8::FunctionTemplate;
using v8::FunctionCallback;
using v8::FunctionCallbackInfo;
using v8::Value;
using v8::Local;
using v8::Context;
using v8::Object;
using v8::String;
using v8::NewStringType;
using v8::Integer;
using v8::Isolate;
using v8::HandleScope;
using v8::Name;
#define NAPI_MODULE(name, fn) NODE_MODULE(name, fn)

/**
 * Node status
 */
typedef enum {
    napi_ok,
    napi_invalid_arg,
    napi_object_expected,
    napi_string_expected,
    napi_name_expected,
    napi_function_expected,
    napi_number_expected,
    napi_boolean_expected,
    napi_array_expected,
    napi_generic_failure,
    napi_pending_exception,
    napi_cancelled,
    napi_escape_called_twice,
    napi_handle_scope_mismatch
} napi_status;

struct buffer_info {
    unsigned char* buffer;
    uint32_t length;
};

using buffer_list = std::array<buffer_info, 4>;
using stream_buffer_type = std::vector<unsigned char>;

enum NodeRenderType
{
    NODE_RENDER_TYPE_LOCAL,
    NODE_RENDER_TYPE_REMOTE,
    NODE_RENDER_TYPE_DEVICE_TEST,
    NODE_RENDER_TYPE_VIDEO_SOURCE
};

#define MAX_MISS_COUNT 500

/**
 * NodeString is used to translate string from V8 value and vice versa in the same way as primitive types.
 */
class NodeString
{
public:
    NodeString()
        : p_mem(nullptr)
    {}
    NodeString(char* buf)
        : p_mem(buf)
    {}
    ~NodeString()
    {
        if (p_mem) {
            free_buf(p_mem);
            p_mem = nullptr;
        }
    }

    void setBuf(char* buf)
    {
        if (p_mem) {
            free_buf(p_mem);
            p_mem = nullptr;
        }
        p_mem = buf;
    }

    operator char* () {
        return p_mem;
    }

    static char* alloc_buf(size_t length)
    {
        return (char*)calloc(length, 1);
    }
    static void free_buf(char* buf)
    {
        free(buf);
    }
private:
    char *p_mem;
};

/** To declare class constructor, needed for classes to be exposed to JS layer. */
#define DECLARE_CLASS \
    static Persistent<Function> constructor
/** Define the class */
#define DEFINE_CLASS(name) \
    Persistent<Function> name::constructor

/**
 * used to define class that could be used directly in JS layer.
 */
#define BEGIN_PROPERTY_DEFINE(className, constructor, fieldCount) \
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, constructor); \
    tpl->SetClassName(Nan::New<v8::String>(#className).ToLocalChecked()); \
    tpl->InstanceTemplate()->SetInternalFieldCount(fieldCount);

/**
 * Add member functions that could be called in JS layer directly.
 */
// #define PROPERTY_METHOD_DEFINE(name) NODE_SET_PROTOTYPE_METHOD(tpl, #name, name);
#define PROPERTY_METHOD_DEFINE(name) Nan::SetPrototypeMethod(tpl, #name, name);

#define EN_PROPERTY_DEFINE() \
    constructor.Reset(tpl->GetFunction(context).ToLocalChecked());

#define NAPI_AUTO_LENGTH SIZE_MAX

/**
 * get the utf8 string from V8 value.
 */
int napi_get_value_string_utf8_(const Local<Value>& str, char *buffer, uint32_t len);

/**
 * get uint32 from V8 value.
 */
napi_status napi_get_value_uint32_(const Local<Value>& value, uint32_t& result);

/**
 * get bool from V8 value.
 */
napi_status napi_get_value_bool_(const Local<Value>& value, bool& result);

/**
 * get int32 from V8 value.
 */
napi_status napi_get_value_int32_(const Local<Value>& value, int32_t& result);

/**
 * get double from V8 value.
 */
napi_status napi_get_value_double_(const Local<Value>& value, double &result);


/**
 * get int64 from V8 value.
 */
napi_status napi_get_value_int64_(const Local<Value>& value, int64_t& result);

/**
 * get nodestring from V8 value.
 */
napi_status napi_get_value_nodestring_(const Local<Value>& str, NodeString& nodechar);

/**
 * get object from V8 value.
 */
napi_status napi_get_value_object_(Isolate* isolate, const Local<Value>& value, Local<Object>& object);

/**
 * Create V8 value from uint32
 */
Local<Value> napi_create_uint32_(Isolate *isolate, const uint32_t& value);

/**
 * Create V8 from bool
 */
Local<Value> napi_create_bool_(Isolate *isolate, const bool& value);

/**
 * Create V8 from string
 */
Local<Value> napi_create_string_(Isolate *isolate, const char* value);

/**
 * Create V8 value from double
 */
Local<Value> napi_create_double_(Isolate *isolate, const double &value);

/**
 * Create V8 value from uint64
 */
Local<Value> napi_create_uint64_(Isolate *isolate, const uint64_t& value);

/**
 * Create V8 value from int32
 */
Local<Value> napi_create_int32_(Isolate *isolate, const int32_t& value);

/**
 * Create V8 value from uint16
 */
Local<Value> napi_create_uint16_(Isolate *isolate, const uint16_t& value);

/**
* get uint32 property from V8 object.
*/
napi_status napi_get_object_property_uint32_(Isolate* isolate, const Local<Object>& obj, const std::string& propName, uint32_t& result);

/**
* get bool property from V8 object.
*/
napi_status napi_get_object_property_bool_(Isolate* isolate, const Local<Object>& obj, const std::string& propName, bool& result);

/**
* get int32 property from V8 object.
*/
napi_status napi_get_object_property_int32_(Isolate* isolate, const Local<Object>& obj, const std::string& propName, int32_t& result);

/**
* get double property from V8 object.
*/
napi_status napi_get_object_property_double_(Isolate* isolate, const Local<Object>& obj, const std::string& propName, double &result);


/**
* get int64 property from V8 object.
*/
napi_status napi_get_object_property_int64_(Isolate* isolate, const Local<Object>& obj, const std::string& propName, int64_t& result);

/**
* get nodestring property from V8 object.
*/
napi_status napi_get_object_property_nodestring_(Isolate* isolate, const Local<Object>& obj, const std::string& propName, NodeString& nodechar);


const char* nullable( char const* s);

#ifdef _WIN32
char* U2G(const char* srcstr);
#endif
#endif