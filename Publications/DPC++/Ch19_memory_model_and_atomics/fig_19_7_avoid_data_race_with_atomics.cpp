// Copyright (C) 2020 Intel Corporation

// SPDX-License-Identifier: MIT

#include <CL/sycl.hpp>
#include <algorithm>
#include <iostream>

using namespace sycl;
using namespace sycl::ONEAPI;

int main() {

  using memory_order = sycl::ONEAPI::memory_order;
  using memory_scope = sycl::ONEAPI::memory_scope;

  queue Q;

  const size_t N = 32;
  const size_t M = 4;

  int* data = malloc_shared<int>(N, Q);
  std::fill(data, data + N, 0);

  Q.parallel_for(N, [=](id<1> i) {
     int j = i % M;
     atomic_ref<int, memory_order::relaxed, memory_scope::system,
                access::address_space::global_space> atomic_data(data[j]);
     atomic_data += 1;
   }).wait();

  for (int i = 0; i < N; ++i) {
    std::cout << "data [" << i << "] = " << data[i] << "\n";
  }
  bool passed = true;
  int* gold = (int*) malloc(N * sizeof(int));
  std::fill(gold, gold + N, 0);
  for (int i = 0; i < N; ++i) {
    int j = i % M;
    gold[j] += 1;
  }
  for (int i = 0; i < N; ++i) {
    if (data[i] != gold[i]) {
      passed = false;
    }
  }
  std::cout << ((passed) ? "SUCCESS\n" : "FAILURE\n");
  free(gold);
  free(data, Q);
  return (passed) ? 0 : 1;
}
