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

#include "useridm_hilog_wrapper.h"
#include "useridm_moudle.h"
namespace OHOS {
namespace UserIAM {
namespace UserIDM {
bool UserIDMMoudle::InsertChallenge(uint64_t challenge)
{
    std::lock_guard<std::mutex> idmMutexGuard(mutex_);
    USERIDM_HILOGI(MODULE_SERVICE, "Data moudle InsertChallenge enter");

    challengeAndScheduleId_.first = challenge;

    return true;
}

bool UserIDMMoudle::InsertScheduleId(uint64_t scheduleId)
{
    std::lock_guard<std::mutex> idmMutexGuard(mutex_);
    USERIDM_HILOGI(MODULE_SERVICE, "Data moudle InsertScheduleId enter");

    challengeAndScheduleId_.second = scheduleId;

    return true;
}

void UserIDMMoudle::DeleteChallenge()
{
    std::lock_guard<std::mutex> idmMutexGuard(mutex_);
    USERIDM_HILOGI(MODULE_SERVICE, "Data moudle DeleteChallenge enter");

    challengeAndScheduleId_.first = 0;
}

void UserIDMMoudle::DeleteSessionId()
{
    std::lock_guard<std::mutex> idmMutexGuard(mutex_);
    USERIDM_HILOGI(MODULE_SERVICE, "Data moudle DeleteSessionId enter");

    challengeAndScheduleId_.second = 0;
}

void UserIDMMoudle::CleanData()
{
    std::lock_guard<std::mutex> idmMutexGuard(mutex_);
    USERIDM_HILOGI(MODULE_SERVICE, "Data moudle CleanData enter");

    // clean pair
    challengeAndScheduleId_.first = 0;
    challengeAndScheduleId_.second = 0;
}

bool UserIDMMoudle::CheckChallenge(uint64_t& challenge)
{
    std::lock_guard<std::mutex> idmMutexGuard(mutex_);
    USERIDM_HILOGI(MODULE_SERVICE, "Data moudle CheckChallenge enter");
    bool res = false;

    if (challengeAndScheduleId_.first == 0) {
        USERIDM_HILOGE(MODULE_SERVICE, "no session num!");
    } else {
        challenge = challengeAndScheduleId_.first;
        res = true;
    }

    return res;
}

bool UserIDMMoudle::CheckScheduleIdIsActive(uint64_t& scheduleId)
{
    std::lock_guard<std::mutex> idmMutexGuard(mutex_);
    USERIDM_HILOGI(MODULE_SERVICE, "Data moudle CheckScheduleIdIsActive enter");
    bool res = false;

    if (challengeAndScheduleId_.second == 0) {
        USERIDM_HILOGE(MODULE_SERVICE, "no session num!");
    } else {
        res = true;
        scheduleId = challengeAndScheduleId_.second;
    }

    return res;
}
}  // namespace UserIDM
}  // namespace UserIAM
}  // namespace OHOS