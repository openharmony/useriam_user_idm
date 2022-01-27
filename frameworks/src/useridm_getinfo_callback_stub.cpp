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
#include "useridm_getinfo_callback_stub.h"

namespace OHOS {
namespace UserIAM {
namespace UserIDM {
UserIDMGetInfoCallbackStub::UserIDMGetInfoCallbackStub(const std::shared_ptr<GetInfoCallback>& impl)
{
    callback_ = impl;
}

int32_t UserIDMGetInfoCallbackStub::OnRemoteRequest(uint32_t code,
                                                    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMGetInfoCallbackStub::OnRemoteRequest, cmd = %{public}d, flags= %d",
                   code, option.GetFlags());

    std::u16string descripter = UserIDMGetInfoCallbackStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        USERIDM_HILOGE(MODULE_INNERKIT,
                       "UserIDMGetInfoCallbackStub::OnRemoteRequest failed, descriptor is not matched!");
        return FAIL;
    }

    switch (code) {
        case static_cast<int32_t>(IGetInfoCallback::ON_GET_INFO):
            return OnGetInfoStub(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int32_t UserIDMGetInfoCallbackStub::OnGetInfoStub(MessageParcel& data, MessageParcel& reply)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMGetInfoCallbackStub OnResultStub enter ");

    int32_t ret = SUCCESS;
    uint32_t vectorSize = data.ReadUint32();    // vector size
    std::vector<CredentialInfo> credInfos;

    if (vectorSize > 0) {
        for (uint32_t i = 0; i < vectorSize; i ++) {
            CredentialInfo info;
            info.credentialId = data.ReadUint64();          // credentialId
            info.authType = static_cast<AuthType>(data.ReadUint32());               // authType
            info.authSubType = static_cast<AuthSubType>(data.ReadUint64());         // authSubType
            info.templateId = data.ReadUint64();            // templateId
            // todo
            credInfos.push_back(info);
        }
    }

    this->OnGetInfo(credInfos);
    if (!reply.WriteInt32(ret)) {
        USERIDM_HILOGE(MODULE_INNERKIT, "failed to WriteInt32(ret)");
        ret = FAIL;
    }

    return ret;
}

void UserIDMGetInfoCallbackStub::OnGetInfo(std::vector<CredentialInfo>& infos)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMGetInfoCallbackStub OnGetInfo enter ");

    if (callback_ == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, "UserIDMGetInfoCallbackStub OnGetInfo callback_ is nullptr ");
        return;
    } else {
        if (infos.size() > 0) {
            USERIDM_HILOGI(MODULE_INNERKIT, "have data");
        } else {
            USERIDM_HILOGI(MODULE_INNERKIT, "get no data");
        }
        // Call the NaPi interface and return the data to JS
        callback_->OnGetInfo(infos);
    }
}
}  // namespace UserIDM
}  // namespace UserIAM
}  // namespace OHOS
