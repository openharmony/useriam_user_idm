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

#include<fstream>
#include<iomanip>

#include <gtest/gtest.h>
#include "useridm_client.h"
#include "useridm_callback_test.h"
#include "useridm_hilog_wrapper.h"
#include "useridm_test.h"
using namespace testing::ext;
namespace OHOS {
namespace UserIAM {
namespace UserIDM {
class UseriamUtTest : public testing::Test {
public:
    static void SetUpTestCase(void);

    static void TearDownTestCase(void);

    void SetUp();

    void TearDown();
};

void UseriamUtTest::SetUpTestCase(void)
{
}

void UseriamUtTest::TearDownTestCase(void)
{
}

void UseriamUtTest::SetUp()
{
}
void UseriamUtTest::TearDown()
{
}
/**
 * @tc.name: UseriamUtTest.UseriamUtTest001
 * @tc.desc: alg-AES pur-DECRYPT mod-CBC pad-none.
 * @tc.type: FUNC
 */

HWTEST_F(UseriamUtTest, UseriamUtTest_001, TestSize.Level1)
{
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_001  enter");
    UserIDMClient::GetInstance().OpenSession();
}
HWTEST_F(UseriamUtTest, UseriamUtTest_002, TestSize.Level1)
{
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_002  enter");
    UserIDMClient::GetInstance().CloseSession();
}
HWTEST_F(UseriamUtTest, UseriamUtTest_003, TestSize.Level1)
{
    AuthType aut=PIN;
    std::shared_ptr<GetInfoCallback> call = nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_003  enter");
    int32_t ret = UserIDMClient::GetInstance().GetAuthInfo(aut, call);
    EXPECT_NE(SUCCESS, ret);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_003b, TestSize.Level1)
{
    AuthType aut=PIN;
    std::shared_ptr<GetInfoCallback> call = std::make_shared<GetInfoCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_003b  enter");
    int32_t ret = UserIDMClient::GetInstance().GetAuthInfo(aut, call);
    EXPECT_NE(SUCCESS, ret);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_003c, TestSize.Level1)
{
    AuthType aut=FACE;
    std::shared_ptr<GetInfoCallback> call = nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_003c  enter");
    int32_t ret = UserIDMClient::GetInstance().GetAuthInfo(aut, call);
    EXPECT_NE(SUCCESS, ret);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_003d, TestSize.Level1)
{
    AuthType aut=FACE;
    std::shared_ptr<GetInfoCallback> call = std::make_shared<GetInfoCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_003d  enter");
    int32_t ret = UserIDMClient::GetInstance().GetAuthInfo(aut, call);
    EXPECT_NE(SUCCESS, ret);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_004, TestSize.Level1)
{
    std::shared_ptr<GetSecInfoCallback> call = nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_004  enter");
    int32_t ret = UserIDMClient::GetInstance().GetSecInfo(call);
    EXPECT_NE(SUCCESS, ret);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_004b, TestSize.Level1)
{
    std::shared_ptr<GetSecInfoCallback> call = std::make_shared<GetSecInfoCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_004b  enter");
    int32_t ret = UserIDMClient::GetInstance().GetSecInfo(call);
    EXPECT_NE(SUCCESS, ret);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_005, TestSize.Level1)
{
    AddCredInfo cre;
    cre.authType=FACE;
    std::shared_ptr<IDMCallback> call = nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_005  enter");
    UserIDMClient::GetInstance().OpenSession();
    UserIDMClient::GetInstance().AddCredential(cre, call);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_005b, TestSize.Level1)
{
    AddCredInfo cre;
    cre.authType = FACE;
    std::shared_ptr<IDMCallback> call = std::make_shared<IDMCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_005b  enter");
    UserIDMClient::GetInstance().OpenSession();
    UserIDMClient::GetInstance().AddCredential(cre, call);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_005c, TestSize.Level1)
{
    AddCredInfo cre;
    cre.authType = PIN;
    std::shared_ptr<IDMCallback> call = nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_005c  enter");
    UserIDMClient::GetInstance().OpenSession();
    UserIDMClient::GetInstance().AddCredential(cre, call);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_005d, TestSize.Level1)
{
    AddCredInfo cre;
    cre.authType = PIN;
    std::shared_ptr<IDMCallback> call = std::make_shared<IDMCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_005d  enter");
    UserIDMClient::GetInstance().OpenSession();
    UserIDMClient::GetInstance().AddCredential(cre, call);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_006, TestSize.Level1)
{
    AddCredInfo cre;
    std::shared_ptr<IDMCallback> call = nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_006  enter");
    UserIDMClient::GetInstance().OpenSession();
    UserIDMClient::GetInstance().UpdateCredential(cre, call);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_006b, TestSize.Level1)
{
    AddCredInfo cre;
    std::shared_ptr<IDMCallback> call = std::make_shared<IDMCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_006b  enter");
    UserIDMClient::GetInstance().OpenSession();
    UserIDMClient::GetInstance().UpdateCredential(cre, call);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_007, TestSize.Level1)
{
    static uint64_t challenge = 1;
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_007  enter");
    int32_t ret = UserIDMClient::GetInstance().Cancel(challenge);
    EXPECT_NE(SUCCESS, ret);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_008, TestSize.Level1)
{
    uint32_t userid = 0;
    std::shared_ptr<IDMCallback> call = nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_008  enter");
    int32_t ret = UserIDMClient::GetInstance().EnforceDelUser(userid, call);
    EXPECT_NE(SUCCESS, ret);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_008b, TestSize.Level1)
{
    uint32_t userid = 1;
    std::shared_ptr<IDMCallback> call = std::make_shared<IDMCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_008b  enter");
    int32_t ret = UserIDMClient::GetInstance().EnforceDelUser(userid, call);
    EXPECT_EQ(SUCCESS, ret);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_009, TestSize.Level1)
{
    std::vector<uint8_t> authToken;
    std::shared_ptr<IDMCallback> call=nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_009  enter");
    UserIDMClient::GetInstance().DelUser(authToken, call);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_009b, TestSize.Level1)
{
    std::vector<uint8_t> authToken;
    std::shared_ptr<IDMCallback> call = std::make_shared<IDMCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_009b  enter");
    UserIDMClient::GetInstance().DelUser(authToken, call);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_010, TestSize.Level1)
{
    uint64_t credentialId = 1;
    std::vector<uint8_t> authToken;
    std::shared_ptr<IDMCallback> call=nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_010  enter");
    UserIDMClient::GetInstance().DelCred(credentialId, authToken, call);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_010b, TestSize.Level1)
{
    uint64_t credentialId = 1;
    std::vector<uint8_t> authToken;
    std::shared_ptr<IDMCallback> call = std::make_shared<IDMCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_010b  enter");
    UserIDMClient::GetInstance().DelCred(credentialId, authToken, call);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_011, TestSize.Level1)
{
    AuthType aut = PIN;
    int32_t userId = 0;
    std::shared_ptr<GetInfoCallback> call =nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_011  enter");
    int32_t ret = UserIDMClient::GetInstance().GetAuthInfo(userId, aut, call);
    EXPECT_NE(SUCCESS, ret);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_011b, TestSize.Level1)
{
    AuthType aut = PIN;
    int32_t userId = 0;
    std::shared_ptr<GetInfoCallback> call = std::make_shared<GetInfoCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_011b  enter");
    int32_t ret = UserIDMClient::GetInstance().GetAuthInfo(userId, aut, call);
    EXPECT_NE(SUCCESS, ret);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_011c, TestSize.Level1)
{
    AuthType aut = FACE;
    int32_t userId = 0;
    std::shared_ptr<GetInfoCallback> call = nullptr;
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_011c  enter");
    int32_t ret = UserIDMClient::GetInstance().GetAuthInfo(userId, aut, call);
    EXPECT_NE(SUCCESS, ret);
}
HWTEST_F(UseriamUtTest, UseriamUtTest_011d, TestSize.Level1)
{
    AuthType aut = FACE;
    int32_t userId = 0;
    std::shared_ptr<GetInfoCallback> call = std::make_shared<GetInfoCallbackUT>();
    USERIDM_HILOGI(MODULE_CLIENT, " UseriamUtTest_011d  enter");
    int32_t ret = UserIDMClient::GetInstance().GetAuthInfo(userId, aut, call);
    EXPECT_NE(SUCCESS, ret);
}
}
}
}
