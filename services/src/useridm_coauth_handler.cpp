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

#include "inner_event.h"
#include "useridm_hilog_wrapper.h"
#include "useridm_adapter.h"
#include "useridm_coauth_handler.h"
namespace OHOS {
namespace UserIAM {
namespace UserIDM {
UserIDMCoAuthHandler::UserIDMCoAuthHandler(CoAuthType type, const uint64_t challenge, const uint64_t scheduleId,
                                           const std::shared_ptr<UserIDMModule>& data,
                                           const sptr<IIDMCallback>& callback)
{
    USERIDM_HILOGD(MODULE_SERVICE, "UserIDMCoAuthHandler constructor enter");

    type_ = type;
    lastChallenge_ = challenge;
    lastScheduleId_ = scheduleId;
    dataCallback_ = data;
    innerCallback_ = callback;
    if (innerCallback_ == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "sorry: input callback is nullptr!");
    }

    // add death recipient start
    sptr<IRemoteObject::DeathRecipient> dr = new CoAuthCallbackDeathRecipient(this);
    if (!callback->AsObject()->AddDeathRecipient(dr)) {
        USERIDM_HILOGE(MODULE_SERVICE, "Failed to add death recipient CoAuthCallbackDeathRecipient");
    }
    USERIDM_HILOGI(MODULE_SERVICE, "add death recipient success!");
    // add death recipient end
}
int32_t UserIDMCoAuthHandler::OnFinishModify(uint32_t resultCode, std::vector<uint8_t> &scheduleToken,
                                             uint64_t& credentialId)
{
    if (innerCallback_ == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "sorry: innerCallback_ is nullptr!");
        return INVALID_PARAMETERS;
    }
    uint64_t challenge = 0;
    uint64_t scheduleId = 0;
    if (dataCallback_ == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "dataCallback error: no found!");
        return FAIL;
    }
    bool res = dataCallback_->CheckChallenge(challenge);
    if (!res) {
        USERIDM_HILOGE(MODULE_SERVICE, "check challenge num error: no challenge!");
        return FAIL;
    }

    res = dataCallback_->CheckScheduleIdIsActive(scheduleId); // check same scheduleId
    if (res && (scheduleId == lastScheduleId_)) {
        // if have: session is still alive
        // call TA info
        CredentialInfo credentialInfo;
        UserIDMAdapter::GetInstance().UpdateCredential(scheduleToken, credentialId, credentialInfo);

        // callback: as set prop info param
        std::shared_ptr<UserIDMSetPropHandler> setPropCallback =
                                                std::make_shared<UserIDMSetPropHandler>(PIN, challenge,
                                                                                        scheduleId,
                                                                                        credentialId,
                                                                                        dataCallback_,
                                                                                        innerCallback_);
            
        AuthResPool::AuthAttributes condition;
        condition.SetUint32Value(AuthAttributeType::AUTH_PROPERTY_MODE, 0);
        condition.SetUint64Value(AuthAttributeType::AUTH_CALLER_UID, 0);
        condition.SetUint64Value(AuthAttributeType::AUTH_CREDENTIAL_ID, credentialInfo.credentialId);
        condition.SetUint32Value(AuthAttributeType::AUTH_TYPE, credentialInfo.authType);
        condition.SetUint64Value(AuthAttributeType::AUTH_TEMPLATE_ID, credentialInfo.templateId);

        // Call the collaboration interface again to delete the template ID and password
        CoAuth::CoAuth::GetInstance().SetExecutorProp(condition, setPropCallback);
        return SUCCESS;
    } else {
        USERIDM_HILOGE(MODULE_SERVICE, "scheduleId wrong!");
        return FAIL;
    }
}

void UserIDMCoAuthHandler::OnFinish(uint32_t resultCode, std::vector<uint8_t> &scheduleToken)
{
    USERIDM_HILOGD(MODULE_SERVICE, "UserIDMCoAuthHandler OnFinish enter: %{public}d type_ is %{public}u",
                   resultCode, type_);
    if (innerCallback_ == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "sorry: innerCallback_ is nullptr!");
        return;
    }
    int32_t result = FAIL;
    uint64_t credentialId = 0;
    if (resultCode != SUCCESS) {
        dataCallback_->DeleteSessionId();
        RequestResult reqRet;
        reqRet.credentialId = credentialId;
        innerCallback_->OnResult(result, reqRet);
        return;
    }
    if ((ADD_PIN_CRED == type_) || (ADD_FACE_CRED == type_)) {
        uint64_t scheduleId = 0;
        bool res = dataCallback_->CheckScheduleIdIsActive(scheduleId); // check same scheduleId
        if (res && (scheduleId == lastScheduleId_)) {
            // if have: session is still alive
            // call TA info
            result = UserIDMAdapter::GetInstance().AddCredential(scheduleToken, credentialId);
            if (SUCCESS != result) {
                USERIDM_HILOGE(MODULE_SERVICE, "call TA info addCred failed!");
            }
        }
        // clean session data
        dataCallback_->DeleteSessionId();
        RequestResult reqRet;
        reqRet.credentialId = credentialId;
        innerCallback_->OnResult(result, reqRet);
    } else if (MODIFY_CRED == type_) {
        result = OnFinishModify(resultCode, scheduleToken, credentialId);
        // if not:  session has been canceled
        RequestResult reqRet;
        reqRet.credentialId = credentialId;
        innerCallback_->OnResult(result, reqRet);
        // clean session data
        dataCallback_->DeleteSessionId();
    } else {
        USERIDM_HILOGE(MODULE_SERVICE, "callback type error: %d!", type_);
    }
}

void UserIDMCoAuthHandler::OnAcquireInfo(uint32_t acquire)
{
    USERIDM_HILOGD(MODULE_SERVICE, "UserIDMCoAuthHandler OnAcquireInfo enter");
    if (innerCallback_ == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "sorry: innerCallback_ is nullptr!");
        return;
    }

    if (ADD_FACE_CRED == type_) {
        RequestResult reqRet;
        reqRet.credentialId = 0;
        innerCallback_->OnResult(acquire, reqRet); // Prompt information is returned through callback
    }
}

// add cred death recipient
UserIDMCoAuthHandler::CoAuthCallbackDeathRecipient::CoAuthCallbackDeathRecipient(UserIDMCoAuthHandler* parent)
    : parent_(parent)
{
}

void UserIDMCoAuthHandler::CoAuthCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    if (remote == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "AddCredCallback OnRemoteDied failed, remote is nullptr");
        return;
    }

    if (parent_ != nullptr) {
        parent_->innerCallback_ = nullptr;
    }
    USERIDM_HILOGI(MODULE_SERVICE, "CoAuthCallbackDeathRecipient: normal notice: no more hode the callback.");
}
}  // namespace UserIDM
}  // namespace UserIAM
}  // namespace OHOS
