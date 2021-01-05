/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <functional>
#include <optional>
#include <vector>

#include <emp-sh2pc/emp-sh2pc.h>

#include "PrivateData.h"

namespace private_lift::secret_sharing {

/**
 * Custom emp::Integer that can be used with these functions to allow
 * integers to be passed without providing an explicit length.
 */
class Integer64 : public emp::Integer {
 public:
  Integer64(long long input, int party = emp::PUBLIC)
      : emp::Integer{INT_SIZE, input, party} {}
  Integer64(int len, const void* b) : emp::Integer{len, b} {}

  // batcher
  template <typename... Args>
  static size_t bool_size(Args... args) {
    return emp::Integer::bool_size(INT_SIZE, std::forward<Args>(args)...);
  }
  static void bool_data(bool* data, long long num) {
    return emp::Integer::bool_data(data, INT_SIZE, num);
  }
};

/*
 * Share one emp::Integer bidirectionally between both parties
 */
template <int MY_ROLE>
PrivateInt<MY_ROLE> privatelyShareInt(int64_t in);

/*
 * Share emp::Integers bidirectionally between both parties
 * numVals = number of items to share (if std::nullopt, use in.size())
 */
template <int MY_ROLE>
const std::vector<PrivateInt<MY_ROLE>> privatelyShareInts(
    const std::vector<int64_t>& in,
    std::optional<int64_t> numVals = std::nullopt,
    int bitLen = INT_SIZE);

/*
 * Share emp::Bits bidirectionally between both parties
 * numVals = number of items to share (if std::nullopt, use in.size())
 */
template <int MY_ROLE>
const std::vector<PrivateBit<MY_ROLE>> privatelyShareBits(
    const std::vector<int64_t>& in,
    std::optional<int64_t> numVals = std::nullopt);

/*
 * Share emp::Integers from SOURCE_ROLE to the opposite party
 * numVals = number of items to share
 */
template <int MY_ROLE, int SOURCE_ROLE>
const std::vector<emp::Integer> privatelyShareIntsFrom(
    const std::vector<int64_t>& in,
    int64_t numVals,
    int bitLen = INT_SIZE);

/*
 * Share emp::Bits from SOURCE_ROLE to the opposite party
 * numVals = number of items to share
 */
template <int MY_ROLE, int SOURCE_ROLE>
const std::vector<emp::Bit> privatelyShareBitsFrom(
    const std::vector<int64_t>& in,
    int64_t numVals);

/*
 * Share an array of type T from SOURCE_ROLE to the opposite party,
 * return an array of type O.
 *
 * O must be emp::batcher compatible. That means O must implement
 *  1) O::bool_size(T val)
 *  2) O::bool_data(bool* data, T val)
 *  3) O(int len, const emp::block* b)
 *
 * T must also be ostream and == compatible to support debug logging.
 *
 * numVals = number of items to share
 * nullValue = value to initialize for the non-source role.
 */
template <int MY_ROLE, int SOURCE_ROLE, typename T, typename O>
const std::vector<O>
privatelyShareArrayFrom(const std::vector<T>& in, int64_t numVals, T nullValue);

/*
 * Share an array of T arrays from SOURCE_ROLE to the opposite party,
 * returning a vector of O arrays.
 *
 * The inner arrays will be padded to prevent the other party from
 * learning how many items are in the arrays.
 *
 * privatelyShareArrayFrom will be used to share the inner arrays.
 *
 * maxArraySize = maximum inner array size
 * paddingValue = value to pad the inner arrays with
 * numVals = number of items to share
 */
template <int MY_ROLE, int SOURCE_ROLE, typename T, typename O>
const std::vector<std::vector<O>> privatelyShareArraysFrom(
    const std::vector<std::vector<T>>& in,
    int64_t numVals,
    int64_t maxArraySize,
    T paddingValue);

/*
 * Share an array of pre-padded int arrays from SOURCE_ROLE to the opposite
 * party.
 *
 * The inner arrays must be in size arraySize. No padding will be performed.
 *
 * privatelyShareArrayFrom will be used to share the inner arrays.
 *
 * numVals = number of items to share
 * arraySize = mandatory inner array size
 * bitLen = number of bits each emp::integer takes to optimize memory usage
 */
template <int MY_ROLE, int SOURCE_ROLE>
const std::vector<std::vector<emp::Integer>>
privatelyShareIntArraysNoPaddingFrom(
    const std::vector<std::vector<int64_t>>& in,
    int64_t numVals,
    int64_t arraySize,
    int bitLen);

/*
 * Share emp::Integers from ALICE to BOB
 * numVals = number of items to share
 */
template <int MY_ROLE>
const std::vector<emp::Integer> privatelyShareIntsFromAlice(
    const std::vector<int64_t>& in,
    int64_t numVals,
    int bitLen = INT_SIZE) {
  return privatelyShareIntsFrom<MY_ROLE, emp::ALICE>(in, numVals, bitLen);
}

/*
 * Share emp::Integers from BOB to ALICE
 * numVals = number of items to share
 */
template <int MY_ROLE>
const std::vector<emp::Integer> privatelyShareIntsFromBob(
    const std::vector<int64_t>& in,
    int64_t numVals,
    int bitLen = INT_SIZE) {
  return privatelyShareIntsFrom<MY_ROLE, emp::BOB>(in, numVals, bitLen);
}

/*
 * Share emp::Bits from ALICE to BOB
 * numVals = number of items to share
 */
template <int MY_ROLE>
const std::vector<emp::Bit> privatelyShareBitsFromAlice(
    const std::vector<int64_t>& in,
    int64_t numVals) {
  return privatelyShareBitsFrom<MY_ROLE, emp::ALICE>(in, numVals);
}

/*
 * Share emp::Bits from BOB to ALICE
 * numVals = number of items to share
 */
template <int MY_ROLE>
const std::vector<emp::Bit> privatelyShareBitsFromBob(
    const std::vector<int64_t>& in,
    int64_t numVals) {
  return privatelyShareBitsFrom<MY_ROLE, emp::BOB>(in, numVals);
}

/*
 * Share an array of arrays from ALICE to BOB
 *
 * The inner arrays will be padded to prevent the other party from
 * learning how many items are in the arrays.
 *
 * maxArraySize = maximum inner array size
 * paddingValue = value to pad the inner arrays with
 * numVals = number of items to share
 */
template <int MY_ROLE, typename T, typename O>
const std::vector<std::vector<O>> privatelyShareArraysFromAlice(
    const std::vector<std::vector<T>>& in,
    int64_t numVals,
    int64_t maxArraySize,
    T paddingValue) {
  return privatelyShareArraysFrom<MY_ROLE, emp::ALICE, T, O>(
      in, numVals, maxArraySize, paddingValue);
}

/*
 * Share an array of arrays from BOB to ALICE
 *
 * The inner arrays will be padded to prevent the other party from
 * learning how many items are in the arrays.
 *
 * maxArraySize = maximum inner array size
 * paddingValue = value to pad the inner arrays with
 * numVals = number of items to share
 */
template <int MY_ROLE, typename T, typename O>
const std::vector<std::vector<O>> privatelyShareArraysFromBob(
    const std::vector<std::vector<T>>& in,
    int64_t numVals,
    int64_t maxArraySize,
    T paddingValue) {
  return privatelyShareArraysFrom<MY_ROLE, emp::BOB, T, O>(
      in, numVals, maxArraySize, paddingValue);
}

/*
 * Share an array of pre-padded int arrays from BOB to ALICE
 *
 * The inner arrays must be in size arraySize. No padding will be performed.
 *
 * arraySize = mandatory inner array size
 * numVals = number of items to share
 * bitLen = number of bits each emp::integer takes to optimize memory usage
 */
template <int MY_ROLE>
const std::vector<std::vector<emp::Integer>>
privatelyShareIntArraysNoPaddingFromBob(
    const std::vector<std::vector<int64_t>>& in,
    int64_t numVals,
    int64_t arraySize,
    int bitLen) {
  return privatelyShareIntArraysNoPaddingFrom<MY_ROLE, emp::BOB>(
      in, numVals, arraySize, bitLen);
}

/*
 * Execute map_fn on pairwise items from vec1 and vec2
 */
template <typename T, typename S>
void zip(
    const std::vector<T>& vec1,
    const std::vector<S>& vec2,
    std::function<void(T, S)> map_fn);

/*
 * Execute map_fn on elements of vec and returns mapped values
 * construct a vector of the return type of map_fn and return that to
 * the caller
 */
template <typename T, typename O>
const std::vector<O> map(const std::vector<T>& vec, std::function<O(T)> map_fn);

/*
 * Execute map_fn on pairwise items from vec1 and vec2
 * construct a vector of the return type of map_fn and return that to
 * the caller
 */
template <typename T, typename S, typename O>
const std::vector<O> zip_and_map(
    const std::vector<T>& vec1,
    const std::vector<S>& vec2,
    std::function<O(T, S)> map_fn);

/*
 * Execute map_fn on pairwise items from vec1, vec2, and vec3
 * construct a vector of the return type of map_fn and return that to
 * the caller
 */
template <typename T, typename S, typename R, typename O>
const std::vector<O> zip_and_map(
    const std::vector<T>& vec1,
    const std::vector<S>& vec2,
    const std::vector<R>& vec3,
    std::function<O(T, S, R)> map_fn);

/*
 * Multiply vec by the bitmask. If the mask is 1 at element i, accept
 * vec[i] If the mask is 0 at element i, accept 0 (default constructed
 * T, effectively)
 */
template <typename T>
const std::vector<T> multiplyBitmask(
    const std::vector<T>& vec,
    const std::vector<emp::Bit>& bitmask);

} // namespace private_lift::secret_sharing

#include "SecretSharing.hpp"
