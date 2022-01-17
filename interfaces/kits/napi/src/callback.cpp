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
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "useridentity_manager.h"
#include "auth_common.h"
#include "hilog_wrapper.h"
#include "callback.h"
const int paramTwo = 2;
const int paramThree = 3;
namespace OHOS {
namespace UserIAM {
namespace UserIDM {
napi_value GetAuthInfoRet(napi_env env, uint64_t Ret)
{
    std::string RetCode = std::to_string(Ret);
    size_t length = RetCode.size();
    void* data = nullptr;
    napi_value arrayBuffer = nullptr;
    size_t bufferSize = length;
    napi_create_arraybuffer(env, bufferSize, &data, &arrayBuffer);
    memcpy_s(data, bufferSize, reinterpret_cast<const void*>(RetCode.c_str()), bufferSize);
    napi_value result = nullptr;
    napi_create_typedarray(env, napi_uint8_array, bufferSize, arrayBuffer, 0, &result);
    return result;
}

IIdmCallback::IIdmCallback(AsyncCallbackContext* asyncCallbackContext)
{
    std::lock_guard<std::mutex> idmMutexGuard(mutex_);
    asyncCallbackContext_ = asyncCallbackContext;
}
 
void IIdmCallback::OnResult(int32_t result, RequestResult extraInfo)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    std::lock_guard<std::mutex> idmMutexGuard(mutex_);
    if (asyncCallbackContext_ != nullptr) {
        napi_env env;
        napi_value global;
        napi_status status;
        env = asyncCallbackContext_->callbackInfo.env;
        napi_value callbackRef;
        napi_value callResult = 0;
        napi_value param[TWO_PARAMETER] = {0};
        status = napi_create_int32(env, result, &param[0]);
        if (status != napi_ok) {
            HILOG_ERROR("napi_create_int32 faild");
        }
        param[ONE_PARAMETER] = AuthCommon::CreateObject(env, FUNC_ONRESULT, extraInfo);
        status =napi_get_reference_value(env, asyncCallbackContext_->callbackInfo.onResult, &callbackRef);
        if (status != napi_ok) {
            HILOG_ERROR("napi_get_reference_value faild");
        }
        status =napi_get_global(env, &global);
        if (status != napi_ok) {
            HILOG_ERROR("napi_get_global faild");
        }
        status = napi_call_function(env, global, callbackRef, paramTwo, param, &callResult);
        if (status != napi_ok) {
            HILOG_ERROR("napi_call_function faild");
        }
        delete asyncCallbackContext_;
        asyncCallbackContext_ = nullptr;
    } else {
        HILOG_ERROR("asyncCallbackContext_ is nullptr");
    }
}

void IIdmCallback::OnAcquireInfo(int32_t module, int32_t acquire, RequestResult extraInfo)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    std::lock_guard<std::mutex> idmMutexGuard(mutex_);
    if (asyncCallbackContext_ != nullptr) {
        napi_env env;
        napi_status status;
        env = asyncCallbackContext_->callbackInfo.env;
        napi_value global;
        napi_value callbackRef;
        napi_value callResult;
        napi_value Sparam[THREE_PARAMETER] = {0};
        status = napi_create_int32(env, module, &Sparam[ZERO_PARAMETER]);
        if (status != napi_ok) {
            HILOG_ERROR("napi_create_int32 faild");
        }
        status = napi_create_int32(env, acquire, &Sparam[ONE_PARAMETER]);
        if (status != napi_ok) {
            HILOG_ERROR("napi_create_int32 faild");
        }
        Sparam[TWO_PARAMETER] = AuthCommon::CreateObject(env, FUNC_ONACQUIREINFO, extraInfo);
        status = napi_get_reference_value(env, asyncCallbackContext_->callbackInfo.onAcquireInfo, &callbackRef);
        if (status != napi_ok) {
            HILOG_ERROR("napi_get_reference_value faild");
        }
        napi_valuetype valuetype;
        status = napi_typeof(env, callbackRef, &valuetype);
        if (status != napi_ok) {
            HILOG_ERROR("napi_typeof faild");
        }
        status = napi_get_global(env, &global);
        if (status != napi_ok) {
            HILOG_ERROR("napi_get_global faild");
        }
        status = napi_call_function(env, global, callbackRef, paramThree, Sparam, &callResult);
        if (status != napi_ok) {
            HILOG_ERROR("napi_call_function faild");
        }
    } else {
        HILOG_ERROR("napi_call_function is nullptr");
    }
}

GetInfoCallbackIDM::GetInfoCallbackIDM(AsyncGetAuthInfo *asyncGetAuthInfo)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    asyncGetAuthInfo_ = asyncGetAuthInfo;
}

napi_value GetInfoCallbackIDM::createCredentialInfo (std::vector<CredentialInfo>& info) 
{
    napi_env env;
    napi_value Obj;
    napi_value Ary;
    napi_value credentialId_;
    napi_value authType_;
    napi_value authSubType_;
    napi_value templateId_;
    if (asyncGetAuthInfo_ != nullptr) {
        env = asyncGetAuthInfo_->env;
        NAPI_CALL(env, napi_create_array_with_length(env,info.size(),&Ary));
        for (uint64_t Vect = 0; Vect < info.size(); Vect ++ ) {
            NAPI_CALL(env, napi_create_object(env,&Obj));
            credentialId_ = GetAuthInfoRet(env,(info[Vect].credentialId));
            NAPI_CALL(env, napi_create_int32(env,static_cast<int32_t>(info[Vect].authType),&authType_));
            NAPI_CALL(env, napi_create_int32(env,static_cast<int32_t>(info[Vect].authSubType),&authSubType_));
            templateId_ = GetAuthInfoRet(env,(info[Vect].templateId));
            NAPI_CALL(env, napi_set_named_property(env,Obj,"credentialId",credentialId_));
            NAPI_CALL(env, napi_set_named_property(env,Obj,"authType",authType_));
            NAPI_CALL(env, napi_set_named_property(env,Obj,"authSubType",authSubType_));
            NAPI_CALL(env, napi_set_named_property(env,Obj,"templateId",templateId_));
            NAPI_CALL(env, napi_set_element(env,Ary,Vect,Obj));
        }
        return Ary;
    }
    return nullptr;
} 

void GetInfoCallbackIDM::OnGetInfo(std::vector<CredentialInfo>& info)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    if (asyncGetAuthInfo_ != nullptr) {
        napi_env env;
        napi_status status;
        env = asyncGetAuthInfo_->env;
        napi_value result[ONE_PARAMETER] = {0};
        if (asyncGetAuthInfo_->callback == nullptr) {
            napi_value ResPromise;
            result[ZERO_PARAMETER] = createCredentialInfo(info);
            ResPromise = result[ZERO_PARAMETER];
            status = napi_resolve_deferred(asyncGetAuthInfo_->env, asyncGetAuthInfo_->deferred, ResPromise);
            if (status != napi_ok) {
            HILOG_ERROR("napi_resolve_deferred faild");
        }
            delete asyncGetAuthInfo_;
            asyncGetAuthInfo_ = nullptr;
        } else {
        napi_value callback;
        napi_value global;
        napi_value callbackRet = 0;
        result[ZERO_PARAMETER] = createCredentialInfo(info);
        status = napi_get_reference_value(env,asyncGetAuthInfo_->callback,&callback);
        if (status != napi_ok) {
            HILOG_ERROR("napi_get_reference_value faild");
        }
        status = napi_get_global(env,&global);
        if (status != napi_ok) {
            HILOG_ERROR("napi_get_global faild");
        }
        status = napi_call_function(env,global,callback,1,result,&callbackRet); 
        if (status != napi_ok) {
            HILOG_ERROR("napi_call_function faild");
        }
        }
    } else {
        HILOG_ERROR("asyncGetAuthInfo_ is nullptr");
    }
}
} // namespace UserIDM
} // namespace UserIAM
} // namespace ohos