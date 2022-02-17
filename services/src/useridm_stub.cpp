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

#include <message_parcel.h>

#include "useridm_hilog_wrapper.h"
#include "useridm_stub.h"
namespace OHOS {
namespace UserIAM {
namespace UserIDM {
UserIDMStub::UserIDMStub()
{
    m_handle_[static_cast<int32_t>(IUserIDM::USERIDM_OPEN_SESSION)]  = &UserIDMStub::OpenSessionStub;
    m_handle_[static_cast<int32_t>(IUserIDM::USERIDM_CLOSE_SESSION)] = &UserIDMStub::CloseSessionStub;
    m_handle_[static_cast<int32_t>(IUserIDM::USERIDM_GET_AUTH_INFO)] = &UserIDMStub::GetAuthInfoStub;
    m_handle_[static_cast<int32_t>(IUserIDM::USERIDM_GET_SEC_INFO)]  = &UserIDMStub::GetSecInfoStub;
    m_handle_[static_cast<int32_t>(IUserIDM::USERIDM_ADD_CREDENTIAL)] = &UserIDMStub::AddCredentialStub;
    m_handle_[static_cast<int32_t>(IUserIDM::USERIDM_UPDATE_CREDENTIAL)] = &UserIDMStub::UpdateCredentialStub;
    m_handle_[static_cast<int32_t>(IUserIDM::USERIDM_CANCEL)]  = &UserIDMStub::CancelSub;
    m_handle_[static_cast<int32_t>(IUserIDM::USERIDM_ENFORCE_DELUSER)]  = &UserIDMStub::EnforceDelUserStub;
    m_handle_[static_cast<int32_t>(IUserIDM::USERIDM_DELUSER)]  = &UserIDMStub::DelUserStub;
    m_handle_[static_cast<int32_t>(IUserIDM::USERIDM_DELCRED)]  = &UserIDMStub::DelCredStub;
}
int32_t UserIDMStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    USERIDM_HILOGI(MODULE_SERVICE, "UserIDMStub::OnRemoteRequest, cmd = %u, flags= %d", code, option.GetFlags());

    std::u16string descripter = UserIDMStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        USERIDM_HILOGE(MODULE_SERVICE, "UserIDMStub::OnRemoteRequest failed, descriptor is not matched!");
        return FAIL;
    }
    typedef std::map<int32_t, PHandle>::const_iterator CI;
    CI iter = m_handle_.find(code);
    if (iter == m_handle_.end()) {
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    PHandle pFunction = iter->second;
    return (this->*pFunction)(data, reply);
}

int32_t UserIDMStub::OpenSessionStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGD(MODULE_SERVICE, "OpenSessionStub enter");

    uint64_t ret = 0;

    ret = OpenSession();
    if (!reply.WriteUint64(ret)) {
        USERIDM_HILOGE(MODULE_SERVICE, "failed to WriteUint64(ret)");
        return FAIL;
    }

    return SUCCESS;
}

int32_t UserIDMStub::CloseSessionStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGD(MODULE_SERVICE, "CloseSessionStub enter");

    CloseSession();

    return SUCCESS;
}

int32_t UserIDMStub::GetAuthInfoStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGD(MODULE_SERVICE, "GetAuthInfoStub enter");

    AuthType authType = static_cast<AuthType>(data.ReadUint32());

    sptr<IGetInfoCallback> callback = iface_cast<IGetInfoCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "callback is nullptr");
        return ERR_INVALID_VALUE;
    }

    int32_t ret = GetAuthInfo(authType, callback);
    if (!reply.WriteInt32(ret)) {
        USERIDM_HILOGE(MODULE_SERVICE, "failed to WriteInt32(ret)");
        return FAIL;
    }

    return SUCCESS;
}

int32_t UserIDMStub::GetAuthInfoByIdStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGD(MODULE_SERVICE, "GetAuthInfoStub enter");

    int32_t userId = data.ReadInt32();
    AuthType authType = static_cast<AuthType>(data.ReadUint32());
    sptr<IGetInfoCallback> callback = iface_cast<IGetInfoCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "callback is nullptr");
        return ERR_INVALID_VALUE;
    }

    int32_t ret = GetAuthInfo(userId, authType, callback);
    if (!reply.WriteInt32(ret)) {
        USERIDM_HILOGE(MODULE_SERVICE, "failed to WriteInt32(ret)");
        return FAIL;
    }

    return SUCCESS;
}

int32_t UserIDMStub::GetSecInfoStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGD(MODULE_SERVICE, "GetSecInfoStub enter");

    sptr<IGetSecInfoCallback> callback = iface_cast<IGetSecInfoCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "callback is nullptr");
        return ERR_INVALID_VALUE;
    }

    int32_t ret = GetSecInfo(callback); // Get user security ID [to service]
    if (!reply.WriteInt32(ret)) {
        USERIDM_HILOGE(MODULE_SERVICE, "failed to WriteInt32(ret)");
        return FAIL;
    }

    return SUCCESS;
}

int32_t UserIDMStub::AddCredentialStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGD(MODULE_SERVICE, "AddCredentialStub enter");

    AddCredInfo credInfo;
    credInfo.authType = static_cast<AuthType>(data.ReadUint32());
    credInfo.authSubType = static_cast<AuthSubType>(data.ReadUint64());
    data.ReadUInt8Vector(&(credInfo.token));

    sptr<IIDMCallback> callback = iface_cast<IIDMCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "callback is nullptr");
        return FAIL;
    }

    AddCredential(credInfo, callback);
    // Obtain the user security ID, add the user credential information, and return the result code and additional
    // information acquireinfo through callback
    return SUCCESS;
}

int32_t UserIDMStub::UpdateCredentialStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGD(MODULE_SERVICE, "UpdateCredentialStub enter");

    AddCredInfo credInfo;
    credInfo.authType = static_cast<AuthType>(data.ReadUint32());
    credInfo.authSubType = static_cast<AuthSubType>(data.ReadUint64());
    data.ReadUInt8Vector(&(credInfo.token));

    sptr<IIDMCallback> callback = iface_cast<IIDMCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "callback is nullptr");
        return FAIL;
    }

    UpdateCredential(credInfo, callback);
    // Update user credential information and return result code and additional information acquireinfo by callback
    return SUCCESS;
}

int32_t UserIDMStub::CancelSub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGD(MODULE_SERVICE, "CancelSub enter");

    uint64_t challenge = data.ReadUint64();

    int32_t ret = Cancel(challenge);
    // Cancel the entry, pass in the challenge value, and the result is returned through ret
    if (!reply.WriteInt32(ret)) {
        USERIDM_HILOGE(MODULE_SERVICE, "failed to WriteInt32(ret)");
        return FAIL;
    }

    return ret;
}

int32_t UserIDMStub::EnforceDelUserStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGD(MODULE_SERVICE, "EnforceDelUserStub enter");

    int32_t userId = data.ReadInt32();

    sptr<IIDMCallback> callback = iface_cast<IIDMCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "callback is nullptr");
        return FAIL;
    }

    int32_t ret = EnforceDelUser(userId, callback);
    // Forcibly delete the user ※ judge that accountmgr is done in the service. After the request,
    // data analysis and circular call deletion are all done in the service
    if (!reply.WriteInt32(ret)) {
        USERIDM_HILOGE(MODULE_SERVICE, "failed to WriteInt32(ret)");
        return FAIL;
    }
    return ret;
}

int32_t UserIDMStub::DelUserStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGD(MODULE_SERVICE, "DelUserStub enter");

    std::vector<uint8_t> authToken;
    data.ReadUInt8Vector(&authToken);

    sptr<IIDMCallback> callback = iface_cast<IIDMCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "callback is nullptr");
        return FAIL;
    }

    DelUser(authToken, callback);
    // Delete the user credential information, pass in the user password authentication token and callback,
    // and obtain the deletion result through the callback
    return SUCCESS;
}

int32_t UserIDMStub::DelCredStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGD(MODULE_SERVICE, "DelCredStub enter");

    uint64_t credentialId = data.ReadUint64();
    std::vector<uint8_t> authToken;
    data.ReadUInt8Vector(&authToken);
    sptr<IIDMCallback> callback = iface_cast<IIDMCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        USERIDM_HILOGE(MODULE_SERVICE, "callback is nullptr");
        return FAIL;
    }

    DelCred(credentialId, authToken, callback);
    return SUCCESS;
}
}  // namespace UserIDM
}  // namespace UserIAM
}  // namespace OHOS
