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

namespace OHOS {
namespace UserIAM {
namespace UserIDM {
void UserIDMCallbackProxy::OnResult(int32_t result, RequestResult reqRet)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMCallbackProxy OnResult enter");

    MessageParcel data;
    MessageParcel reply;

    if (!data.WriteInterfaceToken(UserIDMCallbackProxy::GetDescriptor())) {
        USERIDM_HILOGI(MODULE_INNERKIT, "write descriptor failed!");
        return ;
    }

    if (!data.WriteInt32(result)) {
        USERIDM_HILOGE(MODULE_INNERKIT, "failed to WriteInt32(result).");
        return;
    }

    if (!data.WriteUint64(reqRet.credentialId)) {
        USERIDM_HILOGE(MODULE_INNERKIT, "failed to WriteUint64(reqRet.credentialId).");
        return;
    }

    bool ret = SendRequest(IDM_CALLBACK_ON_RESULT, data, reply);
    if (ret) {
        int32_t result = reply.ReadInt32();
        USERIDM_HILOGI(MODULE_INNERKIT, "result = %{public}d", result);
    }
}

void UserIDMCallbackProxy::OnAcquireInfo(int32_t module, int32_t acquire, RequestResult reqRet)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMCallbackProxy OnAcquireInfo enter");

    MessageParcel data;
    MessageParcel reply;

    if (!data.WriteInterfaceToken(UserIDMCallbackProxy::GetDescriptor())) {
        USERIDM_HILOGI(MODULE_INNERKIT, "write descriptor failed!");
        return;
    }

    if (!data.WriteInt32(module)) {
        USERIDM_HILOGE(MODULE_INNERKIT, "failed to WriteInt32(module).");
        return;
    }

    if (!data.WriteInt32(acquire)) {
        USERIDM_HILOGE(MODULE_INNERKIT, "failed to WriteInt32(acquire).");
        return;
    }

    if (!data.WriteUint64(reqRet.credentialId)) {
        USERIDM_HILOGE(MODULE_INNERKIT, "failed to WriteUint64(reqRet.credentialId).");
        return;
    }

    bool ret = SendRequest(IDM_CALLBACK_ON_RESULT, data, reply);
    if (ret) {
        int32_t result = reply.ReadInt32();
        USERIDM_HILOGI(MODULE_INNERKIT, "result = %{public}d", result);
    }
}

bool UserIDMCallbackProxy::SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, bool isSync)
{
    USERIDM_HILOGI(MODULE_INNERKIT, "UserIDMCallbackProxy SendRequest enter");

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        USERIDM_HILOGE(MODULE_INNERKIT, "failed to get remote.");
        return false;
    }
    MessageOption option(MessageOption::TF_SYNC);
    if (!isSync) {
        option.SetFlags(MessageOption::TF_ASYNC);
    }

    int32_t result = remote->SendRequest(code, data, reply, option);
    if (result != OHOS::UserIAM::UserIDM::SUCCESS) {
        USERIDM_HILOGE(MODULE_INNERKIT, "failed to SendRequest.result = %{public}d", result);
        return false;
    }
    return true;
}
}  // namespace UserIDM
}  // namespace UserIAM
}  // namespace OHOS
