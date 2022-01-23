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

#include "useridm_adapter.h"
#include "useridm_hilog_wrapper.h"
namespace OHOS {
namespace UserIAM {
namespace UserIDM {
UserIDMAdapter &UserIDMAdapter::GetInstance()
{
    static UserIDMAdapter instance;
    return instance;
}

void UserIDMAdapter::OpenEditSession(int32_t userId, uint64_t &challenge)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMAdapter OpenEditSession enter");

    // call TA interface OpenEditSession()
    int32_t ret = OHOS::UserIAM::UserIDM::Hal::OpenSession(userId, challenge);
    USERIDM_HILOGI(MODULE_INNERKIT, "Call TA info: OpenSession: %{public}d", ret);
}

void UserIDMAdapter::CloseEditSession()
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMAdapter CloseEditSession enter");

    // call TA interface CloseEditSession()
    int32_t ret = OHOS::UserIAM::UserIDM::Hal::CloseSession();
    USERIDM_HILOGI(MODULE_INNERKIT, "Call TA info: CloseSession: %{public}d", ret);
}

int32_t UserIDMAdapter::QueryCredential(int32_t userId, AuthType authType,
                                        std::vector<OHOS::UserIAM::UserIDM::CredentialInfo>& credInfos)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMAdapter QueryCredential enter");

    std::vector<OHOS::UserIAM::UserIDM::Hal::CredentialInfo> taInfos;
    int32_t ret = OHOS::UserIAM::UserIDM::Hal::QueryCredential(userId, authType, taInfos);
    if (SUCCESS == ret) {
        uint32_t vectorSize = taInfos.size();
        if (vectorSize > 0) {
            for (uint32_t i = 0; i < vectorSize; i++) {
                OHOS::UserIAM::UserIDM::CredentialInfo credInfo;
                credInfo.authSubType = OHOS::UserIAM::UserIDM::AuthSubType(taInfos[i].authSubType);
                credInfo.authType = OHOS::UserIAM::UserIDM::AuthType(taInfos[i].authType);
                credInfo.credentialId = taInfos[i].credentialId;
                credInfo.templateId = taInfos[i].templateId;
                credInfos.push_back(credInfo);
            }
        } else {
            USERIDM_HILOGI(MODULE_INNERKIT, "vector size is: %{public}d", vectorSize);
        }
    } else {
        USERIDM_HILOGE(MODULE_INNERKIT, "call ta info error: %{public}d", ret);
    }

    return ret;
}

int32_t UserIDMAdapter::GetSecureUid(int32_t userId, uint64_t& secureUid,
                                     std::vector<OHOS::UserIAM::UserIDM::EnrolledInfo>& enroInfos)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMAdapter GetSecureUid enter");

    std::vector<OHOS::UserIAM::UserIDM::Hal::EnrolledInfo> taInfos;
    int32_t ret = OHOS::UserIAM::UserIDM::Hal::GetSecureUid(userId, secureUid, taInfos);
    if (SUCCESS == ret) {
        uint32_t vectorSize = taInfos.size();
        if (vectorSize > 0) {
            for (uint32_t i = 0; i < vectorSize; i++) {
                OHOS::UserIAM::UserIDM::EnrolledInfo enroInfo;
                enroInfo.authType = OHOS::UserIAM::UserIDM::AuthType(taInfos[i].authType);
                enroInfo.enrolledId = taInfos[i].enrolledId;
                enroInfos.push_back(enroInfo);
            }
        } else {
            USERIDM_HILOGI(MODULE_INNERKIT, "vector size is: %{public}d", vectorSize);
        }
    } else {
        USERIDM_HILOGI(MODULE_INNERKIT, "Call TA info: GetSecureUid: %{public}d", ret);
    }

    return ret;
}

int32_t UserIDMAdapter::InitSchedulation(std::vector<uint8_t> autoToken, int32_t userId, AuthType authType,
                                         AuthSubType authSubType, uint64_t & sessionId)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMAdapter InitSchedulation enter");

    // call TA interface InitSchedulation()
    int32_t ret = OHOS::UserIAM::UserIDM::Hal::InitSchedulation(autoToken, userId, authType, authSubType, sessionId);
    USERIDM_HILOGI(MODULE_INNERKIT, "Call TA info: GetScheduleId: %{public}d", ret);

    return ret;
}

int32_t UserIDMAdapter::DeleteCredential(int32_t userId, uint64_t credentialId, std::vector<uint8_t> authToken,
                                         OHOS::UserIAM::UserIDM::CredentialInfo& credInfo)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMAdapter DeleteCredential enter");

    OHOS::UserIAM::UserIDM::Hal::CredentialInfo taInfo;
    int32_t ret = OHOS::UserIAM::UserIDM::Hal::DeleteCredential(userId, credentialId, authToken, taInfo);
    if (SUCCESS == ret) {
        credInfo.authSubType = OHOS::UserIAM::UserIDM::AuthSubType(taInfo.authSubType);
        credInfo.authType = OHOS::UserIAM::UserIDM::AuthType(taInfo.authType);
        credInfo.credentialId = taInfo.credentialId;
        credInfo.templateId = taInfo.templateId;
    } else {
        USERIDM_HILOGE(MODULE_INNERKIT, "get ta info error: %{public}d", ret);
    }
    
    USERIDM_HILOGI(MODULE_INNERKIT, "Call TA info: DeleteCredential: %{public}d", ret);

    return ret;
}

int32_t UserIDMAdapter::DeleteUser(int32_t userId, std::vector<uint8_t> authToken,
                                   std::vector<OHOS::UserIAM::UserIDM::CredentialInfo>& credInfos)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMAdapter DeleteUser enter");

    std::vector<OHOS::UserIAM::UserIDM::Hal::CredentialInfo> taInfos;
    int32_t ret = OHOS::UserIAM::UserIDM::Hal::DeleteUser(userId, authToken, taInfos);
    USERIDM_HILOGI(MODULE_INNERKIT, "### ---> after Hal info");
    if (SUCCESS == ret) {
        uint32_t vectorSize = taInfos.size();
        USERIDM_HILOGI(MODULE_INNERKIT, "### ---> taInfos.size() %{public}d", taInfos.size());
        if (vectorSize > 0) {
            for (uint32_t i = 0; i < vectorSize; i++) {
                OHOS::UserIAM::UserIDM::CredentialInfo credInfo;
                credInfo.authSubType = OHOS::UserIAM::UserIDM::AuthSubType(taInfos[i].authSubType);
                credInfo.authType = OHOS::UserIAM::UserIDM::AuthType(taInfos[i].authType);
                credInfo.credentialId = taInfos[i].credentialId;
                credInfo.templateId = taInfos[i].templateId;
                credInfos.push_back(credInfo);
            }
            USERIDM_HILOGI(MODULE_INNERKIT, "### ---> after for");
        } else {
            USERIDM_HILOGI(MODULE_INNERKIT, "vector size is: %{public}d", vectorSize);
        }
    } else {
        USERIDM_HILOGE(MODULE_INNERKIT, "get ta info error: %{public}d", ret);
    }

    return ret;
}

int32_t UserIDMAdapter::DeleteUserEnforce(int32_t userId,
                                          std::vector<OHOS::UserIAM::UserIDM::CredentialInfo>& credInfos)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMAdapter DeleteUserEnforce enter");

    std::vector<OHOS::UserIAM::UserIDM::Hal::CredentialInfo> taInfos;
    int32_t ret = OHOS::UserIAM::UserIDM::Hal::DeleteUserEnforce(userId, taInfos);
    if (SUCCESS == ret) {
        uint32_t vectorSize = taInfos.size();
        if (vectorSize > 0) {
            for (uint32_t i = 0; i < vectorSize; i++) {
                OHOS::UserIAM::UserIDM::CredentialInfo credInfo;
                credInfo.authSubType = OHOS::UserIAM::UserIDM::AuthSubType(taInfos[i].authSubType);
                credInfo.authType = OHOS::UserIAM::UserIDM::AuthType(taInfos[i].authType);
                credInfo.credentialId = taInfos[i].credentialId;
                credInfo.templateId = taInfos[i].templateId;
                credInfos.push_back(credInfo);
            }
        } else {
            USERIDM_HILOGI(MODULE_INNERKIT, "vector size is: %{public}d", vectorSize);
        }
    } else {
        USERIDM_HILOGE(MODULE_INNERKIT, "call TA info error: %{public}d", ret);
    }

    return ret;
}

int32_t UserIDMAdapter::AddCredential(std::vector<uint8_t>& enrollToken, uint64_t& credentialId)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMAdapter AddCredential enter");

    int32_t ret = OHOS::UserIAM::UserIDM::Hal::AddCredential(enrollToken, credentialId);
    USERIDM_HILOGI(MODULE_INNERKIT, "Call TA info: AddCredential: %{public}d", ret);

    return ret;
}

int32_t UserIDMAdapter::UpdateCredential(std::vector<uint8_t> enrollToken, uint64_t &credentialId,
                                         CredentialInfo &deletedCredential)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMAdapter UpdateCredential enter");

    OHOS::UserIAM::UserIDM::Hal::CredentialInfo taInfo;
    int32_t ret = OHOS::UserIAM::UserIDM::Hal::UpdateCredential(enrollToken, credentialId, taInfo);
    if (SUCCESS == ret) {
        deletedCredential.authSubType = OHOS::UserIAM::UserIDM::AuthSubType(taInfo.authSubType);
        deletedCredential.authType = OHOS::UserIAM::UserIDM::AuthType(taInfo.authType);
        deletedCredential.credentialId = taInfo.credentialId;
        deletedCredential.templateId = taInfo.templateId;
    } else {
        USERIDM_HILOGI(MODULE_INNERKIT, "Call TA info: UpdateCredential: %{public}d", ret);
    }

    return ret;
}
}  // namespace UserIDM
}  // namespace UserIAM
}  // namespace OHOS