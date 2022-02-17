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


#ifndef IUSERIDM_H
#define IUSERIDM_H

#include "iuseridm_callback.h"

namespace OHOS {
namespace UserIAM {
namespace UserIDM {
class IUserIDM : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.useridm.IUserIDM");
    
    /**
     * @brief openSession before PIN or FACE handle
     *
     * @return challengeID.
     */
    virtual uint64_t OpenSession() = 0;

    /**
     * @brief closeSession after PIN or FACE handle
     *
     * @return void.
     */
    virtual void CloseSession() = 0;

    /**
     * @brief get auth info
     * @param authType credential type.
     * @param callback The callback function provided for caller to handle response data.
     * @return void.
     */
    virtual int32_t GetAuthInfo(AuthType authType, const sptr<IGetInfoCallback>& callback) = 0;

    /**
     * @brief get auth info by user id
     * @param userId user id.
     * @param authType credential type.
     * @param callback The callback function provided for caller to handle response data.
     * @return void.
     */
    virtual int32_t GetAuthInfo(int32_t userId, AuthType authType, const sptr<IGetInfoCallback>& callback) = 0;

    /**
     * @brief get sec info
     * @param callback The callback function provided for caller to handle response data.
     * @return void.
     */
    virtual int32_t GetSecInfo(const sptr<IGetSecInfoCallback>& callback) = 0;

    /**
     * @brief add Credential info
     * @param AddCredInfo credInfo.
     * @param callback The callback function provided for caller to handle response data.
     * @return void.
     */
    virtual void AddCredential(AddCredInfo& credInfo, const sptr<IIDMCallback>& callback) = 0;

    /**
     * @brief update credential info
     * @param AddCredInfo credInfo.
     * @param callback The callback function provided for caller to handle response data.
     * @return void.
     */
    virtual void UpdateCredential(AddCredInfo& credInfo, const sptr<IIDMCallback>& callback) = 0;

    /**
     * @brief cancel credential info add or update handle
     * @param challenge value set by app.
     * @param callback The callback function provided for caller to handle response data.
     * @return result.
     */
    virtual int32_t Cancel(uint64_t challenge) = 0;

    /**
     * @brief del user and user info
     * @param userId value of user Id.
     * @param callback The callback function provided for caller to handle response data.
     * @return result.
     */
    virtual int32_t EnforceDelUser(int32_t userId, const sptr<IIDMCallback>& callback) = 0;

    /**
     * @brief del user and user info
     * @param authToken value of token.
     * @param callback The callback function provided for caller to handle response data.
     * @return result.
     */
    virtual void DelUser(std::vector<uint8_t> authToken, const sptr<IIDMCallback>& callback) = 0;

    /**
     * @brief del credential info
     * @param credentialId the id of credential.
     * @param authToken value of token.
     * @param callback The callback function provided for caller to handle response data.
     * @return result.
     */
    virtual void DelCred(uint64_t credentialId, std::vector<uint8_t> authToken, const sptr<IIDMCallback>& callback) = 0;

    enum {
        USERIDM_OPEN_SESSION = 0,        // open the IDM editing session
        USERIDM_CLOSE_SESSION,          // close the IDM editing session
        USERIDM_GET_AUTH_INFO,          // query credential information
        USERIDM_GET_AUTH_INFO_BY_ID,    // query credential information by user id
        USERIDM_GET_SEC_INFO,           // get user security ID
        USERIDM_ADD_CREDENTIAL,         // add user credential information
        USERIDM_UPDATE_CREDENTIAL,      // update user credential information
        USERIDM_CANCEL,                 // cancel entry
        USERIDM_ENFORCE_DELUSER,        // forcibly delete the user
        USERIDM_DELUSER,                // delete password
        USERIDM_DELCRED,                // face deletion
    };
};
}  // namespace UserIDM
}  // namespace UserIAM
}  // namespace OHOS

#endif // IUSERIDM_H
