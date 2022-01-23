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

// #include<fstream>
// #include<iomanip>

#include "useridm_service.h"
#include "useriam_common.h"

namespace OHOS {
namespace UserIAM {
namespace UserIDM {
// define System Ability
REGISTER_SYSTEM_ABILITY_BY_ID(UserIDMService, SUBSYS_USERIAM_SYS_ABILITY_USERIDM, true);

UserIDMService::UserIDMService(int32_t systemAbilityId, bool runOnCreate)
    : SystemAbility(systemAbilityId, runOnCreate)
{
}

UserIDMService::~UserIDMService()
{
}

void UserIDMService::OnStart()
{
    USERIDM_HILOGI(MODULE_INNERKIT, "Start service");
    bool ret = OHOS::UserIAM::Common::IsIAMInited();
    if (!ret) {
        OHOS::UserIAM::Common::Init();
    }
    if (!Publish(this)) {
        USERIDM_HILOGE(MODULE_INNERKIT, "Failed to publish service");
    }
}

void UserIDMService::OnStop()
{
    USERIDM_HILOGI(MODULE_INNERKIT, "Stop service");
    bool ret = OHOS::UserIAM::Common::IsIAMInited();
    if (ret) {
        OHOS::UserIAM::Common::Close();
    }
}

uint64_t UserIDMService::OpenSession()
{
    USERIDM_HILOGI(MODULE_INNERKIT, "service OpenSession enter");

    // getUserId
    int32_t userId = 0;
    uint64_t challenge = 0;

    idmController_.OpenEditSessionCtrl(userId, challenge);

    return challenge;
}

void UserIDMService::CloseSession()
{
    USERIDM_HILOGI(MODULE_INNERKIT, "service CloseSession enter");

    idmController_.CloseEditSessionCtrl();
}

int32_t UserIDMService::GetAuthInfo(AuthType authType, const sptr<IGetInfoCallback>& callback)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "service GetAuthInfo enter");

    // has no info userId
    int32_t userId = 0;
    std::vector<CredentialInfo> credInfos;
    int32_t ret =  idmController_.GetAuthInfoCtrl(userId, authType, credInfos);

    // return data
    callback->OnGetInfo(credInfos);

    return ret;
}

int32_t UserIDMService::GetSecInfo(const sptr<IGetSecInfoCallback>& callback)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "service GetSecInfo enter");

    // getUserId
    int32_t userId = 0;
    SecInfo secInfos;
    int32_t ret =  idmController_.GetSecureInfoCtrl(userId, secInfos.secureUid, secInfos.enrolledInfo);
    if (SUCCESS != ret) {
        USERIDM_HILOGE(MODULE_INNERKIT, "GetSecureInfoCtrl failed");
    }

    secInfos.enrolledInfoLen = secInfos.enrolledInfo.size();
    USERIDM_HILOGI(MODULE_INNERKIT, "SecInfo enrolledInfoLen is  %d", secInfos.enrolledInfoLen);

    callback->OnGetSecInfo(secInfos);

    return ret;
}

void UserIDMService::AddCredential(AddCredInfo& credInfo, const sptr<IIDMCallback>& callback)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "service AddCredential enter");
    uint64_t callerID = this->GetCallingUid();
    std::string callerName = std::to_string(callerID);

    idmController_.AddCredentialCtrl(callerID, callerName, credInfo, callback);
}

void UserIDMService::UpdateCredential(AddCredInfo& credInfo, const sptr<IIDMCallback>& innerkitsCallback)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "service UpdateCredential enter");
    uint64_t callerID = this->GetCallingUid();
    std::string callerName = std::to_string(callerID);

    idmController_.UpdateCredentialCtrl(callerID, callerName, credInfo, innerkitsCallback);
}

int32_t UserIDMService::Cancel(uint64_t challenge)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "service Cancel enter");

    // Check the sessionid corresponding to the challenge, query the map and thread lock
    int32_t ret = idmController_.DelSchedleIdCtrl(challenge);

    return ret;
}

int32_t UserIDMService::EnforceDelUser(int32_t userId, const sptr<IIDMCallback>& callback)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "service EnforceDelUser enter");

    int32_t ret = 0;

    // get accountmgr info
    // bool isAccountMgr = true;
    std::vector<CredentialInfo> credInfos;

    //if (isAccountMgr) {
        ret = idmController_.DeleteUserByForceCtrl(userId, credInfos);
        if (SUCCESS != ret) {
            USERIDM_HILOGE(MODULE_INNERKIT, "DeleteUserByForceCtrl return fail");
            RequestResult reqRet;
            reqRet.credentialId = 0;
            callback->OnResult(ret, reqRet);
        } else {
            ret = idmController_.DelExecutorPinInofCtrl(callback, credInfos);
        }
    //}

    return ret;
}

void UserIDMService::DelUser(std::vector<uint8_t> authToken, const sptr<IIDMCallback>& callback)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "service DelUser enter");

    // getUserId
    int32_t userId = 0;
    std::vector<CredentialInfo> credInfos;

    int32_t ret =  idmController_.DeleteUserCtrl(userId, authToken, credInfos);
    if (ret == SUCCESS) {
        USERIDM_HILOGE(MODULE_INNERKIT, "DeleteUserCtrl success");
        idmController_.DelExecutorPinInofCtrl(callback, credInfos);
    } else {
        USERIDM_HILOGE(MODULE_INNERKIT, "DeleteUserCtrl failed");
        RequestResult reqRet;
        callback->OnResult(ret, reqRet);
    }
}

void UserIDMService::DelCred(uint64_t credentialId, std::vector<uint8_t> authToken,
                             const sptr<IIDMCallback>& innerkitsCallback)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "service DelCred enter");

    // get userId info
    int32_t userId = 0;
    CredentialInfo credentialInfo;

    int32_t ret =  idmController_.DeleteCredentialCtrl(userId, credentialId, authToken, credentialInfo);
    if (ret == SUCCESS) {
        USERIDM_HILOGI(MODULE_INNERKIT, "DeleteCredentialCtrl success ");

        idmController_.DelFaceCredentialCtrl(credentialInfo.authType, credentialInfo.authSubType,
                                             credentialInfo.credentialId, credentialInfo.templateId,
                                             innerkitsCallback);
    } else {
        USERIDM_HILOGE(MODULE_INNERKIT, "DeleteCredentialCtrl failed ");
        RequestResult reqRet;
        innerkitsCallback->OnResult(ret, reqRet);
    }
}
}  // namespace UserIDM
}  // namespace UserIAM
}  // namespace OHOS
