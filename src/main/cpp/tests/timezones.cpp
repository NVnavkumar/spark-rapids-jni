/*
 * Copyright (c) 2023, NVIDIA CORPORATION.
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

#include "timezones.hpp"

#include <cudf/wrappers/timestamps.hpp>

#include <cudf_test/base_fixture.hpp>
#include <cudf_test/column_utilities.hpp>
#include <cudf_test/column_wrapper.hpp>
#include <cudf_test/iterator_utilities.hpp>
#include <cudf_test/type_lists.hpp>

using seconds_col = 
    cudf::test::fixed_width_column_wrapper<cudf::timestamp_s, cudf::timestamp_s::rep>;

struct TimeZoneTest : public cudf::test::BaseFixture {};

TEST_F(TimeZoneTest, ConvertToUTC) {
    auto instants_from_utc_col = cudf::test::fixed_width_column_wrapper<int64_t>({-650019600L});
    auto instants_to_utc_col = cudf::test::fixed_width_column_wrapper<int64_t>({-649990800L});
    auto utc_offsets_col = cudf::test::fixed_width_column_wrapper<int32_t>({28800});
    auto struct_column = 
        cudf::test::structs_column_wrapper{{instants_from_utc_col, instants_to_utc_col, utc_offsets_col}, {1}};
    auto offsets =
        cudf::test::fixed_width_column_wrapper<cudf::size_type>{0, 1};
    auto list_nullmask = std::vector<bool>(1, 1);
    auto [null_mask, null_count] =
        cudf::test::detail::make_null_mask(list_nullmask.begin(), list_nullmask.end());
    auto list_column = cudf::make_lists_column(
        1, offsets.release(), struct_column.release(), null_count, std::move(null_mask));
    auto columns = std::vector<std::unique_ptr<cudf::column>>{};
    columns.push_back(std::move(list_column));
    auto transitions = cudf::table{std::move(columns)};

    auto const ts_col = seconds_col{
        0L
    };
    // check the converted to utc version
    auto const expected = seconds_col{
        -28800L
    };
    auto const actual = spark_rapids_jni::convert_timestamp_to_utc(
        ts_col,
        transitions,
        0,
        cudf::get_default_stream(),
        rmm::mr::get_current_device_resource());

    CUDF_TEST_EXPECT_COLUMNS_EQUAL(expected, *actual);
}

TEST_F(TimeZoneTest, ConvertFromUTC) {
    auto instants_from_utc_col = cudf::test::fixed_width_column_wrapper<int64_t>({-650019600L});
    auto instants_to_utc_col = cudf::test::fixed_width_column_wrapper<int64_t>({-649990800L});
    auto utc_offsets_col = cudf::test::fixed_width_column_wrapper<int32_t>({28800});
    auto struct_column = 
        cudf::test::structs_column_wrapper{{instants_from_utc_col, instants_to_utc_col, utc_offsets_col}, {1}};
    auto offsets =
        cudf::test::fixed_width_column_wrapper<cudf::size_type>{0, 1};
    auto list_nullmask = std::vector<bool>(1, 1);
    auto [null_mask, null_count] =
        cudf::test::detail::make_null_mask(list_nullmask.begin(), list_nullmask.end());
    auto list_column = cudf::make_lists_column(
        1, offsets.release(), struct_column.release(), null_count, std::move(null_mask));
    auto columns = std::vector<std::unique_ptr<cudf::column>>{};
    columns.push_back(std::move(list_column));
    auto transitions = cudf::table{std::move(columns)};

    auto const ts_col = seconds_col{
        0L
    };
    // check the converted to utc version
    auto const expected = seconds_col{
        28800L
    };
    auto const actual = spark_rapids_jni::convert_utc_timestamp_to_timezone(
        ts_col,
        transitions,
        0,
        cudf::get_default_stream(),
        rmm::mr::get_current_device_resource());

    CUDF_TEST_EXPECT_COLUMNS_EQUAL(expected, *actual);
}