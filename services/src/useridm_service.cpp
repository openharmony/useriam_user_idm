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

#include "useriam_common.h"
#include "accesstoken_kit.h"
#include "useridm_service.h"
namespace OHOS {
namespace UserIAM {
namespace UserIDM {
// define System Ability
REGISTER_SYSTEM_ABILITY_BY_ID(UserIDMService, SUBSYS_USERIAM_SYS_ABILITY_USERIDM, true);

static const std::string MANAGE_USER_IDM_PERMISSION = "ohos.permission.MANAGE_USER_IDM";
static const std::string USE_USER_IDM_PERMISSION = "ohos.permission.USE_USER_IDM";

UserIDMService::UserIDMService(int32_t systemAbilityId, bool runOnCreate)
    : SystemAbility(systemAbilityId, runOnCreate)
{
}

UserIDMService::~UserIDMService()
{
}

void UserIDMService::OnStart()
{
    USERIDM_HILOGI(MODULE_SERVICE, "Start service");
    bool ret = OHOS::UserIAM::Common::IsIAMInited();
    if (!ret) {
        OHOS::UserIAM::Common::Init();
    }
    if (!Publish(this)) {
        USERIDM_HILOGE(MODULE_SERVICE, "Failed to publish service");
    }
}

void UserIDMService::OnStop()
{
    USERIDM_HILOGI(MODULE_SERVICE, "Stop service");
    bool ret = OHOS::UserIAM::Common::IsIAMInited();
    if (ret) {
        OHOS::UserIAM::Common::Close();
    }
}

int32_t UserIDMService::GetCallingUserID(int32_t &userID)
{
    using namespace Security::AccessToken;
    uint32_t tokenID = this->GetFirstTokenID();
    if (tokenID == 0) {
        tokenID = this->GetCallingTokenID();
    }
    ATokenTypeEnum callingType = AccessTokenKit::GetTokenType(tokenID);
    if (callingType != TOKEN_HAP) {
        USERIDM_HILOGI(MODULE_SERVICE, "CallingType is not hap.");
        return TYPE_NOT_SUPPORT;
    }
    HapTokenInfo hapTokenInfo;
    int result = AccessTokenKit::GetHapTokenInfo(tokenID, hapTokenInfo);
    if (result != SUCCESS) {
        USERIDM_HILOGI(MODULE_SERVICE, "Get hap token info failed.");
        return TYPE_NOT_SUPPORT;
    }
    userID = (int32_t)hapTokenInfo.userID;
    USERIDM_HILOGI(MODULE_SERVICE, "GetCallingUserID is %{public}d", userID);
    return SUCCESS;
}

bool UserIDMService::CheckPermission(const std::string &permission)
{
    using namespace Security::AccessToken;
    uint32_t tokenID = this->GetFirstTokenID();
    if (tokenID == 0) {
        tokenID = this->GetCallingTokenID();
    }
    return AccessTokenKit::VerifyAccessToken(tokenID, permission) == RET_SUCCESS;
}

uint64_t UserIDMService::OpenSession()
{
    USERIDM_HILOGD(MODULE_SERVICE, "service OpenSession enter");
    if (!CheckPermission(MANAGE_USER_IDM_PERMISSION)) {
        USERIDM_HILOGE(MODULE_SERVICE, "check permission failed");
        return 0;
    }

    int32_t userId = 0;
    uint64_t challenge = 0;
    int32_t ret = this->GetCallingUserID(userId);
    if (ret != SUCCESS) {
        USERIDM_HILOGE(MODULE_SERVICE, "Failed to get userId");
        return 0;
    }
    USERIDM_HILOGI(MODULE_SERVICE, "## OpenSession get userId: %{public}d", userId);

    idmController_.OpenEditSessionCtrl(userId, challenge);

    return challenge;
}

void UserIDMService::CloseSession()
{
    USERIDM_HILOGD(MODULE_SERVICE, "service CloseSession enter");
    if (!CheckPermission(MANAGE_USER_IDM_PERMISSION)) {
        USERIDM_HILOGE(MODULE_SERVICE, "check permission failed");
        return;
    }

    idmController_.CloseEditSessionCtrl();
}

int32_t UserIDMService::GetAuthInfo(AuthType authType, const sptr<IGetInfoCallback>& callback)
{
    USERIDM_HILOGD(MODULE_SERVICE, "service GetAuthInfo enter");
    if (!CheckPermission(USE_USER_IDM_PERMISSION)) {
        USERIDM_HILOGE(MODULE_SERVICE, "check permission failed");
        return CHECK_PERMISSION_FAILED;
    }

    int32_t userId = 0;

    int32_t ret = this->GetCallingUserID(userId);
    if (ret != SUCCESS) {
        USERIDM_HILOGE(MODULE_SERVICE, "Failed to get userId");
        return ret;
    }
    std::vector<CredentialInfo> credInfos;
    ret =  idmController_.GetAuthInfoCtrl(userId, authType, credInfos);

    // return data
    callback->OnGetInfo(credInfos);

    return ret;
}

int32_t UserIDMService::GetAuthInfo(int32_t userId, AuthType authType, const sptr<IGetInfoCallback>& callback)
{
    USERIDM_HILOGD(MODULE_SERVICE, "service GetAuthInfoById enter");

    std::vector<CredentialInfo> credInfos;
    int32_t ret = idmController_.GetAuthInfoCtrl(userId, authType, credInfos);

    // return data
    callback->OnGetInfo(credInfos);

    return ret;
}

int32_t UserIDMService::GetSecInfo(const sptr<IGetSecInfoCallback>& callback)
{
    USERIDM_HILOGD(MODULE_SERVICE, "service GetSecInfo enter");
    if (!CheckPermission(USE_USER_IDM_PERMISSION)) {
        USERIDM_HILOGE(MODULE_SERVICE, "check permission failed");
        return CHECK_PERMISSION_FAILED;
    }

    int32_t userId = 0;

    int32_t ret = this->GetCallingUserID(userId);
    if (ret != SUCCESS) {
        USERIDM_HILOGE(MODULE_SERVICE, "Failed to get userId");
        return ret;
    }
    SecInfo secInfos;
    ret =  idmController_.GetSecureInfoCtrl(userId, secInfos.secureUid, secInfos.enrolledInfo);
    if (SUCCESS != ret) {
        USERIDM_HILOGE(MODULE_SERVICE, "GetSecureInfoCtrl failed");
    }

    secInfos.enrolledInfoLen = secInfos.enrolledInfo.size();
    USERIDM_HILOGI(MODULE_SERVICE, "SecInfo enrolledInfoLen is %u", secInfos.enrolledInfoLen);

    callback->OnGetSecInfo(secInfos);

    return ret;
}

void UserIDMService::AddCredential(AddCredInfo& credInfo, const sptr<IIDMCallback>& callback)
{
    USERIDM_HILOGI(MODULE_SERVICE, "service AddCredential enter");
    if (!CheckPermission(MANAGE_USER_IDM_PERMISSION)) {
        USERIDM_HILOGE(MODULE_SERVICE, "check permission failed");
        RequestResult reqRet;
        callback->OnResult(CHECK_PERMISSION_FAILED, reqRet);
        return;
    }
    uint64_t callerID = this->GetCallingUid();

    int32_t userId = 0;
    int32_t ret = this->GetCallingUserID(userId);
    if (ret != SUCCESS) {
        USERIDM_HILOGE(MODULE_SERVICE, "GetCallingUserID failed!");
        RequestResult reqRet;
        callback->OnResult(ret, reqRet);
        return;
    }
    idmController_.AddCredentialCtrl(userId, callerID, credInfo, callback);
}

void UserIDMService::UpdateCredential(AddCredInfo& credInfo, const sptr<IIDMCallback>& innerkitsCallback)
{
    USERIDM_HILOGD(MODULE_SERVICE, "service UpdateCredential enter");
    if (!CheckPermission(MANAGE_USER_IDM_PERMISSION)) {
        USERIDM_HILOGE(MODULE_SERVICE, "check permission failed");
        RequestResult reqRet;
        innerkitsCallback->OnResult(CHECK_PERMISSION_FAILED, reqRet);
        return;
    }
    uint64_t callerID = this->GetCallingUid();
    std::string callerName = std::to_string(callerID);
    int32_t userId = 0;
    int32_t ret = this->GetCallingUserID(userId);
    if (ret != SUCCESS) {
        USERIDM_HILOGE(MODULE_SERVICE, "GetCallingUserID failed!");
        RequestResult reqRet;
        innerkitsCallback->OnResult(ret, reqRet);
        return;
    }
    idmController_.UpdateCredentialCtrl(userId, callerID, callerName, credInfo, innerkitsCallback);
}

int32_t UserIDMService::Cancel(uint64_t challenge)
{
    USERIDM_HILOGD(MODULE_SERVICE, "service Cancel enter");
    if (!CheckPermission(MANAGE_USER_IDM_PERMISSION)) {
        USERIDM_HILOGE(MODULE_SERVICE, "check permission failed");
        return CHECK_PERMISSION_FAILED;
    }

    // Check the sessionid corresponding to the challenge, query the map and thread lock
    int32_t ret = idmController_.DelSchedleIdCtrl(challenge);

    return ret;
}

int32_t UserIDMService::EnforceDelUser(int32_t userId, const sptr<IIDMCallback>& callback)
{
    USERIDM_HILOGD(MODULE_SERVICE, "service EnforceDelUser enter");

    int32_t ret = 0;

    // get accountmgr info
    std::vector<CredentialInfo> credInfos;

    ret = idmController_.DeleteUserByForceCtrl(userId, credInfos);
    if (SUCCESS != ret) {
        USERIDM_HILOGE(MODULE_SERVICE, "DeleteUserByForceCtrl return fail");
        RequestResult reqRet;
        reqRet.credentialId = 0;
        callback->OnResult(ret, reqRet);
    } else {
        ret = idmController_.DelExecutorPinInfoCtrl(callback, credInfos);
    }

    return ret;
}

void UserIDMService::DelUser(std::vector<uint8_t> authToken, const sptr<IIDMCallback>& callback)
{
    USERIDM_HILOGD(MODULE_SERVICE, "service DelUser enter");
    if (!CheckPermission(MANAGE_USER_IDM_PERMISSION)) {
        USERIDM_HILOGE(MODULE_SERVICE, "check permission failed");
        RequestResult reqRet;
        callback->OnResult(CHECK_PERMISSION_FAILED, reqRet);
        return;
    }

    int32_t userId = 0;
    int32_t ret =  this->GetCallingUserID(userId);
    if (ret != SUCCESS) {
        USERIDM_HILOGE(MODULE_SERVICE, "GetCallingUserID failed");
        RequestResult reqRet;
        callback->OnResult(ret, reqRet);
        return;
    }
    std::vector<CredentialInfo> credInfos;

    ret =  idmController_.DeleteUserCtrl(userId, authToken, credInfos);
    if (ret == SUCCESS) {
        USERIDM_HILOGE(MODULE_SERVICE, "DeleteUserCtrl success");
        idmController_.DelExecutorPinInfoCtrl(callback, credInfos);
    } else {
        USERIDM_HILOGE(MODULE_SERVICE, "DeleteUserCtrl failed");
        RequestResult reqRet;
        callback->OnResult(ret, reqRet);
    }
}

void UserIDMService::DelCred(uint64_t credentialId, std::vector<uint8_t> authToken,
                             const sptr<IIDMCallback>& innerkitsCallback)
{
    USERIDM_HILOGD(MODULE_SERVICE, "service DelCred enter");
    if (!CheckPermission(MANAGE_USER_IDM_PERMISSION)) {
        USERIDM_HILOGE(MODULE_SERVICE, "check permission failed");
        RequestResult reqRet;
        innerkitsCallback->OnResult(CHECK_PERMISSION_FAILED, reqRet);
        return;
    }

    int32_t userId = 0;
    int32_t ret = this->GetCallingUserID(userId);
    if (ret != SUCCESS) {
        USERIDM_HILOGE(MODULE_SERVICE, "GetCallingUserID failed");
        RequestResult reqRet;
        innerkitsCallback->OnResult(ret, reqRet);
    }
    CredentialInfo credentialInfo;
    ret =  idmController_.DeleteCredentialCtrl(userId, credentialId, authToken, credentialInfo);
    if (ret == SUCCESS) {
        USERIDM_HILOGI(MODULE_SERVICE, "DeleteCredentialCtrl success");

        idmController_.DelFaceCredentialCtrl(credentialInfo.authType, credentialInfo.authSubType,
                                             credentialInfo.credentialId, credentialInfo.templateId,
                                             innerkitsCallback);
    } else {
        USERIDM_HILOGE(MODULE_SERVICE, "DeleteCredentialCtrl failed");
        RequestResult reqRet;
        innerkitsCallback->OnResult(ret, reqRet);
    }
}
}  // namespace UserIDM
}  // namespace UserIAM
}  // namespace OHOS
