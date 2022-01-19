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

#ifndef USERIDM_CONTROLLER_H
#define USERIDM_CONTROLLER_H

#include "useridm_info.h"
#include "useridm_coauth_handler.h"
#include "auth_info.h"
#include "co_auth.h"
#include "auth_attributes.h"

namespace OHOS {
namespace UserIAM {
namespace UserIDM {
class UserIDMController {
public:
    explicit UserIDMController();
    ~UserIDMController();

    void OpenEditSessionCtrl(int32_t userId, uint64_t &challenge);
    void CloseEditSessionCtrl();
    int32_t GetAuthInfoCtrl(int32_t userId, AuthType authType, std::vector<CredentialInfo>& credInfos);
    int32_t GetSecureInfoCtrl(int32_t userId, uint64_t& secureUid, std::vector<EnrolledInfo>& enroInfos);
    int32_t CheckEnrollPermissionCtrl(std::vector<uint8_t> autoToken, int32_t userId,
            AuthType authType, AuthSubType authSubType, uint64_t & sessionId);
    int32_t DeleteCredentialCtrl(int32_t userId, uint64_t credentialId,
            std::vector<uint8_t> authToken, CredentialInfo& credInfo);
    int32_t DeleteUserCtrl(int32_t userId, std::vector<uint8_t> authToken, std::vector<CredentialInfo>& credInfo);
    int32_t DeleteUserByForceCtrl(int32_t userId, std::vector<CredentialInfo>& credInfo);
    
    int32_t AddCredentialCtrl(AddCredInfo & credInfo, const sptr<IIDMCallback>& innerCallback); // napi callback
    void    AddCredentialCallCoauth(AddCredInfo& credInfo, const sptr<IIDMCallback>& innerkitsCallback,
                                    uint64_t& challenge, uint64_t& scheduleId, int32_t& userId);
    int32_t UpdateCredentialCtrl(AddCredInfo & credInfo, const sptr<IIDMCallback>& innerCallback);   // napi callback
    int32_t DelSchedleIdCtrl(uint64_t challenge);
    int32_t DelFaceCredentialCtrl(AuthType authType, AuthSubType authSubType,
            uint64_t credentialId, uint64_t templateId, const sptr<IIDMCallback>&innerCallback);    // napi callback
    int32_t DelExecutorPinInofCtrl(const sptr<IIDMCallback>& innerCallback, std::vector<CredentialInfo>& info);
    // napi callback

private:
    // add cred death recipient
    class AddCredCallbackDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        AddCredCallbackDeathRecipient(UserIDMController* parent);
        ~AddCredCallbackDeathRecipient() = default;
        void OnRemoteDied(const wptr<IRemoteObject>& remote) override;
    private:
        UserIDMController* parent_;
        DISALLOW_COPY_AND_MOVE(AddCredCallbackDeathRecipient);
    };

    // update cred death recipient
    class UpdateCredCallbackDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        UpdateCredCallbackDeathRecipient(UserIDMController* parent);
        ~UpdateCredCallbackDeathRecipient() = default;
        void OnRemoteDied(const wptr<IRemoteObject>& remote) override;
    private:
        UserIDMController* parent_;
        DISALLOW_COPY_AND_MOVE(UpdateCredCallbackDeathRecipient);
    };

    // delete face cred death recipient
    class DelCredCallbackDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        DelCredCallbackDeathRecipient(UserIDMController* parent);
        ~DelCredCallbackDeathRecipient() = default;
        void OnRemoteDied(const wptr<IRemoteObject>& remote) override;
    private:
        UserIDMController* parent_;
        DISALLOW_COPY_AND_MOVE(DelCredCallbackDeathRecipient);
    };

    // delete user death recipient
    class DelUserCallbackDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        DelUserCallbackDeathRecipient(UserIDMController* parent);
        ~DelUserCallbackDeathRecipient() = default;
        void OnRemoteDied(const wptr<IRemoteObject>& remote) override;
    private:
        UserIDMController* parent_;
        DISALLOW_COPY_AND_MOVE(DelUserCallbackDeathRecipient);
    };

private:
    sptr<IIDMCallback> addCredCallback_;
    sptr<IIDMCallback> updateCredCallback_;
    sptr<IIDMCallback> delUserCallback_;
    sptr<IIDMCallback> delCredCallback_;
    std::shared_ptr<UserIDMMoudle> data_;
};
}  // namespace UserIDM
}  // namespace UserIAM
}  // namespace OHOS

#endif // USERIDM_CONTROLLER_H
