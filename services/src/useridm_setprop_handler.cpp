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

#include "useridm_hilog_wrapper.h"
#include "useridm_setprop_handler.h"
namespace OHOS {
namespace UserIAM {
namespace UserIDM {
UserIDMSetPropHandler::UserIDMSetPropHandler(AuthType type, const uint64_t challenge,
                                             const uint64_t sessionId, uint64_t credentialId,
                                             const std::shared_ptr<UserIDMMoudle>& data,
                                             const sptr<IIDMCallback>& callback)
{
    USERIDM_HILOGD(MODULE_SERVICE, "UserIDMSetPropHandler constructor enter");

    type_ = type;
    lastChallenge_ = challenge;
    lastScheduleId_ = sessionId;
    lastCredentialId_ = credentialId;
    propDataCallback_ = data;
    propInnerCallback_ = callback;

    if (propInnerCallback_ == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "sorry: input callback is nullptr!");
    }

    // add death recipient start
    sptr<IRemoteObject::DeathRecipient> dr = new SetPropCallbackDeathRecipient(this);
    if (!callback->AsObject()->AddDeathRecipient(dr)) {
        USERIDM_HILOGE(MODULE_SERVICE, "Failed to add death recipient SetPropCallbackDeathRecipient");
    }
    USERIDM_HILOGI(MODULE_SERVICE, "add death recipient success!");
    // add death recipient end
}

void UserIDMSetPropHandler::OnResult(uint32_t result, std::vector<uint8_t> &extraInfo)
{
    USERIDM_HILOGD(MODULE_SERVICE, "UserIDMSetPropHandler OnResult enter: %{public}u, %{public}d", result, type_);

    if ((type_ == PIN) || (type_ == FACE)) {
        if (propInnerCallback_ != nullptr) {
            USERIDM_HILOGI(MODULE_SERVICE, "ready to call callback");
            RequestResult reqResult;
            reqResult.credentialId = lastCredentialId_;
            propInnerCallback_->OnResult(result, reqResult);
        } else {
            USERIDM_HILOGE(MODULE_SERVICE, "propInnerCallback_ is nullptr");
        }
        propDataCallback_->DeleteSessionId(); // no need to check session
    }
}

// add death recipient
UserIDMSetPropHandler::SetPropCallbackDeathRecipient::SetPropCallbackDeathRecipient(UserIDMSetPropHandler* parent)
    : parent_(parent)
{
}

void UserIDMSetPropHandler::SetPropCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    if (remote == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "AddCredCallback OnRemoteDied failed, remote is nullptr");
        return;
    }

    if (parent_ != nullptr) {
        parent_->propInnerCallback_ = nullptr;
    }
    USERIDM_HILOGI(MODULE_SERVICE, "SetPropCallbackDeathRecipient: normal notice: no more hode the callback.");
}
}  // namespace UserIDM
}  // namespace UserIAM
}  // namespace OHOS
