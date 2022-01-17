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
int32_t UserIDMStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMStub::OnRemoteRequest, cmd = %d, flags= %d", code, option.GetFlags());

    std::u16string descripter = UserIDMStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        USERIDM_HILOGE(MODULE_INNERKIT, "UserIDMStub::OnRemoteRequest failed, descriptor is not matched!");
        return FAIL;
    }

    switch (code) {
        case static_cast<int32_t>(IUserIDM::USERIDM_OPEN_SESSION):
            return OpenSessionStub(data, reply);
        case static_cast<int32_t>(IUserIDM::USERIDM_CLOSE_SESSION):
            CloseSessionStub(data, reply);
            return SUCCESS;
        case static_cast<int32_t>(IUserIDM::USERIDM_GET_AUTH_INFO):
            return GetAuthInfoStub(data, reply);
        case static_cast<int32_t>(IUserIDM::USERIDM_GET_SEC_INFO):
            return GetSecInfoStub(data, reply);
        case static_cast<int32_t>(IUserIDM::USERIDM_ADD_CREDENTIAL):
            AddCredentialStub(data, reply);
            return SUCCESS;
        case static_cast<int32_t>(IUserIDM::USERIDM_UPDATE_CREDENTIAL):
            UpdateCredentialStub(data, reply);
            return SUCCESS;
        case static_cast<int32_t>(IUserIDM::USERIDM_CANCEL):
            return CancelSub(data, reply);
        case static_cast<int32_t>(IUserIDM::USERIDM_ENFORCE_DELUSER):
            return EnforceDelUserStub(data, reply);
        case static_cast<int32_t>(IUserIDM::USERIDM_DELUSER):
            DelUserStub(data, reply);
            return SUCCESS;
        case static_cast<int32_t>(IUserIDM::USERIDM_DELCRED):
            DelCredStub(data, reply);
            return SUCCESS;
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

uint64_t UserIDMStub::OpenSessionStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "OpenSessionStub enter ");

    uint64_t ret = 0;

    ret = OpenSession();
    if (!reply.WriteUint64(ret)) {
        USERIDM_HILOGE(MODULE_INNERKIT, "failed to WriteUint64(ret)");
        return FAIL;
    }

    return ret;
}

void UserIDMStub::CloseSessionStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "CloseSessionStub enter ");

    CloseSession();
}

int32_t UserIDMStub::GetAuthInfoStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "GetAuthInfoStub enter ");

    AuthType authType = static_cast<AuthType>(data.ReadUint32());

    sptr<IGetInfoCallback> callback = iface_cast<IGetInfoCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, "callback is nullptr");
        return ERR_INVALID_VALUE;
    }

    int32_t ret = GetAuthInfo(authType, callback);
    if (!reply.WriteInt32(ret)) {
        USERIDM_HILOGE(MODULE_INNERKIT, "failed to WriteInt32(ret)");
        return FAIL;
    }

    return SUCCESS;
}

int32_t UserIDMStub::GetSecInfoStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "GetSecInfoStub enter ");

    sptr<IGetSecInfoCallback> callback = iface_cast<IGetSecInfoCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, "callback is nullptr");
        return ERR_INVALID_VALUE;
    }

    int32_t ret = GetSecInfo(callback);   // Get user security ID [to service]
    if (!reply.WriteInt32(ret)) {
        USERIDM_HILOGE(MODULE_INNERKIT, "failed to WriteInt32(ret)");
        return FAIL;
    }

    return SUCCESS;
}

void UserIDMStub::AddCredentialStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "AddCredentialStub enter ");

    AddCredInfo credInfo;
    credInfo.authType = static_cast<AuthType>(data.ReadUint32());
    credInfo.authSubType = static_cast<AuthSubType>(data.ReadUint64());
    data.ReadUInt8Vector(&(credInfo.token));

    sptr<IIDMCallback> callback = iface_cast<IIDMCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, "callback is nullptr");
        return;
    }

    AddCredential(credInfo, callback);
    // Obtain the user security ID, add the user credential information, and return the result code and additional
    // information acquireinfo through callback
}

void UserIDMStub::UpdateCredentialStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UpdateCredentialStub enter ");

    AddCredInfo credInfo;
    credInfo.authType = static_cast<AuthType>(data.ReadUint32());
    credInfo.authSubType = static_cast<AuthSubType>(data.ReadUint64());
    data.ReadUInt8Vector(&(credInfo.token));

    sptr<IIDMCallback> callback = iface_cast<IIDMCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, "callback is nullptr");
        return;
    }

    UpdateCredential(credInfo, callback);
    // Update user credential information and return result code and additional information acquireinfo by callback
}

int32_t UserIDMStub::CancelSub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "CancelSub enter ");

    uint64_t challenge = data.ReadUint64();

    int32_t ret = Cancel(challenge);
    // Cancel the entry, pass in the challenge value, and the result is returned through ret
    if (!reply.WriteInt32(ret)) {
        USERIDM_HILOGE(MODULE_INNERKIT, "failed to WriteInt32(ret)");
        return FAIL;
    }

    return ret;
}

int32_t UserIDMStub::EnforceDelUserStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "EnforceDelUserStub enter ");

    int32_t userId = data.ReadInt32();

    sptr<IIDMCallback> callback = iface_cast<IIDMCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, "callback is nullptr");
        return FAIL;
    }

    int32_t ret = EnforceDelUser(userId, callback);
    // Forcibly delete the user ※ judge that accountmgr is done in the service. After the request,
    // data analysis and circular call deletion are all done in the service
    if (!reply.WriteInt32(ret)) {
        USERIDM_HILOGE(MODULE_INNERKIT, "failed to WriteInt32(ret)");
        return FAIL;
    }
    return ret;
}

void UserIDMStub::DelUserStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "DelUserStub enter ");

    std::vector<uint8_t> authToken;
    data.ReadUInt8Vector(&authToken);

    sptr<IIDMCallback> callback = iface_cast<IIDMCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, "callback is nullptr");
        return;
    }

    DelUser(authToken, callback);
    // Delete the user credential information, pass in the user password authentication token and callback,
    // and obtain the deletion result through the callback
}

void UserIDMStub::DelCredStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "DelCredStub enter ");

    uint64_t credentialId = data.ReadUint64();
    std::vector<uint8_t> authToken;
    data.ReadUInt8Vector(&authToken);
    sptr<IIDMCallback> callback = iface_cast<IIDMCallback>(data.ReadRemoteObject());
    if (callback == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, "callback is nullptr");
        return;
    }

    DelCred(credentialId, authToken, callback);
}
}  // namespace UserIDM
}  // namespace UserIAM
}  // namespace OHOS