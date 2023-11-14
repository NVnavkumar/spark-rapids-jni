/*
* Copyright (c)  2023, NVIDIA CORPORATION.
*
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

package com.nvidia.spark.rapids.jni;

import java.beans.Transient;
import java.time.Instant;
import java.time.LocalDateTime;
import java.time.ZoneId;
import java.time.ZonedDateTime;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Executor;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

import static ai.rapids.cudf.AssertUtils.assertColumnsAreEqual;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;

import ai.rapids.cudf.ColumnVector;
import ai.rapids.cudf.DType;
import ai.rapids.cudf.HostColumnVector;

import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;

public class TimeZoneTest {
  @BeforeAll
  static void cacheTimezoneDatabase() {
    Executor executor = Executors.newFixedThreadPool(1);
    GpuTimeZoneDB.cacheDatabase(executor);
  }
  
  // ColumnVector createMicrosColumnVector(Long[] epochSeconds) {
  //   int rows = epochSeconds.length;
  //   HostColumnVector.Builder builder = HostColumnVector.builder(DType.TIMESTAMP_MICROSECONDS, rows);
  //   for (int i = 0; i < rows; i++) {
  //     builder.append(epochSeconds[i] * 1000000L);
  //   }
  //   return builder.buildAndPutOnDevice();
  // }
  
  // Long[] getEpochSeconds(int startYear, int endYear) {
  //   long s = Instant.parse("%04d-01-01T00:00:00z".format(startYear)).getEpochSecond();
  //   long e = Instant.parse("%04d-01-01T00:00:00z".format(endYear)).getEpochSecond();
  //   ArrayList<Long> epochSeconds = new ArrayList<>();
  //   for (long epoch = s; epoch < e; e += TimeUnit.MINUTES.toSeconds(15)) {
  //     epochSeconds.add(epoch);
  //   }
  //   return epochSeconds.toArray(new Long[0]);
  // }
  
  @Test
  void databaseLoadTest() {
    // Check for a few timezones
    GpuTimeZoneDB instance = GpuTimeZoneDB.getInstance();
    List transitions = instance.getHostFixedTransitions("UTC+8");
    assertNotNull(transitions);
    assertEquals(1, transitions.size());
    transitions = instance.getHostFixedTransitions("Asia/Shanghai");
    assertNotNull(transitions);
    ZoneId shanghai = ZoneId.of("Asia/Shanghai").normalized();
    assertEquals(shanghai.getRules().getTransitions().size() + 1, transitions.size());
  }
  
  @Test
  void convertToUtcTest() {
    try (ColumnVector input = ColumnVector.timestampSecondsFromBoxedLongs(0L);
        ColumnVector expected = ColumnVector.timestampSecondsFromBoxedLongs(-28800L);
        ColumnVector actual = GpuTimeZoneDB.fromTimestampToUtcTimestamp(input,
          ZoneId.of("Asia/Shanghai"))) {
      assertColumnsAreEqual(expected, actual);
    }
  }
  
  @Test
  void convertFromUtcSecondsTest() {
    
  }
  
}
