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


#ifndef USERIDM_STUB_H
#define USERIDM_STUB_H

#include <map>
#include <iremote_stub.h>

#include "iuser_idm.h"

namespace OHOS {
namespace UserIAM {
namespace UserIDM {
class UserIDMStub : public IRemoteStub<IUserIDM> {
public:
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    uint64_t OpenSessionStub(MessageParcel& data, MessageParcel& reply);
    void CloseSessionStub(MessageParcel& data, MessageParcel& reply);
    int32_t GetAuthInfoStub(MessageParcel& data, MessageParcel& reply);
    int32_t GetAuthInfoByIdStub(MessageParcel& data, MessageParcel& reply);
    int32_t GetSecInfoStub(MessageParcel& data, MessageParcel& reply);
    void AddCredentialStub(MessageParcel& data, MessageParcel& reply);
    void UpdateCredentialStub(MessageParcel& data, MessageParcel& reply);
    int32_t CancelSub(MessageParcel& data, MessageParcel& reply);
    int32_t EnforceDelUserStub(MessageParcel& data, MessageParcel& reply);
    void DelUserStub(MessageParcel& data, MessageParcel& reply);
    void DelCredStub(MessageParcel& data, MessageParcel& reply);
};
}  // namespace UserIDM
}  // namespace UserIAM
}  // namespace OHOS
#endif // USERIDM_STUB_H
