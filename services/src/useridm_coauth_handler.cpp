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

#include "useridm_coauth_handler.h"
#include "useridm_hilog_wrapper.h"
#include "useridm_adapter.h"

namespace OHOS {
namespace UserIAM {
namespace UserIDM {
UserIDMCoAuthHandler::UserIDMCoAuthHandler(CoAuthType type, const uint64_t challenge, const uint64_t sessionId,
                                           const std::shared_ptr<UserIDMMoudle>& data,
                                           const sptr<IIDMCallback>& callback)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMCoAuthHandler constructor enter ");

    type_ = type;
    lastChallenge_ = challenge;
    lastSessionId_ = sessionId;
    dataCallback_ = data;
    innerCallback_ = callback;
}
void UserIDMCoAuthHandler::OnFinishModify(uint32_t resultCode, std::vector<uint8_t> &scheduleToken,
                                          uint64_t& credentialId, int32_t result)
{
    // success
    // check sessionId:
    // if have: session is still alive
    // if not:  session has been canceled
    uint64_t challenge = 0;
    uint64_t sessionId = 0;
    bool res = dataCallback_->CheckChallenge(challenge);
    if (!res) {
        // challenge miss return error, need openSession()
        USERIDM_HILOGE(MODULE_INNERKIT, "check challenge num error: no challenge!");
        return;
    }

    res = dataCallback_->CheckSessionId(sessionId);    // todo check same sessionId
    if (res && (sessionId == lastSessionId_)) {
        // if have: session is still alive
        // call TA info
        // UserIDMAdapter::GetInstance().AddCredential(scheduleToken, credentialId);
        CredentialInfo credentialInfo;
        UserIDMAdapter::GetInstance().UpdateCredential(scheduleToken, credentialId, credentialInfo);

        // callback: as set prop info param
        std::shared_ptr<UserIDMSetPropHandler> setPropCallback =
                                                std::make_shared<UserIDMSetPropHandler>(PIN, challenge,
                                                                                        sessionId,
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
    } else {
        USERIDM_HILOGE(MODULE_INNERKIT, "sessionId wrong!");
    }
}
void UserIDMCoAuthHandler::OnFinish(uint32_t resultCode, std::vector<uint8_t> &scheduleToken)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMCoAuthHandler OnFinish enter: %{public}d type_ is %{public}d",
                   resultCode, type_);

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
        uint64_t sessionId = 0;
        bool res = dataCallback_->CheckSessionId(sessionId);    // check same sessionId
        if (res && (sessionId == lastSessionId_)) {
            // if have: session is still alive
            // call TA info
            result = UserIDMAdapter::GetInstance().AddCredential(scheduleToken, credentialId);
            if (SUCCESS != result) {
                // current session in active
                USERIDM_HILOGE(MODULE_INNERKIT, "call TA info addCred failed!");
            }
        }
        // clean session data
        dataCallback_->DeleteSessionId();
        RequestResult reqRet;
        reqRet.credentialId = credentialId;
        innerCallback_->OnResult(result, reqRet);
    } else if (MODIFY_CRED == type_) {
        int32_t result = FAIL;
        OnFinishModify(resultCode, scheduleToken, credentialId, result);

        // if not:  session has been canceled
        RequestResult reqRet;
        reqRet.credentialId = credentialId;
        innerCallback_->OnResult(result, reqRet);
        // clean session data
        dataCallback_->DeleteSessionId();
        return;
    } else {
        // do nothing
        USERIDM_HILOGE(MODULE_INNERKIT, "callback type error: %d!", type_);
    }
}

void UserIDMCoAuthHandler::OnAcquireInfo(uint32_t acquire)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMCoAuthHandler OnAcquireInfo enter ");

    if (ADD_FACE_CRED == type_) {
        RequestResult reqRet;
        reqRet.credentialId = 0;
        innerCallback_->OnResult(acquire, reqRet);   // Prompt information is returned through callback
    }
}
}  // namespace UserIDM
}  // namespace UserIAM
}  // namespace OHOS
