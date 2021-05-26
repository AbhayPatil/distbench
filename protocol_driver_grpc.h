// Copyright 2021 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef DISTBENCH_PROTOCOL_DRIVER_GRPC_H_
#define DISTBENCH_PROTOCOL_DRIVER_GRPC_H_

#include "distbench.grpc.pb.h"
#include "distbench_utils.h"
#include "protocol_driver.h"

namespace distbench {
class TrafficService;

class ProtocolDriverGrpc : public ProtocolDriver {
 public:
  ProtocolDriverGrpc();
  ~ProtocolDriverGrpc() override;

  absl::Status Initialize(
      std::string_view netdev_name, int port) override;

  void SetHandler(std::function<void(ServerRpcState* state)> handler) override;
  void SetNumPeers(int num_peers) override;

  // Connects to the actual GRPC service.
  absl::Status HandleConnect(
      std::string remote_connection_info, int peer) override;

  // Returns the address of the GRPC service.
  absl::StatusOr<std::string> HandlePreConnect(
      std::string_view remote_connection_info, int peer) override;

  std::vector<TransportStat> GetTransportStats() override;
  void InitiateRpc(int peer_index, ClientRpcState* state,
                   std::function<void(void)> done_callback) override;
  void ChurnConnection(int peer) override;
  void ShutdownServer() override;
  void ShutdownClient() override;

 private:
  void RpcCompletionThread();

  std::atomic<int> pending_rpcs_ = 0;
  absl::Notification shutdown_;
  std::thread cq_poller_;
  grpc::CompletionQueue cq_;
  std::unique_ptr<TrafficService> traffic_service_;
  std::unique_ptr<grpc::Server> server_;
  std::vector<std::unique_ptr<Traffic::Stub>> grpc_client_stubs_;
  int server_port_ = 0;
  std::string server_ip_address_;
  std::string server_socket_address_;
};

}  // namespace distbench

#endif  // DISTBENCH_PROTOCOL_DRIVER_GRPC_H_