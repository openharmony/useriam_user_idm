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

#include <if_system_ability_manager.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "useridm_callback_stub.h"
#include "useridm_getinfo_callback_stub.h"
#include "useridm_getsecinfo_callback_stub.h"
#include "useridm_hilog_wrapper.h"
#include "useridm_client.h"

namespace OHOS {
namespace UserIAM {
namespace UserIDM {
UserIDMClient::UserIDMClient() = default;
UserIDMClient::~UserIDMClient() = default;

sptr<IUserIDM> UserIDMClient::GetUserIDMProxy()
{
    USERIDM_HILOGI(MODULE_INNERKIT, " GetUserIDMProxy  enter");

    std::lock_guard<std::mutex> lock(mutex_);
    if (proxy_ != nullptr) {
        return proxy_;
    }

    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!sam) {
        USERIDM_HILOGE(MODULE_INNERKIT, "failed to get systemAbilityManager.");
        return nullptr;
    }

    sptr<IRemoteObject> obj = sam->CheckSystemAbility(SUBSYS_USERIAM_SYS_ABILITY_USERIDM);
    if (!obj) {
        USERIDM_HILOGE(MODULE_INNERKIT, "failed to get remoteObject.");
        return nullptr;
    }

    sptr<IRemoteObject::DeathRecipient> dr = new UserIDMDeathRecipient();
    if ((obj->IsProxyObject()) && (!obj->AddDeathRecipient(dr))) {
        USERIDM_HILOGE(MODULE_INNERKIT, "Failed to add death recipient.");
        return nullptr;
    }

    proxy_ = iface_cast<IUserIDM>(obj);
    deathRecipient_ = dr;

    USERIDM_HILOGE(MODULE_INNERKIT, "Succeed to connect distributed gallery manager service.");
    return proxy_;
}

void UserIDMClient::ResetUserIDMProxy(const wptr<IRemoteObject>& remote)
{
    USERIDM_HILOGI(MODULE_INNERKIT, " ResetUserIDMProxy  enter");

    std::lock_guard<std::mutex> lock(mutex_);
    if (!proxy_) {
        return;
    }

    auto serviceRemote = proxy_->AsObject();
    if ((serviceRemote != nullptr) && (serviceRemote == remote.promote())) {
        serviceRemote->RemoveDeathRecipient(deathRecipient_);
        proxy_ = nullptr;
    }
}

void UserIDMClient::UserIDMDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    USERIDM_HILOGI(MODULE_INNERKIT, " OnRemoteDied  enter");

    if (remote == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, "OnRemoteDied failed, remote is nullptr");
        return;
    }

    UserIDMClient::GetInstance().ResetUserIDMProxy(remote);
    USERIDM_HILOGE(MODULE_INNERKIT, "DistributedGalleryDeathRecipient::Recv death notice.");
}

uint64_t UserIDMClient::OpenSession()
{
    USERIDM_HILOGI(MODULE_INNERKIT, " OpenSession  enter");

    auto proxy = GetUserIDMProxy();
    if (proxy == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, " OpenSession proxy is nullptr");
        return FAIL;
    }

    uint64_t challenge = proxy->OpenSession();

    return challenge;
}

void UserIDMClient::CloseSession()
{
    USERIDM_HILOGI(MODULE_INNERKIT, " CloseSession  enter");

    auto proxy = GetUserIDMProxy();
    if (proxy == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, " CloseSession proxy is nullptr");
        return;
    }

    proxy->CloseSession();
}

int32_t UserIDMClient::GetAuthInfo(int32_t userId, AuthType authType, const std::shared_ptr<GetInfoCallback>& callback)
{
    USERIDM_HILOGI(MODULE_INNERKIT, " GetAuthInfoById  enter");

    auto proxy = GetUserIDMProxy();
    if (proxy == nullptr) {
        return FAIL;
    }

    sptr<IGetInfoCallback> callbackStub = new UserIDMGetInfoCallbackStub(callback);

    int32_t res = proxy->GetAuthInfo(userId, authType, callbackStub);

    return res;
}

int32_t UserIDMClient::GetAuthInfo(AuthType authType, const std::shared_ptr<GetInfoCallback>& napiCallback)
{
    USERIDM_HILOGI(MODULE_INNERKIT, " GetAuthInfo  enter");

    auto proxy = GetUserIDMProxy();
    if (proxy == nullptr) {
        return FAIL;
    }

    sptr<IGetInfoCallback> callbackStub = new UserIDMGetInfoCallbackStub(napiCallback);

    int32_t res = proxy->GetAuthInfo(authType, callbackStub);

    return res;
}

int32_t UserIDMClient::GetSecInfo(const std::shared_ptr<GetSecInfoCallback>& napiCallback)
{
    USERIDM_HILOGI(MODULE_INNERKIT, " GetSecInfo  enter");

    auto proxy = GetUserIDMProxy();
    if (proxy == nullptr) {
        return FAIL;
    }

    sptr<IGetSecInfoCallback> callbackStub= new UserIDMGetSecInfoCallbackStub(napiCallback);

    int32_t res = proxy->GetSecInfo(callbackStub);

    return res;
}

void UserIDMClient::AddCredential(AddCredInfo & credInfo, const std::shared_ptr<IDMCallback>& napiCallback)
{
    USERIDM_HILOGI(MODULE_INNERKIT, " AddCredential  enter");

    auto proxy = GetUserIDMProxy();
    if (proxy == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, " AddCredential proxy is nullptr");
        return;
    }

    sptr<IIDMCallback> callbackStub = new UserIDMCallbackStub(napiCallback);

    proxy->AddCredential(credInfo, callbackStub);
}

void UserIDMClient::UpdateCredential(AddCredInfo & credInfo, const std::shared_ptr<IDMCallback>& napiCallback)
{
    USERIDM_HILOGI(MODULE_INNERKIT, " UpdateCredential  enter");

    auto proxy = GetUserIDMProxy();
    if (proxy == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, " UpdateCredential proxy is nullptr");
        return;
    }

    sptr<IIDMCallback> callbackStub = new UserIDMCallbackStub(napiCallback);

    proxy->UpdateCredential(credInfo, callbackStub);
}

int32_t UserIDMClient::Cancel(uint64_t challenge)
{
    USERIDM_HILOGI(MODULE_INNERKIT, " Cancel  enter");

    auto proxy = GetUserIDMProxy();
    if (proxy == nullptr) {
        return FAIL;
    }

    int32_t res = proxy->Cancel(challenge);

    return res;
}

int32_t UserIDMClient::EnforceDelUser(int32_t userId, const std::shared_ptr<IDMCallback>& napiCallback)
{
    USERIDM_HILOGI(MODULE_INNERKIT, " EnforceDelUser  enter");

    auto proxy = GetUserIDMProxy();
    if (proxy == nullptr) {
        return FAIL;
    }

    sptr<IIDMCallback> callbackStub = new UserIDMCallbackStub(napiCallback);

    int32_t res = proxy->EnforceDelUser(userId, callbackStub);

    return res;
}

void UserIDMClient::DelUser(std::vector<uint8_t> authToken, const std::shared_ptr<IDMCallback>& napiCallback)
{
    USERIDM_HILOGI(MODULE_INNERKIT, " DelUser  enter");

    auto proxy = GetUserIDMProxy();
    if (proxy == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, " DelUser proxy is nullptr");
        return;
    }

    sptr<IIDMCallback> callbackStub = new UserIDMCallbackStub(napiCallback);

    proxy->DelUser(authToken, callbackStub);
}

void UserIDMClient::DelCred(uint64_t credentialId, std::vector<uint8_t> authToken,
                            const std::shared_ptr<IDMCallback>& napiCallback)
{
    USERIDM_HILOGI(MODULE_INNERKIT, " DelCred  enter");

    auto proxy = GetUserIDMProxy();
    if (proxy == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, " DelCred proxy is nullptr");
        return;
    }
    sptr<IIDMCallback> callbackStub = new UserIDMCallbackStub(napiCallback);
    proxy->DelCred(credentialId, authToken, callbackStub);
}
}  // namespace UserIDM
}  // namespace UserIAM
}  // namespace OHOS
