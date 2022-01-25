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
#include <string>
#include <sstream>
#include <iostream>
#include <securec.h>
#include <iremote_broker.h>
#include "callback.h"
#include "auth_common.h"
#include "hilog_wrapper.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "useridentity_manager.h"

const size_t ARGC = 2;
namespace OHOS {
namespace UserIAM {
namespace UserIDM {
UserIdentityManager::UserIdentityManager()
{
}

UserIdentityManager::~UserIdentityManager()
{
}
/**
 * @brief NAPI method : openSession.
 *
 * @param env
 * @param info
 *
 * @return
 */
napi_value UserIdentityManager::NAPI_OpenSession(napi_env env, napi_callback_info info)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    AsyncOpenSession *asyncInfo = OCreateAsyncInfo(env);
    if (asyncInfo == nullptr) {
        return nullptr;
    }
    napi_value ret = OpenSessionWrap(env, info, asyncInfo);
    if (ret == nullptr) {
        if (asyncInfo != nullptr) {
            delete asyncInfo;
            asyncInfo = nullptr;
        }
    }
    return ret;
}

napi_value UserIdentityManager::OpenSessionWrap(napi_env env, napi_callback_info info, AsyncOpenSession *asyncInfo)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    if (asyncInfo == nullptr) {
        return nullptr;
    }
    size_t argc = ARGS_MAX_COUNT;
    size_t callbackIndex = ZERO_PARAMETER;
    napi_value argv[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    if (argc == 0) {
        NAPI_CALL(env, napi_create_promise(env, &asyncInfo->deferred, &asyncInfo->promise));
        ret = OpenSessionPromise(env, argv, argc, asyncInfo);
    } else {
        callbackIndex = argc - 1;
        NAPI_CALL(env, napi_typeof(env, argv[callbackIndex], &valuetype));
        if (valuetype == napi_function) {
            NAPI_CALL(env, napi_create_reference(env, argv[callbackIndex], 1, &asyncInfo->callback));
            ret = OpenSessionCallback(env, argv, argc, asyncInfo);
        } else {
            NAPI_CALL(env, napi_create_promise(env, &asyncInfo->deferred, &asyncInfo->promise));
            ret = OpenSessionPromise(env, argv, argc, asyncInfo);
        }
    }
    return ret;
}

napi_value OpenSessionRet(napi_env env, AsyncOpenSession* asyncOpenSession)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    size_t length = sizeof(asyncOpenSession->OpenSession);
    void* data = nullptr;
    napi_value arrayBuffer = nullptr;
    size_t bufferSize = length;
    NAPI_CALL(env, napi_create_arraybuffer(env, bufferSize, &data, &arrayBuffer));
    memcpy_s(data, bufferSize, reinterpret_cast<const void*>(&asyncOpenSession->OpenSession), bufferSize);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_typedarray(env, napi_uint8_array, bufferSize, arrayBuffer, 0, &result));
    return result;
}


napi_value UserIdentityManager::OpenSessionCallback(napi_env env, napi_value *argv, size_t argc,
                                                    AsyncOpenSession *asyncInfo)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    if (argv == nullptr || asyncInfo == nullptr) {
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resourceName,
        [](napi_env env, void *data) {
            AsyncOpenSession *asyncInfo = (AsyncOpenSession*)data;
            asyncInfo->OpenSession = UserIDMClient::GetInstance().OpenSession();
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncOpenSession *asyncInfo = (AsyncOpenSession*)data;
            napi_value result[1] = {0};
            napi_value callback_;
            napi_value undefined;
            napi_value callResult = ZERO_PARAMETER;
            result[0] = OpenSessionRet(env, asyncInfo);
            if (result[0] == nullptr) {
                HILOG_ERROR("translate uint64 to uint8Array faild");
            }
            napi_get_undefined(env, &undefined);
            napi_get_reference_value(env, asyncInfo->callback, &callback_);
            napi_call_function(env, undefined, callback_, 1, result, &callResult);
            napi_delete_async_work(env, asyncInfo->asyncWork);
            delete asyncInfo;
            asyncInfo = nullptr;
        },
        (void *)asyncInfo, &asyncInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncInfo->asyncWork));
    napi_value result = RESULT;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

napi_value UserIdentityManager::OpenSessionPromise(napi_env env, napi_value *argv, size_t argc,
                                                   AsyncOpenSession *asyncInfo)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    if (asyncInfo == nullptr) {
        return nullptr;
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resourceName,
        [](napi_env env, void *data) {
            AsyncOpenSession *asyncInfo = (AsyncOpenSession*)data;
            asyncInfo->OpenSession = UserIDMClient::GetInstance().OpenSession();
        },
        [](napi_env env, napi_status status, void *data) {
           AsyncOpenSession *asyncInfo = (AsyncOpenSession*)data;
            napi_value RetPromise = nullptr;
            RetPromise = OpenSessionRet(env, asyncInfo);
            if (RetPromise == nullptr) {
                HILOG_ERROR("translate uint64 to uint8Array faild");
            }
            napi_resolve_deferred(asyncInfo->env, asyncInfo->deferred, RetPromise);
            napi_delete_async_work(env, asyncInfo->asyncWork);
            delete asyncInfo;
            asyncInfo = nullptr;
        },
        (void *)asyncInfo, &asyncInfo->asyncWork));
    NAPI_CALL(env,  napi_queue_async_work(env, asyncInfo->asyncWork));
    return asyncInfo->promise;
}

void UserIdentityManager::AddCredentialExecute(napi_env env, void *data)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    AsyncHolder *asyncHolder = reinterpret_cast<AsyncHolder *>(data);
    if (asyncHolder == nullptr) {
        HILOG_ERROR("%{public}s asyncHolder nullptr", __func__);
        return;
    }
    AsyncCallbackContext *asyncCallbackContext = reinterpret_cast<AsyncCallbackContext *>(asyncHolder->data);
    if (asyncCallbackContext == nullptr) {
        HILOG_ERROR("%{public}s asyncCallbackContext nullptr", __func__);
        return;
    }
    AddCredInfo addCredInfo;
    addCredInfo.authType = asyncCallbackContext->authType;
    addCredInfo.authSubType = asyncCallbackContext->authSubType;
    addCredInfo.token.assign(asyncCallbackContext->token.begin(), asyncCallbackContext->token.end());
    std::shared_ptr<IDMCallback> iidmCallback = std::make_shared<IIdmCallback>(asyncCallbackContext);
    UserIDMClient::GetInstance().AddCredential(addCredInfo, iidmCallback);
}

void UserIdentityManager::AddCredentialComplete(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    AsyncHolder *asyncHolder = reinterpret_cast<AsyncHolder *>(data);
    if (asyncHolder == nullptr) {
        HILOG_ERROR("%{public}s asyncHolder nullptr", __func__);
        return;
    }
    napi_delete_async_work(env, asyncHolder->asyncWork);
    delete asyncHolder;
}

napi_value UserIdentityManager::BuildAddCredentialInfo(napi_env env, napi_callback_info info, AsyncHolder *asyncHolder)
{
    AsyncCallbackContext *asyncCallbackContext = reinterpret_cast<AsyncCallbackContext *>(asyncHolder->data);
    if (AuthCommon::JudgeObjectType(env, info, asyncCallbackContext) != napi_ok) {
        HILOG_ERROR("NAPI_AddCredential JudgeObjectType fail");
        return nullptr;
    }
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, AddCredentialExecute, AddCredentialComplete,
        (void *)asyncHolder, &asyncHolder->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncHolder->asyncWork));
    return result;
}

/**
 * @brief NAPI method : addCredential.
 *
 * @param env
 * @param info
 *
 * @return
 */
napi_value UserIdentityManager::NAPI_AddCredential(napi_env env, napi_callback_info info)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    AsyncHolder *asyncHolder = new (std::nothrow) AsyncHolder();
    if (asyncHolder == nullptr) {
        HILOG_ERROR("NAPI_AddCredential asyncHolder nullptr");
        return nullptr;
    }
    AsyncCallbackContext *asyncCallbackContext = new (std::nothrow) AsyncCallbackContext();
    if (asyncCallbackContext == nullptr) {
        HILOG_ERROR("NAPI_AddCredential asyncCallbackContext nullptr");
        delete asyncHolder;
        return nullptr;
    }
    asyncHolder->data = asyncCallbackContext;
    asyncCallbackContext->env = env;
    napi_value ret = BuildAddCredentialInfo(env, info, asyncHolder);
    if (ret == nullptr) {
        HILOG_ERROR("NAPI_AddCredential BuildAddCredentialInfo fail");
        delete asyncCallbackContext;
        delete asyncHolder;
        if (asyncHolder->asyncWork != nullptr) {
            napi_delete_async_work(env, asyncHolder->asyncWork);
        }
    }
    return ret;
}

void UserIdentityManager::UpdateCredentialExecute(napi_env env, void *data)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    AsyncHolder *asyncHolder = reinterpret_cast<AsyncHolder *>(data);
    if (asyncHolder == nullptr) {
        HILOG_ERROR("%{public}s asyncHolder nullptr", __func__);
        return;
    }
    AsyncCallbackContext *asyncCallbackContext = reinterpret_cast<AsyncCallbackContext *>(asyncHolder->data);
    if (asyncCallbackContext == nullptr) {
        HILOG_ERROR("%{public}s asyncCallbackContext nullptr", __func__);
        return;
    }
    AddCredInfo addCredInfo;
    addCredInfo.authType = asyncCallbackContext->authType;
    addCredInfo.authSubType = asyncCallbackContext->authSubType;
    addCredInfo.token.assign(asyncCallbackContext->token.begin(), asyncCallbackContext->token.end());
    std::shared_ptr<IDMCallback> iidmCallback = std::make_shared<IIdmCallback>(asyncCallbackContext);
    UserIDMClient::GetInstance().UpdateCredential(addCredInfo, iidmCallback);
}

void UserIdentityManager::UpdateCredentialComplete(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    AsyncHolder *asyncHolder = reinterpret_cast<AsyncHolder *>(data);
    if (asyncHolder == nullptr) {
        HILOG_ERROR("%{public}s asyncHolder nullptr", __func__);
        return;
    }
    napi_delete_async_work(env, asyncHolder->asyncWork);
    delete asyncHolder;
}

napi_value UserIdentityManager::BuildUpdateCredentialInfo(
    napi_env env, napi_callback_info info, AsyncHolder *asyncHolder)
{
    AsyncCallbackContext *asyncCallbackContext = reinterpret_cast<AsyncCallbackContext *>(asyncHolder->data);
    if (AuthCommon::JudgeObjectType(env, info, asyncCallbackContext) != napi_ok) {
        HILOG_ERROR("BuildUpdateCredentialInfo JudgeObjectType fail");
        return nullptr;
    }
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resourceName, UpdateCredentialExecute, UpdateCredentialComplete,
        (void *)asyncHolder, &asyncHolder->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncHolder->asyncWork));
    return result;
}

/**
 * @brief NAPI method : UpdateCredential.
 *
 * @param env
 * @param info
 *
 * @return
 */
napi_value UserIdentityManager::NAPI_UpdateCredential(napi_env env, napi_callback_info info)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    AsyncHolder *asyncHolder = new (std::nothrow) AsyncHolder();
    if (asyncHolder == nullptr) {
        HILOG_ERROR("NAPI_AddCredential asyncHolder nullptr");
        return nullptr;
    }
    AsyncCallbackContext *asyncCallbackContext = new (std::nothrow) AsyncCallbackContext();
    if (asyncCallbackContext == nullptr) {
        HILOG_ERROR("NAPI_AddCredential asyncCallbackContext nullptr");
        delete asyncHolder;
        return nullptr;
    }
    asyncHolder->data = asyncCallbackContext;
    asyncCallbackContext->env = env;
    napi_value ret = BuildUpdateCredentialInfo(env, info, asyncHolder);
    if (ret == nullptr) {
        HILOG_ERROR("NAPI_AddCredential BuildAddCredentialInfo fail");
        delete asyncCallbackContext;
        delete asyncHolder;
        if (asyncHolder->asyncWork != nullptr) {
            napi_delete_async_work(env, asyncHolder->asyncWork);
        }
    }
    return ret;
}

/**
 * @brief NAPI method : closeSession.
 *
 * @param env
 * @param info
 *
 * @return
 */
napi_value UserIdentityManager::NAPI_CloseSession(napi_env env, napi_callback_info info)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    napi_value result = nullptr;
    int32_t RetCloseSession = 0;
    UserIDMClient::GetInstance().CloseSession();
    NAPI_CALL(env, napi_create_int32(env, RetCloseSession, &result));
    return result;
}

/**
 * @brief NAPI method : Cancle.
 *
 * @param env
 * @param info
 *
 * @return
 */
napi_value UserIdentityManager::NAPI_Cancel(napi_env env, napi_callback_info info)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    int32_t ret = 0;
    napi_value result = nullptr;
    size_t argc = ONE_PARAMETER;
    napi_value argv[ONE_PARAMETER] = {0};
    SyncCancelContext *syncCancelContext = new (std::nothrow) SyncCancelContext();
    syncCancelContext->env = env;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    syncCancelContext->challenge = AuthCommon::JudgeArryType(env, ZERO_PARAMETER, argv);
    uint8_t tmp[sizeof(uint64_t)];
    for (uint32_t i = 0; i < sizeof(uint64_t); i++) {
        tmp[i] = syncCancelContext->challenge[i];
    }
    uint64_t *re = static_cast<uint64_t *>(static_cast<void *>(tmp));
    ret = UserIDMClient::GetInstance().Cancel(*re);
    NAPI_CALL(env, napi_create_int32(env, ret, &result));
    delete syncCancelContext;
    syncCancelContext = nullptr;
    return result;
}

void UserIdentityManager::DelUserExecute(napi_env env, void *data)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    AsyncHolder *asyncHolder = reinterpret_cast<AsyncHolder *>(data);
    if (asyncHolder == nullptr) {
        HILOG_ERROR("%{public}s asyncHolder nullptr", __func__);
        return;
    }
    AsyncCallbackContext *asyncCallbackContext = reinterpret_cast<AsyncCallbackContext *>(asyncHolder->data);
    if (asyncCallbackContext == nullptr) {
        HILOG_ERROR("%{public}s asyncCallbackContext nullptr", __func__);
        return;
    }
    std::shared_ptr<IDMCallback> iidmCallback = std::make_shared<IIdmCallback>(asyncCallbackContext);
    UserIDMClient::GetInstance().DelUser(asyncCallbackContext->token, iidmCallback);
}

void UserIdentityManager::DelUserComplete(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    AsyncHolder *asyncHolder = reinterpret_cast<AsyncHolder *>(data);
    if (asyncHolder == nullptr) {
        HILOG_ERROR("%{public}s asyncHolder nullptr", __func__);
        return;
    }
    napi_delete_async_work(env, asyncHolder->asyncWork);
    delete asyncHolder;
}

napi_value UserIdentityManager::DoDelUser(napi_env env, napi_callback_info info, AsyncHolder *asyncHolder)
{
    AsyncCallbackContext *asyncCallbackContext = reinterpret_cast<AsyncCallbackContext *>(asyncHolder->data);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    AuthCommon::JudgeDelUserType(env, info, asyncCallbackContext);
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, DelUserExecute, DelUserComplete,
                                          (void *)asyncHolder, &asyncHolder->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncHolder->asyncWork));
    return result;
}

/**
 * @brief NAPI method : delUser.
 *
 * @param env
 * @param info
 *
 * @return
 */
napi_value UserIdentityManager::NAPI_DelUser(napi_env env, napi_callback_info info)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    AsyncHolder *asyncHolder = new (std::nothrow) AsyncHolder();
    if (asyncHolder == nullptr) {
        HILOG_ERROR("NAPI_DelUser asyncHolder nullptr");
        return nullptr;
    }
    AsyncCallbackContext *asyncCallbackContext = new (std::nothrow) AsyncCallbackContext();
    if (asyncCallbackContext == nullptr) {
        HILOG_ERROR("NAPI_DelUser asyncCallbackContext nullptr");
        delete asyncHolder;
        return nullptr;
    }
    asyncHolder->data = asyncCallbackContext;
    asyncCallbackContext->env = env;
    napi_value ret = DoDelUser(env, info, asyncHolder);
    if (ret == nullptr) {
        HILOG_ERROR("NAPI_DelUser DoDelUser fail");
        delete asyncCallbackContext;
        delete asyncHolder;
        if (asyncHolder->asyncWork != nullptr) {
            napi_delete_async_work(env, asyncHolder->asyncWork);
        }
    }
    return ret;
}

void UserIdentityManager::DelCredExecute(napi_env env, void *data)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    AsyncHolder *asyncHolder = reinterpret_cast<AsyncHolder *>(data);
    if (asyncHolder == nullptr) {
        HILOG_ERROR("%{public}s asyncHolder nullptr", __func__);
        return;
    }
    AsyncCallbackContext *asyncCallbackContext = reinterpret_cast<AsyncCallbackContext *>(asyncHolder->data);
    if (asyncCallbackContext == nullptr) {
        HILOG_ERROR("%{public}s asyncCallbackContext nullptr", __func__);
        return;
    }
    uint8_t tmp[sizeof(uint64_t)];
    for (uint32_t i = 0; i < sizeof(uint64_t); i++) {
    tmp[i] = asyncCallbackContext->credentialId[i];
    }
    uint64_t *tempCredentialId = static_cast<uint64_t *>(static_cast<void *>(tmp));
    std::shared_ptr<IDMCallback> iidmCallback = std::make_shared<IIdmCallback>(asyncCallbackContext);
    UserIDMClient::GetInstance().DelCred(*tempCredentialId, asyncCallbackContext->token, iidmCallback);
}

void UserIdentityManager::DelCredComplete(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    AsyncHolder *asyncHolder = reinterpret_cast<AsyncHolder *>(data);
    if (asyncHolder == nullptr) {
        HILOG_ERROR("%{public}s asyncHolder nullptr", __func__);
        return;
    }
    napi_delete_async_work(env, asyncHolder->asyncWork);
    delete asyncHolder;
}

napi_value UserIdentityManager::DoDelCred(napi_env env, napi_callback_info info, AsyncHolder *asyncHolder)
{
    AsyncCallbackContext *asyncCallbackContext = reinterpret_cast<AsyncCallbackContext *>(asyncHolder->data);
    AuthCommon::JudgeDelCredType(env, info, asyncCallbackContext);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, DelCredExecute, DelCredComplete,
                                          (void *)asyncHolder, &asyncHolder->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncHolder->asyncWork));
    return result;
}

/**
 * @brief NAPI method : delCred.
 *
 * @param env
 * @param info
 *
 * @return
 */
napi_value UserIdentityManager::NAPI_DelCred(napi_env env, napi_callback_info info)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    AsyncHolder *asyncHolder = new (std::nothrow) AsyncHolder();
    if (asyncHolder == nullptr) {
        HILOG_ERROR("NAPI_DelCred asyncHolder nullptr");
        return nullptr;
    }
    AsyncCallbackContext *asyncCallbackContext = new (std::nothrow) AsyncCallbackContext();
    if (asyncCallbackContext == nullptr) {
        HILOG_ERROR("NAPI_DelCred asyncCallbackContext nullptr");
        delete asyncHolder;
        return nullptr;
    }
    asyncHolder->data = asyncCallbackContext;
    asyncCallbackContext->env = env;
    napi_value ret = DoDelCred(env, info, asyncHolder);
    if (ret == nullptr) {
        HILOG_ERROR("NAPI_DelCred DoDelUser fail");
        delete asyncCallbackContext;
        delete asyncHolder;
        if (asyncHolder->asyncWork != nullptr) {
            napi_delete_async_work(env, asyncHolder->asyncWork);
        }
    }
    return ret;
}

/**
 * @brief NAPI method : getAuthInfo.
 *
 * @param env
 * @param info
 *
 * @return
 */
napi_value UserIdentityManager::NAPI_GetAuthInfo(napi_env env, napi_callback_info info)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    AsyncHolder *asyncHolder = new (std::nothrow) AsyncHolder();
    if (asyncHolder == nullptr) {
        HILOG_ERROR("NAPI_DelCred asyncHolder nullptr");
        return nullptr;
    }
    AsyncGetAuthInfo *asyncGetAuthInfo = GCreateAsyncInfo(env);
    if (asyncGetAuthInfo == nullptr) {
        HILOG_ERROR("NAPI_DelCred asyncGetAuthInfo nullptr");
        delete asyncHolder;
        return nullptr;
    }
    asyncHolder->data = asyncGetAuthInfo;
    napi_value ret = GetAuthInfoWrap(env, info, asyncHolder);
    if (ret == nullptr) {
        HILOG_ERROR("NAPI_DelCred DoDelUser fail");
        delete asyncGetAuthInfo;
        delete asyncHolder;
        if (asyncHolder->asyncWork != nullptr) {
            napi_delete_async_work(env, asyncHolder->asyncWork);
        }
    }
    return ret;
}

napi_value UserIdentityManager::GetAuthInfoWrap(napi_env env, napi_callback_info info, AsyncHolder *asyncHolder)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    AsyncGetAuthInfo *asyncGetAuthInfo = reinterpret_cast<AsyncGetAuthInfo *>(asyncHolder->data);
    size_t argc = ARGS_MAX_COUNT;
    size_t callbackIndex = ZERO_PARAMETER;
    napi_value argv[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    if (argc == 0) {
        NAPI_CALL(env, napi_create_promise(env, &asyncGetAuthInfo->deferred, &asyncGetAuthInfo->promise));
        ret = GetAuthInfoPromise(env, argv, argc, asyncHolder);
    } else {
        callbackIndex = argc - 1;
        NAPI_CALL(env, napi_typeof(env, argv[callbackIndex], &valuetype));
        if (valuetype == napi_function) {
            NAPI_CALL(env, napi_create_reference(env, argv[callbackIndex], 1, &asyncGetAuthInfo->callback));
            ret = GetAuthInfoCallback(env, argv, argc, asyncHolder);
        } else {
            NAPI_CALL(env, napi_create_promise(env, &asyncGetAuthInfo->deferred, &asyncGetAuthInfo->promise));
            ret = GetAuthInfoPromise(env, argv, argc, asyncHolder);
        }
    }
    return ret;
}

void UserIdentityManager::GetAuthInfoExecute(napi_env env, void *data)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    AsyncHolder *asyncHolder = reinterpret_cast<AsyncHolder *>(data);
    if (asyncHolder == nullptr) {
        HILOG_ERROR("%{public}s asyncHolder nullptr", __func__);
        return;
    }
    AsyncGetAuthInfo *asyncGetAuthInfo = reinterpret_cast<AsyncGetAuthInfo *>(asyncHolder->data);
    if (asyncGetAuthInfo == nullptr) {
        HILOG_ERROR("%{public}s asyncCallbackContext nullptr", __func__);
        return;
    }
    std::shared_ptr<GetInfoCallback> getInfoCallbackIDM = std::make_shared<GetInfoCallbackIDM>(asyncGetAuthInfo);
    UserIDMClient::GetInstance().GetAuthInfo(asyncGetAuthInfo->authType, getInfoCallbackIDM);
}

void UserIdentityManager::GetAuthInfoComplete(napi_env env, napi_status status, void *data)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    AsyncHolder *asyncHolder = reinterpret_cast<AsyncHolder *>(data);
    if (asyncHolder == nullptr) {
        HILOG_ERROR("%{public}s asyncHolder nullptr", __func__);
        return;
    }
    napi_delete_async_work(env, asyncHolder->asyncWork);
    delete asyncHolder;
}

napi_value UserIdentityManager::GetAuthInfoCallback(napi_env env, napi_value *argv, size_t argc,
                                                    AsyncHolder *asyncHolder)
{
    HILOG_INFO("authFace : %{public}s, start  .", __func__);
    AsyncGetAuthInfo *asyncGetAuthInfo = reinterpret_cast<AsyncGetAuthInfo *>(asyncHolder->data);
    if (argc == ARGC) {
        int32_t authType_ = 0;
        NAPI_CALL(env, napi_get_value_int32(env, argv[0], &authType_));
        asyncGetAuthInfo->authType = static_cast<AuthType>(authType_);
    }
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_null(env, &result));
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resourceName, GetAuthInfoExecute, GetAuthInfoComplete,
        (void *)asyncHolder, &asyncHolder->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncHolder->asyncWork));
    return result;
}

napi_value UserIdentityManager::GetAuthInfoPromise(napi_env env, napi_value *argv, size_t argc,
                                                   AsyncHolder *asyncHolder)
{
    HILOG_INFO("authFace : %{public}s, start.", __func__);
    AsyncGetAuthInfo *asyncGetAuthInfo = reinterpret_cast<AsyncGetAuthInfo *>(asyncHolder->data);
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
    if (argv[0] != nullptr && valuetype == napi_number) {
        int32_t jsAuthType = 0;
        NAPI_CALL(env, napi_get_value_int32(env, argv[0], &jsAuthType));
        asyncGetAuthInfo->authType = static_cast<AuthType>(jsAuthType);
    } else {
        asyncGetAuthInfo->authType = static_cast<AuthType>(0);
    }
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(
        env, nullptr, resourceName,
        GetAuthInfoExecute, GetAuthInfoComplete,
        (void *)asyncHolder, &asyncHolder->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, asyncHolder->asyncWork));
    return asyncGetAuthInfo->promise;
}
} // namespace UserIDM
} // namespace UserIAM
} // namespace ohos
