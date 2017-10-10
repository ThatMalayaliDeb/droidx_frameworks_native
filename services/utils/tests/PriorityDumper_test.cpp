/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "serviceutils/PriorityDumper.h"

#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <utils/String16.h>
#include <utils/Vector.h>

using namespace android;

using ::testing::ElementsAreArray;
using ::testing::Mock;
using ::testing::Test;

class PriorityDumperMock : public PriorityDumper {
public:
    MOCK_METHOD2(dumpCritical, status_t(int, const Vector<String16>&));
    MOCK_METHOD2(dumpHigh, status_t(int, const Vector<String16>&));
    MOCK_METHOD2(dumpNormal, status_t(int, const Vector<String16>&));
    MOCK_METHOD2(dumpAll, status_t(int, const Vector<String16>&));
};

class DumpAllMock : public PriorityDumper {
public:
    MOCK_METHOD2(dumpCritical, status_t(int, const Vector<String16>&));
    MOCK_METHOD2(dumpHigh, status_t(int, const Vector<String16>&));
    MOCK_METHOD2(dumpNormal, status_t(int, const Vector<String16>&));
};

class PriorityDumperTest : public Test {
public:
    PriorityDumperTest() : dumper_(), dumpAlldumper_(), fd(1) {}
    PriorityDumperMock dumper_;
    DumpAllMock dumpAlldumper_;
    int fd;
};

static void addAll(Vector<String16>& av, const std::vector<std::string>& v) {
    for (auto element : v) {
        av.add(String16(element.c_str()));
    }
}

TEST_F(PriorityDumperTest, noArgsPassed) {
    Vector<String16> args;
    EXPECT_CALL(dumper_, dumpAll(fd, ElementsAreArray(args)));
    dumper_.priorityDump(fd, args);
}

TEST_F(PriorityDumperTest, noPriorityArgsPassed) {
    Vector<String16> args;
    addAll(args, {"bunch", "of", "args"});
    EXPECT_CALL(dumper_, dumpAll(fd, ElementsAreArray(args)));
    dumper_.priorityDump(fd, args);
}

TEST_F(PriorityDumperTest, priorityArgsOnly) {
    Vector<String16> args;
    addAll(args, {"--dump-priority", "CRITICAL"});
    Vector<String16> strippedArgs;
    EXPECT_CALL(dumper_, dumpCritical(fd, ElementsAreArray(strippedArgs)));
    dumper_.priorityDump(fd, args);
}

TEST_F(PriorityDumperTest, dumpCritical) {
    Vector<String16> args;
    addAll(args, {"--dump-priority", "CRITICAL", "args", "left", "behind"});
    Vector<String16> strippedArgs;
    addAll(strippedArgs, {"args", "left", "behind"});

    EXPECT_CALL(dumper_, dumpCritical(fd, ElementsAreArray(strippedArgs)));
    dumper_.priorityDump(fd, args);
}

TEST_F(PriorityDumperTest, dumpHigh) {
    Vector<String16> args;
    addAll(args, {"--dump-priority", "HIGH", "args", "left", "behind"});
    Vector<String16> strippedArgs;
    addAll(strippedArgs, {"args", "left", "behind"});

    EXPECT_CALL(dumper_, dumpHigh(fd, ElementsAreArray(strippedArgs)));
    dumper_.priorityDump(fd, args);
}

TEST_F(PriorityDumperTest, dumpNormal) {
    Vector<String16> args;
    addAll(args, {"--dump-priority", "NORMAL", "args", "left", "behind"});
    Vector<String16> strippedArgs;
    addAll(strippedArgs, {"args", "left", "behind"});

    EXPECT_CALL(dumper_, dumpNormal(fd, ElementsAreArray(strippedArgs)));
    dumper_.priorityDump(fd, args);
}

TEST_F(PriorityDumperTest, dumpAll) {
    Vector<String16> args;
    addAll(args, {"args", "left", "behind"});

    EXPECT_CALL(dumpAlldumper_, dumpCritical(fd, ElementsAreArray(args)));
    EXPECT_CALL(dumpAlldumper_, dumpHigh(fd, ElementsAreArray(args)));
    EXPECT_CALL(dumpAlldumper_, dumpNormal(fd, ElementsAreArray(args)));

    dumpAlldumper_.priorityDump(fd, args);
}

TEST_F(PriorityDumperTest, priorityArgWithPriorityMissing) {
    Vector<String16> args;
    addAll(args, {"--dump-priority"});
    EXPECT_CALL(dumper_, dumpAll(fd, ElementsAreArray(args)));

    dumper_.priorityDump(fd, args);
}

TEST_F(PriorityDumperTest, priorityArgWithInvalidPriority) {
    Vector<String16> args;
    addAll(args, {"--dump-priority", "REALLY_HIGH"});
    EXPECT_CALL(dumper_, dumpAll(fd, ElementsAreArray(args)));

    dumper_.priorityDump(fd, args);
}