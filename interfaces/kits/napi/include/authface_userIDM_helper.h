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
#ifndef AUTHFACE_USERIDM_HELPER_H
#define AUTHFACE_USERIDM_HELPER_H

#include "napi/native_api.h"
#include "napi/native_common.h"

namespace OHOS {
namespace UserIAM {
namespace UserIDM {
/**
 * @brief Napi initialization
 *
 * @param env
 * @param exports
 */
void AuthFaceInit(napi_env env, napi_value exports);

/**
 * @brief Get the Ctor object
 *
 * @param env
 * @return napi_value UserIdentityManager Instance
 */
napi_value GetCtor(napi_env env);

/**
 * @brief Construction method
 *
 * @param env
 * @param info
 * @return napi_value UserAuth Instance
 */
napi_value Constructor(napi_env env, napi_callback_info info);

/**
 * @brief Construction method
 *
 * @param env
 * @param info
 * @return napi_value UserAuth Instance
 */
napi_value UserIdentityManagerConstructor(napi_env env, napi_callback_info info);

/**
 * @brief UserIdentityManagerConstructor
 *
 * @param env
 * @param info
 * @return napi_value Instance
 */
napi_value OpenSession(napi_env env, napi_callback_info info);

/**
 * @brief OpenSession
 *
 * @param env
 * @param info
 * @return napi_value void
 */
napi_value AddCredential(napi_env env, napi_callback_info info);

/**
 * @brief AddCredential
 *
 * @param env
 * @param info
 * @return napi_value callback Onresult onAcquireInfo
 */
napi_value UpdateCredential(napi_env env, napi_callback_info info);

/**
 * @brief UpdateCredential
 *
 * @param env
 * @param info
 * @return napi_value callback Onresult onAcquireInfo
 */
napi_value CloseSession(napi_env env, napi_callback_info info);

/**
 * @brief CloseSession
 *
 * @param env
 * @param info
 * @return napi_value void
 */
napi_value Cancel(napi_env env, napi_callback_info info);

/**
 * @brief Cancel
 *
 * @param env
 * @param info
 * @return napi_value void
 */
napi_value DelUser(napi_env env, napi_callback_info info);

/**
 * @brief DelUser
 *
 * @param env
 * @param info
 * @return napi_value callback Onresult onAcquireInfo
 */
napi_value DelCred(napi_env env, napi_callback_info info);

/**
 * @brief DelCred
 *
 * @param env
 * @param info
 * @return napi_value callback Onresult onAcquireInfo
 */
napi_value GetAuthInfo(napi_env env, napi_callback_info info);
} // namespace UserIDM
} // namespace UserIAM
} // namespace OHOS
#endif // AUTHFACE_USERIDM_HELPER_H
