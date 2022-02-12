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
#include "useridm_controller.h"

const int MIN_VECTOR_SIZE = 1;

namespace OHOS {
namespace UserIAM {
namespace UserIDM {
UserIDMController::UserIDMController()
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMController constructor ");
    data_ = std::make_shared<UserIDMMoudle>();
}

UserIDMController::~UserIDMController()
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMController deconstructor ");
}

void UserIDMController::OpenEditSessionCtrl(int32_t userId, uint64_t &challenge)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "OpenEditSessionCtrl enter ");

    uint64_t sessionId = 0;
    uint64_t temp = 0;
    bool res = data_->CheckSessionId(sessionId);
    if (res && (data_->CheckChallenge(temp))) {
        // call coauth::cancel(): cancel current active session
        CoAuth::CoAuth::GetInstance().Cancel(sessionId);
    }
    data_->CleanData();

    // call TA info
    UserIDMAdapter::GetInstance().OpenEditSession(userId, challenge);

    data_->InsertChallenge(challenge);  // add challenge num
}

void UserIDMController::CloseEditSessionCtrl()
{
    USERIDM_HILOGI(MODULE_INNERKIT, "CloseEditSessionCtrl enter ");

    uint64_t sessionId = 0;
    uint64_t temp = 0;
    bool res = data_->CheckSessionId(sessionId);
    if (res && (data_->CheckChallenge(temp))) {
        // call coauth::cancel(): cancel current active session
        CoAuth::CoAuth::GetInstance().Cancel(sessionId);
    }
    data_->CleanData();

    UserIDMAdapter::GetInstance().CloseEditSession();
}

int32_t UserIDMController::GetAuthInfoCtrl(int32_t userId, AuthType authType, std::vector<CredentialInfo>& credInfos)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "CloseEditSessionCtrl enter ");
    return UserIDMAdapter::GetInstance().QueryCredential(userId, authType, credInfos);
}

int32_t UserIDMController::GetSecureInfoCtrl(int32_t userId, uint64_t& secureUid, std::vector<EnrolledInfo>& enroInfos)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "GetSecureInfoCtrl enter ");
    return UserIDMAdapter::GetInstance().GetSecureUid(userId, secureUid, enroInfos);
}

int32_t UserIDMController::DeleteCredentialCtrl(int32_t userId, uint64_t credentialId, std::vector<uint8_t> authToken,
                                                CredentialInfo& credInfo)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "DeleteCredentialCtrl enter ");
    return UserIDMAdapter::GetInstance().DeleteCredential(userId, credentialId, authToken, credInfo);
}

int32_t UserIDMController::DeleteUserCtrl(int32_t userId, std::vector<uint8_t> authToken,
                                          std::vector<CredentialInfo>& credInfo)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "DeleteUserCtrl enter ");
    return UserIDMAdapter::GetInstance().DeleteUser(userId, authToken, credInfo);
}

int32_t UserIDMController::DeleteUserByForceCtrl(int32_t userId, std::vector<CredentialInfo>& credInfo)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "DeleteUserByForceCtrl enter ");
    return UserIDMAdapter::GetInstance().DeleteUserEnforce(userId, credInfo);
}

void UserIDMController::AddCredentialCallCoauth(uint64_t callerID, std::string callerName,
    AddCredInfo& credInfo, const sptr<IIDMCallback>& innerkitsCallback, uint64_t& challenge,
    uint64_t& scheduleId, int32_t& userId)
{
    data_->InsertSessionId(scheduleId);
    // callback: as coauth info param
    std::shared_ptr<UserIDMCoAuthHandler> coAuthCallback;
    CoAuth::AuthInfo paramInfo;
    paramInfo.SetPkgName(callerName);
    paramInfo.SetCallerUid(callerID);

    USERIDM_HILOGE(MODULE_INNERKIT, "credInfo.authType is [%{public}d]!", credInfo.authType);

    if (credInfo.authType == PIN) {
        coAuthCallback = std::make_shared<UserIDMCoAuthHandler>(ADD_PIN_CRED, challenge, scheduleId, data_,
                                                                innerkitsCallback);
    } else if (FACE == credInfo.authType) {
        coAuthCallback = std::make_shared<UserIDMCoAuthHandler>(ADD_FACE_CRED, challenge, scheduleId, data_,
                                                                innerkitsCallback);
    } else {
        USERIDM_HILOGE(MODULE_INNERKIT, "credInfo.authType error: %{public}d!", credInfo.authType);
        coAuthCallback = std::make_shared<UserIDMCoAuthHandler>(ADD_PIN_CRED, challenge, scheduleId, data_,
                                                                innerkitsCallback);
    }
    if (coAuthCallback == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, "coAuthCallback is nullptr!");
    }
    // call coauth info
    CoAuth::CoAuth::GetInstance().coAuth(scheduleId, paramInfo, coAuthCallback);
}

int32_t UserIDMController::AddCredentialCtrl(int32_t userId, uint64_t callerID, std::string callerName,
    AddCredInfo& credInfo, const sptr<IIDMCallback>& innerkitsCallback)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "AddCredentialCtrl enter ");
    uint64_t scheduleId = 0;
    uint64_t challenge = 0;
    // add death recipient start
    sptr<IRemoteObject::DeathRecipient> dr = new AddCredCallbackDeathRecipient(this);
    if (!innerkitsCallback->AsObject()->AddDeathRecipient(dr)) {
        USERIDM_HILOGE(MODULE_INNERKIT, "Failed to add death recipient AddCredCallbackDeathRecipient");
    }
    addCredCallback_ = innerkitsCallback;
    USERIDM_HILOGI(MODULE_SERVICE, "add death recipient success!");
    // add death recipient end
    bool res = data_->CheckChallenge(challenge);
    if (!res) {
        // challenge miss return error, need openSession()
        USERIDM_HILOGE(MODULE_INNERKIT, "check challenge num error: no challenge!");
        RequestResult reqRet;
        innerkitsCallback->OnResult(FAIL, reqRet);
        return FAIL;
    }
    res = data_->CheckSessionId(scheduleId);
    if (res) {
        // current session in active
        USERIDM_HILOGE(MODULE_INNERKIT, "current session in active: busy!");
        RequestResult reqRet;
        innerkitsCallback->OnResult(BUSY, reqRet);
        return BUSY;  // todo siqi busy
    }
    int32_t ret = UserIDMAdapter::GetInstance().InitSchedulation(credInfo.token, userId, credInfo.authType,
                                                                 credInfo.authSubType, scheduleId);
    if (ret == SUCCESS) {
        AddCredentialCallCoauth(callerID, callerName, credInfo, innerkitsCallback, challenge, scheduleId, userId);
    } else {
        // check failed
        // no need insert sessionId
        // direct return failed
        USERIDM_HILOGE(MODULE_INNERKIT, "call TA info: InitSchedulation failed!");
        RequestResult reqRet;
        innerkitsCallback->OnResult(FAIL, reqRet);
        ret = FAIL;
    }
    USERIDM_HILOGE(MODULE_INNERKIT, "AddCredentialCtrl end");
    return ret;
}

int32_t UserIDMController::UpdateCredentialCtrl(int32_t userId, uint64_t callerID, std::string callerName,
    AddCredInfo & credInfo, const sptr<IIDMCallback>& innerkitsCallback)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UpdateCredentialCtrl enter ");
    uint64_t scheduleId = 0;
    uint64_t challenge = 0;

    sptr<IRemoteObject::DeathRecipient> dr = new UpdateCredCallbackDeathRecipient(this);
    if (!innerkitsCallback->AsObject()->AddDeathRecipient(dr)) {
        USERIDM_HILOGE(MODULE_INNERKIT, "Failed to add death recipient UpdateCredCallbackDeathRecipient");
    }
    updateCredCallback_ = innerkitsCallback;
    USERIDM_HILOGI(MODULE_SERVICE, "update death recipient success!");

    bool res = data_->CheckChallenge(challenge);
    if (!res) {
        // challenge miss return error, need openSession()
        USERIDM_HILOGE(MODULE_INNERKIT, "check challenge num error: no challenge!");
        RequestResult reqRet;
        innerkitsCallback->OnResult(FAIL, reqRet);
        return FAIL;
    }
    res = data_->CheckSessionId(scheduleId);
    if (res) {
        // current session in active
        USERIDM_HILOGE(MODULE_INNERKIT, "current session in active: busy!");
        RequestResult reqRet;
        innerkitsCallback->OnResult(BUSY, reqRet);
        return BUSY;  // todo busy
    }

    int32_t ret = UserIDMAdapter::GetInstance().InitSchedulation(credInfo.token, userId, credInfo.authType,
                                                                 credInfo.authSubType, scheduleId);
    if (SUCCESS == ret) {
        USERIDM_HILOGI(MODULE_INNERKIT, "InitSchedulation OK ");
        // success
        data_->InsertSessionId(scheduleId);
        // callback: as coauth info param
        std::shared_ptr<UserIDMCoAuthHandler> coAuthCallback =
                                               std::make_shared<UserIDMCoAuthHandler>(MODIFY_CRED, challenge,
                                                                                     scheduleId, data_,
                                                                                     innerkitsCallback);
        CoAuth::AuthInfo paramInfo;
        paramInfo.SetPkgName(callerName);
        paramInfo.SetCallerUid(callerID);
        CoAuth::CoAuth::GetInstance().coAuth(scheduleId, paramInfo, coAuthCallback);
    } else {
        // InitSchedulation failed
        USERIDM_HILOGE(MODULE_INNERKIT, "call TA info: InitSchedulation failed!");
        RequestResult reqRet;
        innerkitsCallback->OnResult(FAIL, reqRet);
        ret = FAIL;
    }
    return ret;
}

int32_t UserIDMController::DelSchedleIdCtrl(uint64_t challenge)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "DelSchedleIdCtrl enter ");

    int32_t result = FAIL;

    // check challenge -> sessionId
    uint64_t sessionId = 0;
    uint64_t lastCha = 0;
    data_->CheckChallenge(lastCha);
    bool res = data_->CheckSessionId(sessionId);
    if (res && (lastCha == challenge)) {
        result = CoAuth::CoAuth::GetInstance().Cancel(sessionId);
        // clean the sessionId
        data_->DeleteSessionId();
    } else {
        USERIDM_HILOGE(MODULE_INNERKIT, "Not same challenge num!");
    }
    
    return result;
}

int32_t UserIDMController::DelFaceCredentialCtrl(AuthType authType, AuthSubType authSubType, uint64_t credentialId,
                                                 uint64_t templateId, const sptr<IIDMCallback>& innerCallback)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "DelFaceCredentialCtrl enter authType: %{public}d", authType);

    // delete face recipient start
    sptr<IRemoteObject::DeathRecipient> dr = new DelCredCallbackDeathRecipient(this);
    if (!innerCallback->AsObject()->AddDeathRecipient(dr)) {
        USERIDM_HILOGE(MODULE_INNERKIT, "Failed to add death recipient DelCredCallbackDeathRecipient");
    }
    delCredCallback_ = innerCallback;
    USERIDM_HILOGI(MODULE_SERVICE, "delete face recipient success!");
    // delete face recipient end

    if (authType == FACE) { // FACE
        std::shared_ptr<UserIDMSetPropHandler> setPropCallback =
                                               std::make_shared<UserIDMSetPropHandler>(FACE, 0, 0, credentialId,
                                                                                       data_, innerCallback);

        AuthResPool::AuthAttributes condition;
        condition.SetUint32Value(AuthAttributeType::AUTH_PROPERTY_MODE, 0);
        condition.SetUint64Value(AuthAttributeType::AUTH_CALLER_UID, 0);
        condition.SetUint32Value(AuthAttributeType::AUTH_TYPE, authType);
        condition.SetUint64Value(AuthAttributeType::AUTH_SUBTYPE, authSubType);
        condition.SetUint64Value(AuthAttributeType::AUTH_CREDENTIAL_ID, credentialId);
        condition.SetUint64Value(AuthAttributeType::AUTH_TEMPLATE_ID, templateId);

        CoAuth::CoAuth::GetInstance().SetExecutorProp(condition, setPropCallback);
    } else {
        USERIDM_HILOGE(MODULE_INNERKIT, "authType error !");
    }

    return SUCCESS;
}

int32_t UserIDMController::DelExecutorPinInofCtrl(const sptr<IIDMCallback>& innerCallback,
                                                  std::vector<CredentialInfo>& info)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "DelExecutorPinInofCtrl enter: info.size(): %{public}d.", info.size());

    sptr<IRemoteObject::DeathRecipient> dr = new DelUserCallbackDeathRecipient(this);
    if (!innerCallback->AsObject()->AddDeathRecipient(dr)) {
        USERIDM_HILOGE(MODULE_INNERKIT, "Failed to add death recipient DelUserCallbackDeathRecipient");
    }
    delUserCallback_ = innerCallback;
    USERIDM_HILOGI(MODULE_SERVICE, "delete user recipient success!");

    if (info.size() < MIN_VECTOR_SIZE) {
        USERIDM_HILOGI(MODULE_INNERKIT, "info size error!: %{public}d.", info.size());
        RequestResult reqRet;
        innerCallback->OnResult(FAIL, reqRet);
        return FAIL;
    }

    for (uint32_t i = 0; i < info.size(); i++) {
        if (PIN == info[i].authType) {
            // PIN
            std::shared_ptr<UserIDMSetPropHandler> setPropCallback =
                std::make_shared<UserIDMSetPropHandler>(PIN, 0, 0, info[i].credentialId, data_, innerCallback);
            AuthResPool::AuthAttributes condition;
            condition.SetUint32Value(AuthAttributeType::AUTH_PROPERTY_MODE, 0);
            condition.SetUint64Value(AuthAttributeType::AUTH_CALLER_UID, 0);
            condition.SetUint32Value(AuthAttributeType::AUTH_TYPE, info[i].authType);
            condition.SetUint64Value(AuthAttributeType::AUTH_SUBTYPE, info[i].authSubType);
            condition.SetUint64Value(AuthAttributeType::AUTH_CREDENTIAL_ID, info[i].credentialId);
            condition.SetUint64Value(AuthAttributeType::AUTH_TEMPLATE_ID, info[i].templateId);
            CoAuth::CoAuth::GetInstance().SetExecutorProp(condition, setPropCallback);
        } else if (info[i].authType == FACE) {     // to be delete
            // FACE
            std::shared_ptr<UserIDMSetPropHandler> setPropCallback =
                std::make_shared<UserIDMSetPropHandler>(FACE, 0, 0, info[i].credentialId, data_, innerCallback);
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

// add cred death recipient
UserIDMController::AddCredCallbackDeathRecipient::AddCredCallbackDeathRecipient(UserIDMController* parent)
    : parent_(parent)
{
    // todo
}

void UserIDMController::AddCredCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    if (remote == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, "AddCredCallback OnRemoteDied failed, remote is nullptr");
        return;
    }

    if (parent_ != nullptr) {
        parent_->addCredCallback_ = nullptr;
    }
    USERIDM_HILOGI(MODULE_INNERKIT, "AddCredCallbackDeathRecipient: normal notice: no more hode the callback.");
}

// update cred death recipient
UserIDMController::UpdateCredCallbackDeathRecipient::UpdateCredCallbackDeathRecipient(UserIDMController* parent)
    : parent_(parent)
{
    // todo
}

void UserIDMController::UpdateCredCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    if (remote == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, "UpdateCredCallback OnRemoteDied failed, remote is nullptr");
        return;
    }

    if (parent_ != nullptr) {
        parent_->updateCredCallback_ = nullptr;
    }
    USERIDM_HILOGI(MODULE_INNERKIT, "UpdateCredCallbackDeathRecipient: normal notice: no more hode the callback.");
}

// delete face cred death recipient
UserIDMController::DelCredCallbackDeathRecipient::DelCredCallbackDeathRecipient(UserIDMController* parent)
    : parent_(parent)
{
    // todo
}

void UserIDMController::DelCredCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    if (remote == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, "DelCredCallback OnRemoteDied failed, remote is nullptr");
        return;
    }

    if (parent_ != nullptr) {
        parent_->delCredCallback_ = nullptr;
    }
    USERIDM_HILOGI(MODULE_INNERKIT, "DelCredCallbackDeathRecipient: normal notice: no more hode the callback.");
}

// delete user death recipient
UserIDMController::DelUserCallbackDeathRecipient::DelUserCallbackDeathRecipient(UserIDMController* parent)
    : parent_(parent)
{
    // todo
}

void UserIDMController::DelUserCallbackDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    if (remote == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, "DelUserCallback OnRemoteDied failed, remote is nullptr");
        return;
    }

    if (parent_ != nullptr) {
        parent_->delUserCallback_ = nullptr;
    }
    USERIDM_HILOGI(MODULE_INNERKIT, "DelUserCallbackDeathRecipient: normal notice: no more hode the callback.");
}
}  // namespace UserIDM
}  // namespace UserIAM
}  // namespace OHOS
