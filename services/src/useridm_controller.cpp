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

#include "useridm_callback_proxy.h"
#include "useridm_hilog_wrapper.h"
#include "useridm_adapter.h"
#include "inner_event.h"
#include "useridm_controller.h"
namespace {
    const int MIN_VECTOR_SIZE = 1;
}

namespace OHOS {
namespace UserIAM {
namespace UserIDM {
UserIDMController::UserIDMController()
{
    USERIDM_HILOGD(MODULE_SERVICE, "UserIDMController constructor");
    data_ = std::make_shared<UserIDMModule>();
}

UserIDMController::~UserIDMController()
{
    USERIDM_HILOGD(MODULE_SERVICE, "UserIDMController deconstructor");
}

void UserIDMController::OpenEditSessionCtrl(int32_t userId, uint64_t &challenge)
{
    USERIDM_HILOGD(MODULE_SERVICE, "OpenEditSessionCtrl enter");

    uint64_t sessionId = 0;
    uint64_t temp = 0;
    bool res = data_->CheckScheduleIdIsActive(sessionId);
    if (res && (data_->CheckChallenge(temp))) {
        // call coauth::cancel(): cancel current active session
        CoAuth::CoAuth::GetInstance().Cancel(sessionId);
    }
    data_->CleanData();

    // call TA info
    UserIDMAdapter::GetInstance().OpenEditSession(userId, challenge);

    data_->InsertChallenge(challenge); // add challenge num
}

void UserIDMController::CloseEditSessionCtrl()
{
    USERIDM_HILOGD(MODULE_SERVICE, "CloseEditSessionCtrl enter");

    uint64_t sessionId = 0;
    uint64_t temp = 0;
    bool res = data_->CheckScheduleIdIsActive(sessionId);
    if (res && (data_->CheckChallenge(temp))) {
        // call coauth::cancel(): cancel current active session
        CoAuth::CoAuth::GetInstance().Cancel(sessionId);
    }
    data_->CleanData();

    UserIDMAdapter::GetInstance().CloseEditSession();
}

int32_t UserIDMController::GetAuthInfoCtrl(int32_t userId, AuthType authType, std::vector<CredentialInfo>& credInfos)
{
    USERIDM_HILOGD(MODULE_SERVICE, "GetAuthInfoCtrl enter");
    return UserIDMAdapter::GetInstance().QueryCredential(userId, authType, credInfos);
}

int32_t UserIDMController::GetSecureInfoCtrl(int32_t userId, uint64_t& secureUid,
    std::vector<EnrolledInfo>& enrolledInfos)
{
    USERIDM_HILOGD(MODULE_SERVICE, "GetSecureInfoCtrl enter");
    return UserIDMAdapter::GetInstance().GetSecureUid(userId, secureUid, enrolledInfos);
}

int32_t UserIDMController::DeleteCredentialCtrl(int32_t userId, uint64_t credentialId, std::vector<uint8_t> authToken,
                                                CredentialInfo& credInfo)
{
    USERIDM_HILOGD(MODULE_SERVICE, "DeleteCredentialCtrl enter");
    return UserIDMAdapter::GetInstance().DeleteCredential(userId, credentialId, authToken, credInfo);
}

int32_t UserIDMController::DeleteUserCtrl(int32_t userId, std::vector<uint8_t> authToken,
                                          std::vector<CredentialInfo>& credInfo)
{
    USERIDM_HILOGD(MODULE_SERVICE, "DeleteUserCtrl enter");
    return UserIDMAdapter::GetInstance().DeleteUser(userId, authToken, credInfo);
}

int32_t UserIDMController::DeleteUserByForceCtrl(int32_t userId, std::vector<CredentialInfo>& credInfo)
{
    USERIDM_HILOGD(MODULE_SERVICE, "DeleteUserByForceCtrl enter");
    return UserIDMAdapter::GetInstance().DeleteUserEnforce(userId, credInfo);
}

int32_t UserIDMController::AddCredentialCallCoauth(uint64_t callerID, AddCredInfo& credInfo,
                                                   const sptr<IIDMCallback>& innerkitsCallback, uint64_t& challenge,
                                                   uint64_t& scheduleId)
{
    USERIDM_HILOGD(MODULE_SERVICE, "AddCredentialCallCoauth enter");
    if (innerkitsCallback == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "sorry: innerkitsCallback is nullptr!");
        return INVALID_PARAMETERS;
    }

    std::string callerName = std::to_string(callerID);
    data_->InsertScheduleId(scheduleId);
    // callback: as coauth info param
    CoAuth::AuthInfo paramInfo;
    paramInfo.SetPkgName(callerName);
    paramInfo.SetCallerUid(callerID);
    std::shared_ptr<UserIDMCoAuthHandler> coAuthCallback;

    USERIDM_HILOGE(MODULE_SERVICE, "credInfo.authType is [%{public}d]!", credInfo.authType);
    if (credInfo.authType == PIN) {
        coAuthCallback = std::make_shared<UserIDMCoAuthHandler>(ADD_PIN_CRED, challenge, scheduleId, data_,
                                                                innerkitsCallback);
    } else if (FACE == credInfo.authType) {
        coAuthCallback = std::make_shared<UserIDMCoAuthHandler>(ADD_FACE_CRED, challenge, scheduleId, data_,
                                                                innerkitsCallback);
    } else {
        USERIDM_HILOGE(MODULE_SERVICE, "credInfo.authType error: %{public}d!", credInfo.authType);
        coAuthCallback = std::make_shared<UserIDMCoAuthHandler>(ADD_PIN_CRED, challenge, scheduleId, data_,
                                                                innerkitsCallback);
    }

    if (coAuthCallback == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "sorry: coAuthCallback is nullptr!");
        return FAIL;
    } else {
        // call coauth
        CoAuth::CoAuth::GetInstance().BeginSchedule(scheduleId, paramInfo, coAuthCallback);
    }
    return SUCCESS;
}

int32_t UserIDMController::AddCredentialCtrl(int32_t userId, uint64_t callerID, AddCredInfo& credInfo,
                                             const sptr<IIDMCallback>& innerkitsCallback)
{
    USERIDM_HILOGD(MODULE_SERVICE, "AddCredentialCtrl enter");

    if (innerkitsCallback == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "sorry: innerkitsCallback is nullptr!");
        return INVALID_PARAMETERS;
    }
    uint64_t scheduleId = 0;
    uint64_t challenge = 0;

    bool result = data_->CheckChallenge(challenge);
    if (!result) {
        // challenge miss return error, need openSession()
        USERIDM_HILOGE(MODULE_SERVICE, "check challenge num error: no challenge!");
        RequestResult reqRet;
        innerkitsCallback->OnResult(FAIL, reqRet);
        return FAIL;
    }
    result = data_->CheckScheduleIdIsActive(scheduleId);
    if (result) {
        // current session in active
        USERIDM_HILOGE(MODULE_SERVICE, "current session in active: busy!");
        RequestResult reqRet;
        innerkitsCallback->OnResult(BUSY, reqRet);
        return BUSY;
    }
    int32_t ret = UserIDMAdapter::GetInstance().InitSchedulation(credInfo.token, userId, credInfo.authType,
                                                                 credInfo.authSubType, scheduleId);
    if (ret == SUCCESS) {
        ret = AddCredentialCallCoauth(callerID, credInfo, innerkitsCallback, challenge, scheduleId);
    } else {
        // get sessionId failed
        USERIDM_HILOGE(MODULE_SERVICE, "call TA info: InitSchedulation failed!");
        RequestResult reqRet;
        innerkitsCallback->OnResult(FAIL, reqRet);
    }
    return ret;
}

int32_t UserIDMController::UpdateCredentialCtrl(int32_t userId, uint64_t callerID, std::string callerName,
                                                AddCredInfo & credInfo, const sptr<IIDMCallback>& innerkitsCallback)
{
    USERIDM_HILOGD(MODULE_SERVICE, "UpdateCredentialCtrl enter");
    if (innerkitsCallback == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "sorry: innerkitsCallback is nullptr!");
        return INVALID_PARAMETERS;
    }

    uint64_t scheduleId = 0;
    uint64_t challenge = 0;
    bool result = data_->CheckChallenge(challenge);
    if (!result) {
        // challenge miss return error, need openSession()
        USERIDM_HILOGE(MODULE_SERVICE, "check challenge num error: no challenge!");
        RequestResult reqRet;
        innerkitsCallback->OnResult(FAIL, reqRet);
        return FAIL;
    }
    result = data_->CheckScheduleIdIsActive(scheduleId);
    if (result) { // current session in active
        USERIDM_HILOGE(MODULE_SERVICE, "current session in active: busy!");
        RequestResult reqRet;
        innerkitsCallback->OnResult(BUSY, reqRet);
        return BUSY;
    }
    int32_t ret = UserIDMAdapter::GetInstance().InitSchedulation(credInfo.token, userId, credInfo.authType,
                                                                 credInfo.authSubType, scheduleId);
    if (SUCCESS == ret) {
        USERIDM_HILOGI(MODULE_SERVICE, "InitSchedulation SUCCESS");
        // success
        data_->InsertScheduleId(scheduleId);
        std::shared_ptr<UserIDMCoAuthHandler> coAuthCallback =
                                               std::make_shared<UserIDMCoAuthHandler>(MODIFY_CRED, challenge,
                                                                                      scheduleId, data_,
                                                                                      innerkitsCallback);
        CoAuth::AuthInfo paramInfo;
        paramInfo.SetPkgName(callerName);
        paramInfo.SetCallerUid(callerID);
        CoAuth::CoAuth::GetInstance().BeginSchedule(scheduleId, paramInfo, coAuthCallback);
    } else {
        USERIDM_HILOGE(MODULE_SERVICE, "call TA info: InitSchedulation failed!");
        RequestResult reqRet;
        innerkitsCallback->OnResult(FAIL, reqRet);
    }
    return ret;
}

int32_t UserIDMController::DelSchedleIdCtrl(uint64_t challenge)
{
    USERIDM_HILOGD(MODULE_SERVICE, "DelSchedleIdCtrl enter");

    int32_t result = FAIL;

    // check challenge -> sessionId
    uint64_t sessionId = 0;
    uint64_t lastCha = 0;
    data_->CheckChallenge(lastCha);
    bool res = data_->CheckScheduleIdIsActive(sessionId);
    if (res && (lastCha == challenge)) {
        result = CoAuth::CoAuth::GetInstance().Cancel(sessionId);
        if (result == SUCCESS) {
            // clean the sessionId
            data_->DeleteSessionId();
        } else {
            USERIDM_HILOGE(MODULE_SERVICE, "Cancel Failed!");
        }
    } else {
        USERIDM_HILOGE(MODULE_SERVICE, "Not same challenge num!");
    }

    return result;
}

int32_t UserIDMController::DelFaceCredentialCtrl(AuthType authType, AuthSubType authSubType, uint64_t credentialId,
                                                 uint64_t templateId, const sptr<IIDMCallback>& innerCallback)
{
    USERIDM_HILOGD(MODULE_SERVICE, "DelFaceCredentialCtrl enter authType: %{public}d", authType);

    if (authType == FACE) { // FACE
        std::shared_ptr<UserIDMSetPropHandler> setPropCallback =
                                               std::make_shared<UserIDMSetPropHandler>(FACE, 0, 0, credentialId,
                                                                                       data_, innerCallback);
        // set timeout in <UserIDMSetPropHandler>
        AuthResPool::AuthAttributes condition;
        condition.SetUint32Value(AuthAttributeType::AUTH_PROPERTY_MODE, 0);
        condition.SetUint64Value(AuthAttributeType::AUTH_CALLER_UID, 0);
        condition.SetUint32Value(AuthAttributeType::AUTH_TYPE, authType);
        condition.SetUint64Value(AuthAttributeType::AUTH_SUBTYPE, authSubType);
        condition.SetUint64Value(AuthAttributeType::AUTH_CREDENTIAL_ID, credentialId);
        condition.SetUint64Value(AuthAttributeType::AUTH_TEMPLATE_ID, templateId);
        CoAuth::CoAuth::GetInstance().SetExecutorProp(condition, setPropCallback);
    } else {
        USERIDM_HILOGE(MODULE_SERVICE, "authType error !");
    }

    return SUCCESS;
}

int32_t UserIDMController::DelExecutorPinInfoCtrl(const sptr<IIDMCallback>& innerCallback,
                                                  std::vector<CredentialInfo>& info)
{
    // 拼写错误
    USERIDM_HILOGD(MODULE_SERVICE, "DelExecutorPinInfoCtrl enter: info.size(): %{public}zu.", info.size());

    if (info.size() < MIN_VECTOR_SIZE) {
        USERIDM_HILOGI(MODULE_SERVICE, "info size error!: %{public}zu.", info.size());
        RequestResult reqRet;
        innerCallback->OnResult(FAIL, reqRet);
        return FAIL;
    }

    for (uint32_t i = 0; i < info.size(); i++) {
        if (PIN == info[i].authType) {
            // PIN
            std::shared_ptr<UserIDMSetPropHandler> setPropCallback =
                                                   std::make_shared<UserIDMSetPropHandler>(PIN, 0, 0,
                                                                                           info[i].credentialId,
                                                                                           data_, innerCallback);
            AuthResPool::AuthAttributes condition;
            condition.SetUint32Value(AuthAttributeType::AUTH_PROPERTY_MODE, 0);
            condition.SetUint64Value(AuthAttributeType::AUTH_CALLER_UID, 0);
            condition.SetUint32Value(AuthAttributeType::AUTH_TYPE, info[i].authType);
            condition.SetUint64Value(AuthAttributeType::AUTH_SUBTYPE, info[i].authSubType);
            condition.SetUint64Value(AuthAttributeType::AUTH_CREDENTIAL_ID, info[i].credentialId);
            condition.SetUint64Value(AuthAttributeType::AUTH_TEMPLATE_ID, info[i].templateId);
            CoAuth::CoAuth::GetInstance().SetExecutorProp(condition, setPropCallback);
        } else if (info[i].authType == FACE) {
            // FACE
            std::shared_ptr<UserIDMSetPropHandler> setPropCallback =
                                                   std::make_shared<UserIDMSetPropHandler>(FACE, 0, 0,
                                                                                           info[i].credentialId,
                                                                                           data_, innerCallback);
            AuthResPool::AuthAttributes condition;
            condition.SetUint32Value(AuthAttributeType::AUTH_PROPERTY_MODE, 0);
            condition.SetUint64Value(AuthAttributeType::AUTH_CALLER_UID, 0);
            condition.SetUint32Value(AuthAttributeType::AUTH_TYPE, info[i].authType);
            condition.SetUint64Value(AuthAttributeType::AUTH_SUBTYPE, info[i].authSubType);
            condition.SetUint64Value(AuthAttributeType::AUTH_CREDENTIAL_ID, info[i].credentialId);
            condition.SetUint64Value(AuthAttributeType::AUTH_TEMPLATE_ID, info[i].templateId);
            CoAuth::CoAuth::GetInstance().SetExecutorProp(condition, setPropCallback);
        }   // endif
    }   // end for
    return SUCCESS;
}
}  // namespace UserIDM
}  // namespace UserIAM
}  // namespace OHOS
