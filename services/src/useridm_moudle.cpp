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

#include "useridm_moudle.h"
#include "useridm_hilog_wrapper.h"

namespace OHOS {
namespace UserIAM {
namespace UserIDM {
bool UserIDMMoudle::InsertChallenge(uint64_t challenge)
{
    std::lock_guard<std::mutex> idmMutexGuard(mutex_);
    USERIDM_HILOGI(MODULE_INNERKIT, "Data moudle InsertChallenge enter");

    myPair_.first = challenge;

    return true;
}

bool UserIDMMoudle::InsertSessionId(uint64_t sessionId)
{
    std::lock_guard<std::mutex> idmMutexGuard(mutex_);
    USERIDM_HILOGI(MODULE_INNERKIT, "Data moudle InsertSessionId enter");

    myPair_.second = sessionId;

    return true;
}

void UserIDMMoudle::DeleteChallenge()
{
    std::lock_guard<std::mutex> idmMutexGuard(mutex_);
    USERIDM_HILOGI(MODULE_INNERKIT, "Data moudle DeleteChallenge enter");

    myPair_.first = 0;
}

void UserIDMMoudle::DeleteSessionId()
{
    std::lock_guard<std::mutex> idmMutexGuard(mutex_);
    USERIDM_HILOGI(MODULE_INNERKIT, "Data moudle DeleteSessionId enter");

    myPair_.second = 0;
}

void UserIDMMoudle::CleanData()
{
    std::lock_guard<std::mutex> idmMutexGuard(mutex_);
    USERIDM_HILOGI(MODULE_INNERKIT, "Data moudle CleanData enter");

    // clean pair
    myPair_.first = 0;
    myPair_.second = 0;

    USERIDM_HILOGI(MODULE_INNERKIT, "After CleanData: challenge num: [%llu] session num: [%llu]",
                   myPair_.first, myPair_.second);
}

bool UserIDMMoudle::CheckChallenge(uint64_t& challenge)
{
    std::lock_guard<std::mutex> idmMutexGuard(mutex_);
    USERIDM_HILOGI(MODULE_INNERKIT, "Data moudle CheckChallenge enter");
    bool res = false;

    if (myPair_.first == 0) {
        USERIDM_HILOGE(MODULE_INNERKIT, "no session num!");
    } else {
        challenge = myPair_.first;
        res = true;
    }

    return res;
}

bool UserIDMMoudle::CheckSessionId(uint64_t& sessionId)
{
    std::lock_guard<std::mutex> idmMutexGuard(mutex_);
    USERIDM_HILOGI(MODULE_INNERKIT, "Data moudle CheckSessionId enter");
    bool res = false;

    if (myPair_.second == 0) {
        USERIDM_HILOGE(MODULE_INNERKIT, "no session num!");
    } else {
        res = true;
        sessionId = myPair_.second;
        USERIDM_HILOGE(MODULE_INNERKIT, "session num: [%{public}llu]", myPair_.second);
    }

    return res;
}
}  // namespace UserIDM
}  // namespace UserIAM
}  // namespace OHOS