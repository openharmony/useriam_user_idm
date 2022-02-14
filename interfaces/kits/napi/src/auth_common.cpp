/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <iremote_broker.h>

#include "hilog_wrapper.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "useridentity_manager.h"
#include "auth_common.h"

namespace OHOS {
namespace UserIAM {
namespace UserIDM {
void AuthCommon::SaveCallback(napi_env env, size_t argc, napi_value* argv,
                              AsyncCallbackContext* asyncCallbackContext)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    napi_valuetype valueType;
    napi_status status = napi_typeof(env, argv[argc], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR("napi_typeof faild");
    }
    if (valueType == napi_object) {
        asyncCallbackContext->callbackInfo.env = env;
        status = napi_get_named_property(env, argv[argc], "onResult",
            &asyncCallbackContext->IdmCallOnResult);
        if (status != napi_ok) {
            HILOG_ERROR("napi_get_named_property faild");
        }
        status = napi_create_reference(
            env, asyncCallbackContext->IdmCallOnResult, 1, &asyncCallbackContext->callbackInfo.onResult);
        if (status != napi_ok) {
            HILOG_ERROR("napi_create_reference faild");
        }
        status = napi_get_named_property(
            env, argv[argc], "onAcquireInfo", &asyncCallbackContext->IdmCallonAcquireInfo);
        if (status != napi_ok) {
            HILOG_ERROR("napi_get_named_property faild");
        }
        status = napi_create_reference(
            env, asyncCallbackContext->IdmCallonAcquireInfo, 1, &asyncCallbackContext->callbackInfo.onAcquireInfo);
        if (status != napi_ok) {
            HILOG_ERROR("napi_create_reference faild");
        }
    } else {
        HILOG_INFO("type mismatch");
    }
}

int32_t AuthCommon::GetNamedProperty(napi_env env, napi_value obj, const std::string &keyStr)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    napi_value value = nullptr;
    napi_status status = napi_get_named_property(env, obj, keyStr.c_str(), &value);
    if (status != napi_ok) {
        HILOG_ERROR("napi_get_named_property faild");
    }
    uint32_t propLen;
    status = napi_get_value_uint32(env, value, &propLen);
    if (status != napi_ok) {
        HILOG_ERROR("napi_get_value_uint32 faild");
    }
    return propLen;
}

std::vector<uint8_t> AuthCommon::GetNamedAttribute(napi_env env, napi_value obj)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    std::vector<uint8_t>RetNull = {0};
    napi_value token;
    napi_status status = napi_get_named_property(env, obj, PROPERTY_KEY_EVENT.c_str(), &token);
    if (status != napi_ok) {
        HILOG_ERROR("napi_get_named_property faild");
    }
    napi_typedarray_type arraytype;
    size_t length = 0;
    napi_value buffer = nullptr;
    size_t offset = 0;
    uint8_t *data = nullptr;
    bool isTypedArray = false;
    napi_is_typedarray(env, token, &isTypedArray);
    if (isTypedArray) {
        HILOG_INFO("token is a array");
    } else {
        HILOG_INFO("token is not a array");
        return RetNull;
    }
    napi_get_typedarray_info(env, token, &arraytype, &length, reinterpret_cast<void **>(&data), &buffer, &offset);
    if (arraytype == napi_uint8_array) {
        HILOG_INFO("token is uint8 array ");
    } else {
        HILOG_ERROR("token is not a uint8 array ");
        return RetNull;
    }
    if (offset != 0) {
        HILOG_INFO("offset is %{public}d", offset);
        return RetNull;
    }
    std::vector<uint8_t> result(data, data + length);
    return result;
}

napi_value AuthCommon::CreateObject(napi_env env, const std::string &keyStr, uint64_t credentialId)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    napi_value obj;
    napi_value napiCredentialId = nullptr;
    NAPI_CALL(env, napi_create_object(env, &obj));
    if (keyStr.c_str() == FUNC_ONRESULT || keyStr.c_str() == FUNC_ONACQUIREINFO) {
        void* data = nullptr;
        napi_value arrayBuffer = nullptr;
        size_t length = sizeof(credentialId);
        size_t bufferSize = length;
        NAPI_CALL(env, napi_create_arraybuffer(env, bufferSize, &data, &arrayBuffer));
        memcpy_s(data, bufferSize, reinterpret_cast<const void*>(&credentialId), bufferSize);
        NAPI_CALL(env, napi_create_typedarray(env, napi_uint8_array, bufferSize, arrayBuffer, 0, &napiCredentialId));
    }
    NAPI_CALL(env, napi_set_named_property(env, obj, "credentialId", napiCredentialId));
    return obj;
}

napi_status AuthCommon::JudgeObjectType (
    napi_env env, napi_callback_info info, AsyncCallbackContext* asyncCallbackContext)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    size_t argc = TWO_PARAMETER;
    napi_value argv[TWO_PARAMETER] = {0};
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR("napi_get_cb_info faild");
        return status;
    }
    napi_valuetype valueType = napi_undefined;
    status = napi_typeof(env, argv[ZERO_PARAMETER], &valueType);
    if (status != napi_ok) {
        HILOG_ERROR("napi_typeof faild");
        return status;
    }
    if (valueType == napi_object) {
        asyncCallbackContext->authType = static_cast<AuthType>(GetNamedProperty(env, argv[0], PROPERTY_KEY_NAME));
        asyncCallbackContext->authSubType = static_cast<AuthSubType>(GetNamedProperty(env, argv[0], PROPERTY_KEY_ID));
        asyncCallbackContext->token = GetNamedAttribute(env, argv[0]);
        if (asyncCallbackContext->token.empty()) {
            HILOG_INFO("input invalid token");
            asyncCallbackContext->token.push_back(0);
        }
    }
    SaveCallback(env, ONE_PARAMETER, argv, asyncCallbackContext);
    return status;
}

void AuthCommon::JudgeDelUserType(napi_env env, napi_callback_info info, AsyncCallbackContext* asyncCallbackContext)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    napi_status status;
    size_t argc = TWO_PARAMETER;
    napi_value argv[TWO_PARAMETER] = {0};
    status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR("napi_get_cb_info faild");
    }
    asyncCallbackContext->token = JudgeArryType(env, ZERO_PARAMETER, argv);
    if (asyncCallbackContext->token.empty()) {
        HILOG_ERROR("JudgeArryType token faild");
    }
    SaveCallback(env, ONE_PARAMETER, argv, asyncCallbackContext);
}

void AuthCommon::JudgeDelCredType(napi_env env, napi_callback_info info, AsyncCallbackContext* asyncCallbackContext)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    napi_status status;
    size_t argc = THREE_PARAMETER;
    napi_value argv[THREE_PARAMETER] = {0};
    status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (status != napi_ok) {
        HILOG_ERROR("napi_get_cb_info faild");
    }
    asyncCallbackContext->token = JudgeArryType(env, ONE_PARAMETER, argv);
    if (asyncCallbackContext->token.empty()) {
        HILOG_ERROR("JudgeArryType token faild");
    }
    asyncCallbackContext->credentialId = JudgeArryType(env, ZERO_PARAMETER, argv);
    if (asyncCallbackContext->credentialId.empty()) {
        HILOG_ERROR("JudgeArryType credentialId faild");
    }
    SaveCallback(env, TWO_PARAMETER, argv, asyncCallbackContext);
}

std::vector<uint8_t> AuthCommon::JudgeArryType(napi_env env, size_t argc, napi_value *argv)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    std::vector<uint8_t>RetNull = {0};
    napi_typedarray_type arraytype;
    size_t length = 0;
    napi_value buffer = nullptr;
    size_t offset = 0;
    uint8_t *data = nullptr;
    bool isTypedArray = false;
    napi_is_typedarray(env, argv[argc], &isTypedArray);
    if (isTypedArray) {
        HILOG_INFO("this is a array");
    } else {
        HILOG_INFO("this is not a array");
        return RetNull;
    }
    napi_get_typedarray_info(env, argv[argc], &arraytype, &length, reinterpret_cast<void **>(&data), &buffer, &offset);
    if (arraytype == napi_uint8_array) {
        HILOG_INFO("this is a uint8 array ");
    } else {
        HILOG_ERROR("this is not a uint8 array ");
        return RetNull;
    }
    if (offset != 0) {
        HILOG_INFO(" offset is %{public}d", offset);
        return RetNull;
    }
    std::vector<uint8_t>result(data, data + length);
    return result;
}

AsyncGetAuthInfo *GCreateAsyncInfo(napi_env env)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    return new (std::nothrow) AsyncGetAuthInfo {
        .env = env,
        .deferred = nullptr,
    };
}

AsyncOpenSession *OCreateAsyncInfo(napi_env env)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    return new (std::nothrow) AsyncOpenSession {
        .env = env,
        .asyncWork = nullptr,
        .deferred = nullptr,
    };
}
} // UserIDM
} // USerIAM
} // namespace OHOS
