/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef IUSERIDM_H
#define IUSERIDM_H

#include "iuseridm_callback.h"

namespace OHOS {
namespace UserIAM {
namespace UserIDM {
class IUserIDM : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.useridm.IUserIDM");
    virtual uint64_t OpenSession() = 0;
    virtual void CloseSession() = 0;
    virtual int32_t GetAuthInfo(AuthType authType, const sptr<IGetInfoCallback>& callback) = 0;
    virtual int32_t GetAuthInfo(int32_t userId, AuthType authType, const sptr<IGetInfoCallback>& callback) = 0;
    virtual int32_t GetSecInfo(int32_t userId, const sptr<IGetSecInfoCallback>& callback) = 0;
    virtual void AddCredential(AddCredInfo& credInfo, const sptr<IIDMCallback>& callback) = 0;
    virtual void UpdateCredential(AddCredInfo& credInfo, const sptr<IIDMCallback>& callback) = 0;
    virtual int32_t Cancel(uint64_t challenge) = 0;
    virtual int32_t EnforceDelUser(int32_t userId, const sptr<IIDMCallback>& callback) = 0;
    virtual void DelUser(std::vector<uint8_t> authToken, const sptr<IIDMCallback>& callback) = 0;
    virtual void DelCred(uint64_t credentialId, std::vector<uint8_t> authToken, const sptr<IIDMCallback>& callback) = 0;

    enum {
        USERIDM_OPEN_SESSION = 0,
        USERIDM_CLOSE_SESSION,
        USERIDM_GET_AUTH_INFO,
        USERIDM_GET_AUTH_INFO_BY_ID,
        USERIDM_GET_SEC_INFO,
        USERIDM_ADD_CREDENTIAL,
        USERIDM_UPDATE_CREDENTIAL,
        USERIDM_CANCEL,
        USERIDM_ENFORCE_DELUSER,
        USERIDM_DELUSER,
        USERIDM_DELCRED,
    };
};
}  // namespace UserIDM
}  // namespace UserIAM
}  // namespace OHOS

#endif // IUSERIDM_H
