//benchmark adapted from https://github.com/microsoft/ALEX/tree/master/src/benchmark
#include "../core/alex.h"
#include <iomanip>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <unistd.h>
#include <random>

#define KEY_TYPE uint64_t
#define PAYLOAD_TYPE int

long get_memory_usage() {
  struct rusage used_mem;
  getrusage(RUSAGE_SELF, &used_mem);
  return used_mem.ru_maxrss;
}

void print_stats_batch(int batch_number, int num_lookups_per_batch, int num_actual_inserts, int num_updates_per_batch, auto batch_lookup_time, auto batch_insert_time, auto batch_update_time,
                        int cumulative_lookups, int cumulative_inserts, int cumulative_updates, auto cumulative_lookup_time, auto cumulative_insert_time,auto  cumulative_update_time) {
      int num_batch_operations = num_lookups_per_batch + num_actual_inserts + num_updates_per_batch;
      double batch_time = batch_lookup_time + batch_insert_time + batch_update_time;
      long long cumulative_operations = cumulative_lookups + cumulative_inserts + cumulative_updates;
      double cumulative_time = cumulative_lookup_time + cumulative_insert_time + cumulative_update_time;
      std::cout << "Batch " << batch_number
                << ", cumulative ops: " << cumulative_operations
                << "\n\tbatch throughput:\t"
                << num_lookups_per_batch / batch_lookup_time * 1e9
                << " lookups/sec,\t"
                << num_updates_per_batch / batch_update_time * 1e9
                << " updates/sec,\t"
                << num_actual_inserts / batch_insert_time * 1e9
                << " inserts/sec,\t" << num_batch_operations / batch_time * 1e9
                << " ops/sec"
                << "\n\tcumulative throughput:\t"
                << cumulative_lookups / cumulative_lookup_time * 1e9
                << " lookups/sec,\t"
                << cumulative_inserts / cumulative_insert_time * 1e9
                << " inserts/sec,\t"
                << cumulative_operations / cumulative_time * 1e9 << " ops/sec"
                << cumulative_updates / cumulative_update_time * 1e9 << " updates/sec"
                << " memory usage,\t"
                << get_memory_usage()
                << std::endl;

}

void print_stats_cumulative(int batch_number, int cumulative_lookups, int cumulative_inserts, int cumulative_updates, 
                            auto cumulative_lookup_time, auto cumulative_insert_time, auto cumulative_update_time) {
  long long cumulative_operations = cumulative_lookups + cumulative_inserts + cumulative_updates;
  double cumulative_time = cumulative_lookup_time + cumulative_insert_time + cumulative_update_time;
  std::cout << "Cumulative stats: " << batch_number << " batches, "
            << cumulative_operations << " ops (" << cumulative_lookups
            << " lookups, " << cumulative_inserts << " inserts)"
            << "\n\tcumulative throughput:\t"
            << cumulative_lookups / cumulative_lookup_time * 1e9
            << " lookups/sec,\t"
            << cumulative_inserts / cumulative_insert_time * 1e9
            << " inserts/sec,\t"
            << cumulative_updates / cumulative_update_time * 1e9
            << " updates/sec,\t"
            << cumulative_operations / cumulative_time * 1e9 << " ops/sec"
            << std::endl;
}

//method from https://github.com/microsoft/ALEX/blob/master/src/benchmark/utils.h
template <class T>
T* get_search_keys(T array[], int num_keys, int num_searches) {
  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<int> dis(0, num_keys - 1);
  auto* keys = new T[num_searches];
  for (int i = 0; i < num_searches; i++) {
    int pos = dis(gen);
    keys[i] = array[pos];
  }
  return keys;
}

//method from https://github.com/microsoft/ALEX/blob/master/src/benchmark/utils.h
template <class T>
bool load_binary_data(T data[], int length, const std::string& file_path) {
  std::ifstream is(file_path.c_str(), std::ios::binary | std::ios::in);
  if (!is.is_open()) {
    return false;
  }
  is.read(reinterpret_cast<char*>(data), std::streamsize(length * sizeof(T)));
  is.close();
  return true;
}

int main(int argc, char* argv[]) {
  std::string keys_file_path = "resources/uniform_dense_200M_uint64";
  auto init_num_keys = 0;
  auto total_num_keys = 200000000;
  auto batch_size = 25000000;
  auto insert_frac = 0.5;

  auto keys = new KEY_TYPE[total_num_keys];
  load_binary_data(keys, total_num_keys, keys_file_path);

  alex::Alex<KEY_TYPE, PAYLOAD_TYPE> index;

  int i = init_num_keys;
  long long cumulative_inserts = 0;
  long long cumulative_lookups = 0;
  long long cumulative_updates = 0;
  int num_inserts_per_batch = static_cast<int>(batch_size * insert_frac);
  int num_lookups_per_batch = batch_size - num_inserts_per_batch;
  int num_updates_per_batch = 1000000;
  double cumulative_insert_time = 0;
  double cumulative_lookup_time = 0;
  double cumulative_update_time = 0;

  auto workload_start_time = std::chrono::high_resolution_clock::now();
  int batch_number = 0;

  while (1) {
    batch_number++;

    //lookups
    double batch_lookup_time = 0.0;
    if (i > 0) {
      KEY_TYPE* lookup_keys = nullptr;
      lookup_keys = get_search_keys(keys, i, num_lookups_per_batch);

      auto lookups_start_time = std::chrono::high_resolution_clock::now();
      for (int j = 0; j < num_lookups_per_batch; j++) {
        KEY_TYPE key = lookup_keys[j];
        PAYLOAD_TYPE* payload = index.get_payload(key);
        if (payload) {
          int tmp = 1;
        }
      }
      auto lookups_end_time = std::chrono::high_resolution_clock::now();
      batch_lookup_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
                              lookups_end_time - lookups_start_time)
                              .count();
      cumulative_lookup_time += batch_lookup_time;
      cumulative_lookups += num_lookups_per_batch;
      delete[] lookup_keys;
    }

    //updates
    double batch_update_time = 0.0;
    if (i > 0) {
      KEY_TYPE* lookup_keys = nullptr;
      lookup_keys = get_search_keys(keys, i, num_updates_per_batch);

      auto update_start_time = std::chrono::high_resolution_clock::now();
      for (int j = 0; j < num_updates_per_batch; j++) {
        KEY_TYPE key = lookup_keys[j];
        if (index.get_payload(key)) {
          *index.get_payload(key) = rand();
        }
      }
      auto update_end_time = std::chrono::high_resolution_clock::now();
      batch_update_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
                              update_end_time - update_start_time)
                              .count();
      cumulative_update_time += batch_update_time;
      cumulative_updates += num_updates_per_batch;
      delete[] lookup_keys;
    }

    //inserts
    int num_actual_inserts =
        std::min(num_inserts_per_batch, total_num_keys - i);
    int num_keys_after_batch = i + num_actual_inserts;
    auto inserts_start_time = std::chrono::system_clock::now();
    for (; i < num_keys_after_batch; i++) {
      index.insert(keys[i], rand());
    }
    auto inserts_end_time = std::chrono::system_clock::now();
    double batch_insert_time =
        std::chrono::duration_cast<std::chrono::nanoseconds>(inserts_end_time -
                                                             inserts_start_time)
            .count();
    cumulative_insert_time += batch_insert_time;
    cumulative_inserts += num_actual_inserts;

    print_stats_batch(batch_number, num_lookups_per_batch, num_actual_inserts, num_updates_per_batch, batch_lookup_time, batch_insert_time, batch_update_time,
                        cumulative_lookups, cumulative_inserts, cumulative_updates, cumulative_lookup_time, cumulative_insert_time, cumulative_update_time);

    if (i >= 200000000) {
      std::cout << "here" << std::endl;
      break;
    }
  }

  print_stats_cumulative(batch_number, cumulative_lookups, cumulative_inserts, cumulative_updates, 
                          cumulative_lookup_time, cumulative_insert_time, cumulative_update_time);

  delete[] keys;
}

// example of how to insert and update a key
// #include "../core/alex.h"
// #include <iomanip>
// #include <sys/time.h>
// #include <sys/resource.h>
// #include <stdlib.h>
// #include <unistd.h>

// int main(int argc, char* argv[]) {
//   alex::Alex<int, int> index;
//   // index.insert({1,99});
//   // std::cout << *index.get_payload(1) << std::endl;
//   // std::cout << index.model_size() << std::endl;
//   // *index.get_payload(1) = 1001;
//   // std::cout << *index.get_payload(1) << std::endl;

//   return 0;
// }
