/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
*/

#pragma once

#include <string>
#include <vector>

#include "../../pcf/mpc/EmpApp.h"
#include "../../pcf/mpc/EmpGame.h"
#include "../common/GroupedLiftMetrics.h"
#include "KAnonymityLiftAggregationGame.h"

namespace private_lift {
class KAnonymityAggregatorApp : public pcf::EmpApp<
                          KAnonymityLiftAggregationGame<emp::NetIO>,
                          std::vector<GroupedLiftMetrics>,
                          GroupedLiftMetrics> {
 public:
  KAnonymityAggregatorApp(
      pcf::Party party,
      pcf::Visibility visibility,
      const std::string& serverIp,
      uint16_t port,
      int32_t numShards,
      int64_t threshold,
      const std::string& inputPath,
      const std::string& outputPath)
      : pcf::EmpApp<
            KAnonymityLiftAggregationGame<emp::NetIO>,
            std::vector<GroupedLiftMetrics>,
            GroupedLiftMetrics>{party, serverIp, port},
        numShards_{numShards},
        threshold_{threshold},
        inputPath_{inputPath},
        outputPath_{outputPath},
        visibility_{visibility} {}

  void run() override;

 protected:
  std::vector<GroupedLiftMetrics> getInputData() override;

  void putOutputData(const GroupedLiftMetrics& metrics) override;

 private:
  static std::vector<std::string> getInputPaths(
      const std::string& inputPath,
      int32_t numShards);

 private:
  int32_t numShards_;
  int64_t threshold_;
  std::string inputPath_;
  std::string outputPath_;
  pcf::Visibility visibility_;
};
} // namespace private_lift
