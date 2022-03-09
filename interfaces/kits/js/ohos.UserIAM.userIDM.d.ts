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

import{AsyncCallback} from './basic';
declare namespace userIDM {
    /**
     * js @calss UserIdentityManager
     */
    class UserIdentityManager {
    /**
     * constructor.
     *
     * @return Constructor to get the userauth class instance
     */
    constructor();
    /**
     * openSession.
     *
     * <p>Permissions required: {@code ohos.permission.MANAGE_USER_IDM}
     *
     * Start an IDM operation to obtain challenge value
     * A challenge value of 0 indicates that opensession failed.
     *
     * @return Uint8Array is success or fail.
     */
    openSession() : Promise<Uint8Array>;
    openSession(callback : AsyncCallback<Uint8Array>) : void;

    /**
     * getAvailabeStatus.
     *
     * <p>Permissions required: {@code ohos.permission.MANAGE_USER_IDM}
     *
     * Add user credential information, pass in credential addition method and credential information
     * (credential type, subclass, if adding user's non password credentials, pass in password authentication token),
     * and get the result / acquireinfo callback
     *
     * @param credentialInfo Incoming credential addition method and credential information
     * (credential type, subclass, password authentication token).
     * @param callback Get results / acquireinfo callback.
     */
    addCredential(credentialInfo : CredentialInfo, callback : IIdmCallback) : void;

    /**
     * updateCredential.
     *
     * <p>Permissions required: {@code ohos.permission.MANAGE_USER_IDM}
     *
     * @param credentialInfo Incoming credential addition method and credential information
     * (credential type, subclass, password authentication token).
     * @param callback Get results / acquireinfo callback.
     */
    updateCredential(credentialInfo:CredentialInfo, callback:IIdmCallback) : void;

    /**
     * closeSession.
     *
     * End an IDM operation.
     *
     * <p>Permissions required: {@code ohos.permission.MANAGE_USER_IDM}
     */
    closeSession() : void;

    /**
     * cancel.
     *
     * <p>Permissions required: {@code ohos.permission.MANAGE_USER_IDM}
     *
     * Cancel entry and pass in challenge value.
     *
     * @param challenge challenge value.
     */
    cancel(challenge : Uint8Array) : number;

    /**
     * delUser.
     *
     * <p>Permissions required: {@code ohos.permission.MANAGE_USER_IDM}
     *
     * Delete the user credential information, pass in the user password authentication token and callback,
     * and obtain the deletion result through the callback.
     *
     * @param token User password authentication token.
     * @param callback Get deletion result through callback.
     */
    delUser(token : Uint8Array, callback : IIdmCallback) : void;

    /**
     * delCred.
     *
     * <p>Permissions required: {@code ohos.permission.MANAGE_USER_IDM}
     *
     * Delete the user credential information, pass in the credential index, password authentication token and callback,
     * and obtain the deletion result through the callback Only deleting non password credentials is supported.
     *
     * @param credentialId Credential index.
     * @param token Password authentication token.
     * @param callback Get deletion result through callback.
     */
    delCred(credentialId : Uint8Array, token : Uint8Array, callback : IIdmCallback) : void;

    /**
     * getAuthInfo
     *
     * <p>Permissions required: {@code ohos.permission.ACCESS_USER_IDM}
     *
     * @param authType Credential type.
     * @param callback Returns all registered credential information of this type for the current user
     */
    getAuthInfo(callback : AsyncCallback<Array<EnrolledCredInfo>>,authType? : AuthType) : void;
    getAuthInfo(authType? : AuthType) : Promise<Array<EnrolledCredInfo>>;
    }

    /**
     * Credential information: credential type, subclass, device ID, password authentication token
     */
    interface CredentialInfo {
        credType : AuthType;
        credSubType : AuthSubType;
        token : Uint8Array;
    }

    /**
     * Return result code and additional information through callback / acquireinfo
     */
    interface IIdmCallback {
        onResult:(result : number, extraInfo : RequestResult)=>void;
        onAcquireInfo?:(module : number, acquire : number, extraInfo : any)=>void;
    }

    /**
     * Add credential result: credential index value
     */
    interface RequestResult {
        credentialId?:Uint8Array;
    }

    /**
     * Registered credential information: credential index,
     * credential type, subclass, credential template ID
     */
    interface EnrolledCredInfo {
        credentialId:Uint8Array;
        authType:AuthType;
        authSubType:AuthSubType;
        templateId:Uint8Array;
    }

    /**
     * Credential type for authentication
     */
    enum AuthType{
        /**
         * Authentication type pin.
         */
        PIN = 1,
        /**
         * Authentication type face.
         */
        FACE = 2
    }

    /**
     * Credential subtype: 6-digit digital password, user-defined digital password,
     * user-defined mixed password, 2D face, 3D face
     */
    enum AuthSubType {
        /**
         * Authentication sub type six number pin.
         */
        PIN_SIX = 10000,
        /**
         * Authentication sub type self defined number pin.
         */
        PIN_NUMBER = 10001,
        /**
         * Authentication sub type 2D face.
         */
        PIN_MIXED = 10002,
        /**
         * Authentication sub type 2D face.
         */
        FACE_2D = 20000,
        /**
         * Authentication sub type 3D face.
         */
        FACE_3D = 20001
    }
}  //namespace UserIAM
export default userIDM;