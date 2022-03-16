/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "useridm_ut_test.h"
#include <fstream>
#include <iomanip>
#include <gtest/gtest.h>
#include "useridm_client.h"
#include "useridm_callback_test.h"
#include "useridm_hilog_wrapper.h"

using namespace testing::ext;
namespace OHOS {
namespace UserIAM {
namespace UserIDM {
class UserIDMUtTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void UserIDMUtTest::SetUpTestCase(void)
{
}

void UserIDMUtTest::TearDownTestCase(void)
{
}

void UserIDMUtTest::SetUp()
{
}
void UserIDMUtTest::TearDown()
{
}
/**
 * @tc.name: UserIDMUtTest.UserIDMUtTest001
 * @tc.desc: alg-AES pur-DECRYPT mod-CBC pad-none.
 * @tc.type: FUNC
 */

HWTEST_F(UserIDMUtTest, UserIDMUtTest_001, TestSize.Level1)
{
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_001 enter");
    UserIDMClient::GetInstance().OpenSession();
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_002, TestSize.Level1)
{
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_002 enter");
    UserIDMClient::GetInstance().CloseSession();
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_003, TestSize.Level1)
{
    AuthType authType = PIN;
    std::shared_ptr<GetInfoCallback> callback = nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_003 enter");
    int32_t ret = UserIDMClient::GetInstance().GetAuthInfo(authType, callback);
    EXPECT_NE(SUCCESS, ret);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_003b, TestSize.Level1)
{
    AuthType authType = PIN;
    std::shared_ptr<GetInfoCallback> callback = std::make_shared<GetInfoCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_003b enter");
    int32_t ret = UserIDMClient::GetInstance().GetAuthInfo(authType, callback);
    EXPECT_NE(SUCCESS, ret);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_003c, TestSize.Level1)
{
    AuthType authType = FACE;
    std::shared_ptr<GetInfoCallback> callback = nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_003c enter");
    int32_t ret = UserIDMClient::GetInstance().GetAuthInfo(authType, callback);
    EXPECT_NE(SUCCESS, ret);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_003d, TestSize.Level1)
{
    AuthType authType = FACE;
    std::shared_ptr<GetInfoCallback> callback = std::make_shared<GetInfoCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_003d enter");
    int32_t ret = UserIDMClient::GetInstance().GetAuthInfo(authType, callback);
    EXPECT_NE(SUCCESS, ret);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_004, TestSize.Level1)
{
    uint32_t userId = 1;
    std::shared_ptr<GetSecInfoCallback> callback = nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_004 enter");
    int32_t ret = UserIDMClient::GetInstance().GetSecInfo(userId, callback);
    EXPECT_NE(SUCCESS, ret);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_004b, TestSize.Level1)
{
    uint32_t userId = 1;
    std::shared_ptr<GetSecInfoCallback> callback = std::make_shared<GetSecInfoCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_004b enter");
    int32_t ret = UserIDMClient::GetInstance().GetSecInfo(userId, callback);
    EXPECT_NE(SUCCESS, ret);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_005, TestSize.Level1)
{
    AddCredInfo credInfo;
    credInfo.authType = FACE;
    std::shared_ptr<IDMCallback> callback = nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_005 enter");
    UserIDMClient::GetInstance().OpenSession();
    UserIDMClient::GetInstance().AddCredential(credInfo, callback);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_005b, TestSize.Level1)
{
    AddCredInfo credInfo;
    credInfo.authType = FACE;
    std::shared_ptr<IDMCallback> callback = std::make_shared<IDMCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_005b enter");
    UserIDMClient::GetInstance().OpenSession();
    UserIDMClient::GetInstance().AddCredential(credInfo, callback);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_005c, TestSize.Level1)
{
    AddCredInfo credInfo;
    credInfo.authType = PIN;
    std::shared_ptr<IDMCallback> callback = nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_005c enter");
    UserIDMClient::GetInstance().OpenSession();
    UserIDMClient::GetInstance().AddCredential(credInfo, callback);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_005d, TestSize.Level1)
{
    AddCredInfo credInfo;
    credInfo.authType = PIN;
    std::shared_ptr<IDMCallback> callback = std::make_shared<IDMCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_005d enter");
    UserIDMClient::GetInstance().OpenSession();
    UserIDMClient::GetInstance().AddCredential(credInfo, callback);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_006, TestSize.Level1)
{
    AddCredInfo credInfo;
    std::shared_ptr<IDMCallback> callback = nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_006 enter");
    UserIDMClient::GetInstance().OpenSession();
    UserIDMClient::GetInstance().UpdateCredential(credInfo, callback);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_006b, TestSize.Level1)
{
    AddCredInfo credInfo;
    std::shared_ptr<IDMCallback> callback = std::make_shared<IDMCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_006b enter");
    UserIDMClient::GetInstance().OpenSession();
    UserIDMClient::GetInstance().UpdateCredential(credInfo, callback);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_007, TestSize.Level1)
{
    static uint64_t challenge = 1;
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_007 enter");
    int32_t ret = UserIDMClient::GetInstance().Cancel(challenge);
    EXPECT_NE(SUCCESS, ret);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_008, TestSize.Level1)
{
    uint32_t userId = 0;
    std::shared_ptr<IDMCallback> callback = nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_008 enter");
    int32_t ret = UserIDMClient::GetInstance().EnforceDelUser(userId, callback);
    EXPECT_NE(SUCCESS, ret);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_008b, TestSize.Level1)
{
    uint32_t userId = 1;
    std::shared_ptr<IDMCallback> callback = std::make_shared<IDMCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_008b enter");
    int32_t ret = UserIDMClient::GetInstance().EnforceDelUser(userId, callback);
    EXPECT_EQ(SUCCESS, ret);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_009, TestSize.Level1)
{
    std::vector<uint8_t> authToken;
    std::shared_ptr<IDMCallback> callback = nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_009 enter");
    UserIDMClient::GetInstance().DelUser(authToken, callback);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_009b, TestSize.Level1)
{
    std::vector<uint8_t> authToken;
    std::shared_ptr<IDMCallback> callback = std::make_shared<IDMCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_009b enter");
    UserIDMClient::GetInstance().DelUser(authToken, callback);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_010, TestSize.Level1)
{
    uint64_t credentialId = 1;
    std::vector<uint8_t> authToken;
    std::shared_ptr<IDMCallback> callback = nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_010 enter");
    UserIDMClient::GetInstance().DelCred(credentialId, authToken, callback);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_010b, TestSize.Level1)
{
    uint64_t credentialId = 1;
    std::vector<uint8_t> authToken;
    std::shared_ptr<IDMCallback> callback = std::make_shared<IDMCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_010b enter");
    UserIDMClient::GetInstance().DelCred(credentialId, authToken, callback);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_011, TestSize.Level1)
{
    AuthType authType = PIN;
    int32_t userId = 0;
    std::shared_ptr<GetInfoCallback> callback = nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_011 enter");
    int32_t ret = UserIDMClient::GetInstance().GetAuthInfo(userId, authType, callback);
    EXPECT_NE(SUCCESS, ret);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_011b, TestSize.Level1)
{
    AuthType authType = PIN;
    int32_t userId = 0;
    std::shared_ptr<GetInfoCallback> callback = std::make_shared<GetInfoCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_011b enter");
    int32_t ret = UserIDMClient::GetInstance().GetAuthInfo(userId, authType, callback);
    EXPECT_NE(SUCCESS, ret);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_011c, TestSize.Level1)
{
    AuthType authType = FACE;
    int32_t userId = 0;
    std::shared_ptr<GetInfoCallback> callback = nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_011c enter");
    int32_t ret = UserIDMClient::GetInstance().GetAuthInfo(userId, authType, callback);
    EXPECT_NE(SUCCESS, ret);
}

HWTEST_F(UserIDMUtTest, UserIDMUtTest_011d, TestSize.Level1)
{
    AuthType aut = FACE;
    int32_t userId = 0;
    std::shared_ptr<GetInfoCallback> callback = std::make_shared<GetInfoCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, "UserIDMUtTest_011d enter");
    int32_t ret = UserIDMClient::GetInstance().GetAuthInfo(userId, aut, callback);
    EXPECT_NE(SUCCESS, ret);
}
}
}
}
