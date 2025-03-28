# SourceCode for Today's TBB: C++ Parallel Programming with Threading Building Blocks, Second Edition

Source code of the examples provided in each chapter of the new TBB book. Coming soon in April 2025!


## Prerequisites

   - Make sure you have installed CMake version 3.4 (or newer) on your system. These examples use CMake build configuration
   - TBB, which is needed of course, can be installed [from source](https://github.com/uxlfoundation/oneTBB) or as part of a [oneAPI tookit](https://www.intel.com/content/www/us/en/developer/tools/oneapi/overview.html)

## Buildin

To configure, at the command prompt, type

```cpp
cmake <options> <repo_root
```

To build the examples, run

```
cmake --build .
```

## Figure Decoder

We have organized this repository by topic instead of by chapter and some figures in the book are combined into a single
example source file. The table below can be used to map book figure numbers to their corresponding source files.

| Chapter | Figure Number | Source File  |
| ------- | ------------- | -----------  |
|	1	|	1	| [	intro/intro_pi.cpp	](	intro/intro_pi.cpp	) |
|	1	|	4	| [	intro/intro_helloTBB.cpp	](	intro/intro_helloTBB.cpp	) |
|	1	|	7	| [	intro/intro_gamma.cpp	](	intro/intro_gamma.cpp	) |
|	1	|	10	| [	intro/intro_flowgraph.cpp	](	intro/intro_flowgraph.cpp	) |
|	1	|	11	| [	intro/intro_parallel_for.cpp	](	intro/intro_parallel_for.cpp	) |
|	1	|	12	| [	intro/intro_parallel_for_transform.cpp	](	intro/intro_parallel_for_transform.cpp	) |
|	2	|	4	| [	algorithms/parallel_invoke_two_quicksorts.cpp	](	algorithms/parallel_invoke_two_quicksorts.cpp	) |
|	2	|	6	| [	algorithms/parallel_for_unoptimized_mxm.cpp	](	algorithms/parallel_for_unoptimized_mxm.cpp	) |
|	2	|	7	| [	algorithms/parallel_for_unoptimized_mxm.cpp	](	algorithms/parallel_for_unoptimized_mxm.cpp	) |
|	2	|	9	| [	algorithms/parallel_for_each_primes.cpp	](	algorithms/parallel_for_each_primes.cpp	) |
|	2	|	10	| [	algorithms/parallel_for_each_primes.cpp	](	algorithms/parallel_for_each_primes.cpp	) |
|	2	|	11	| [	algorithms/parallel_for_each_primes.cpp	](	algorithms/parallel_for_each_primes.cpp	) |
|	2	|	12	| [	algorithms/parallel_for_each_primes.cpp	](	algorithms/parallel_for_each_primes.cpp	) |
|	2	|	13	| [	algorithms/parallel_for_each_fwd_substitution.cpp	](	algorithms/parallel_for_each_fwd_substitution.cpp	) |
|	2	|	15	| [	algorithms/parallel_for_each_fwd_substitution.cpp	](	algorithms/parallel_for_each_fwd_substitution.cpp ) |
|	2	|	16	| [	algorithms/parallel_for_each_fwd_substitution.cpp	](	algorithms/parallel_for_each_fwd_substitution.cpp	) |
|	2	|	17	| [	algorithms/parallel_invoke_two_quicksorts.cpp	](	algorithms/parallel_invoke_two_quicksorts.cpp	) |
|	2	|	18	| [	algorithms/parallel_invoke_recursive_quicksort.cpp	](	algorithms/parallel_invoke_recursive_quicksort.cpp	) |
|	2	|	21	| [	algorithms/parallel_reduce_max.cpp	](	algorithms/parallel_reduce_max.cpp	) |
|	2	|	22	| [	algorithms/parallel_reduce_pi.cpp	](	algorithms/parallel_reduce_pi.cpp	) |
|	2	|	23	| [	algorithms/parallel_reduce_pi.cpp	](	algorithms/parallel_reduce_pi.cpp	) |
|	2	|	24	| [	algorithms/serial_running_sum.cpp	](	algorithms/serial_running_sum.cpp	) |
|	2	|	27	| [	algorithms/parallel_scan_running_sum.cpp	](	algorithms/parallel_scan_running_sum.cpp	) |
|	2	|	28	| [	algorithms/parallel_scan_line_of_sight.cpp	](	algorithms/parallel_scan_line_of_sight.cpp	) |
|	2	|	29	| [	algorithms/parallel_scan_line_of_sight.cpp	](	algorithms/parallel_scan_line_of_sight.cpp	) |
|	2	|	30	| [	algorithms/parallel_pipeline_case.cpp	](	algorithms/parallel_pipeline_case.cpp	) |
|	2	|	34	| [	algorithms/parallel_pipeline_case.cpp	](	algorithms/parallel_pipeline_case.cpp	) |
|	2	|	35	| [	algorithms/3Dstereo_serial_no_pipeline.cpp	](	algorithms/3Dstereo_serial_no_pipeline.cpp	) |
|	2	|	37	| [	algorithms/3Dstereo_parallel_pipeline.cpp	](	algorithms/3Dstereo_parallel_pipeline.cpp	) |
|	2	|	38	| [	algorithms/parallel_sort.cpp	](	algorithms/parallel_sort.cpp	) |
|	3	|	1	| [	containers/pop_danger.cpp ](	containers/pop_danger.cpp ) and [pop_danger_fixed.cpp](containers/pop_danger_fixed.cpp) |
|	3	|	3	| [	containers/concurrent_hash_maps.cpp	](	containers/concurrent_hash_maps.cpp	) |
|	3	|	4	| [	containers/concurrent_hash_maps.cpp	](	containers/concurrent_hash_maps.cpp	) |
|	3	|	6	| [	containers/concurrent_hash_maps.cpp	](	containers/concurrent_hash_maps.cpp	) |
|	3	|	7	| [	containers/concurrent_maps.cpp	](	containers/concurrent_maps.cpp	) |
|	3	|	8	| [	containers/concurrent_queues.cpp	](	containers/concurrent_queues.cpp	) |
|	3	|	9	| [	containers/concurrent_queues.cpp	](	containers/concurrent_queues.cpp	) |
|	3	|	10	| [	containers/concurrent_queues.cpp	](	containers/concurrent_queues.cpp	) |
|	3	|	11	| [	containers/debugging_queue.cpp	](	containers/debugging_queue.cpp	) |
|	3	|	12	| [	containers/concurrent_vectors.cpp	](	containers/concurrent_vectors.cpp	) |
|	4	|	3	| [	graph/graph_two_nodes.cpp	](	graph/graph_two_nodes.cpp	) |
|	4	|	6	| [	graph/graph_two_nodes_deduced.cpp	](	graph/graph_two_nodes_deduced.cpp	) |
|	4	|	9	| [	graph/graph_with_join.cpp	](	graph/graph_with_join.cpp	) |
|	4	|	11	| [	graph/graph_loops.cpp	](	graph/graph_loops.cpp	) |
|	4	|	13	| [	graph/graph_stereoscopic_3d.cpp	](	graph/graph_stereoscopic_3d.cpp	) |
|	4	|	14	| [	graph/graph_stereoscopic_3d.cpp	](	graph/graph_stereoscopic_3d.cpp	) |
|	4	|	15	| [	graph/graph_fwd_substitution.cpp	](	graph/graph_fwd_substitution.cpp	) |
|	4	|	16	| [	graph/graph_fwd_substitution.cpp	](	graph/graph_fwd_substitution.cpp	) |
|	4	|	17	| [	graph/graph_fwd_substitution.cpp	](	graph/graph_fwd_substitution.cpp	) |
|	5	|	1	| [	graph/graph_limiting_messages.cpp	](	graph/graph_limiting_messages.cpp	) |
|	5	|	3	| [	graph/graph_limiting_messages.cpp	](	graph/graph_limiting_messages.cpp	) |
|	5	|	5	| [	graph/graph_limiting_messages.cpp	](	graph/graph_limiting_messages.cpp	) |
|	5	|	6	| [	graph/graph_small_nodes.cpp	](	graph/graph_small_nodes.cpp	) |
|	5	|	8	| [	graph/graph_node_priorities.cpp	](	graph/graph_node_priorities.cpp	) |
|	5	|	9	| [	graph/graph_node_priorities.cpp	](	graph/graph_node_priorities.cpp	) |
|	5	|	11	| [	graph/graph_execute_while_building.cpp	](	graph/graph_execute_while_building.cpp	) |
|	5	|	12	| [	graph/graph_reestablish_order.cpp	](	graph/graph_reestablish_order.cpp	) |
|	5	|	13	| [	graph/graph_reestablish_order.cpp	](	graph/graph_reestablish_order.cpp	) |
|	5	|	15	| [	graph/graph_composite_node.cpp	](	graph/graph_composite_node.cpp	) |
|	5	|	16	| [	graph/graph_composite_node.cpp	](	graph/graph_composite_node.cpp	) |
|	5	|	18	| [	graph/graph_async_sycl.cpp	](	graph/graph_async_sycl.cpp	) |
|	5	|	19	| [	graph/graph_async_sycl.cpp	](	graph/graph_async_sycl.cpp	) |
|	6	|	1	| [	tasks/parallel_invoke_fib.cpp	](	tasks/parallel_invoke_fib.cpp	) |
|	6	|	3	| [	tasks/parallel_invoke_fib.cpp	](	tasks/parallel_invoke_fib.cpp	) |
|	6	|	4	| [	tasks/parallel_invoke_fib.cpp	](	tasks/parallel_invoke_fib.cpp	) |
|	6	|	7	| [	tasks/task_group_fib.cpp	](	tasks/task_group_fib.cpp	) |
|	6	|	8	| [	tasks/task_group_poor_scaling.cpp	](	tasks/task_group_poor_scaling.cpp	) |
|	6	|	9	| [	tasks/task_group_poor_scaling.cpp	](	tasks/task_group_poor_scaling.cpp	) |
|	6	|	10	| [	tasks/task_group_fib_defer.cpp	](	tasks/task_group_fib_defer.cpp	) |
|	6	|	11	| [	tasks/task_group_with_dependencies.cpp	](	tasks/task_group_with_dependencies.cpp	) |
|	6	|	12	| [	tasks/task_group_with_dependencies.cpp	](	tasks/task_group_with_dependencies.cpp	) |
|	6	|	13	| [	tasks/resumable_tasks.cpp	](	tasks/resumable_tasks.cpp	) |
|	7	|	2	| [	synchronization/histogram_07_3rd_safe_parallel_cache_aligned.cpp	](	synchronization/histogram_07_3rd_safe_parallel_cache_aligned.cpp	) |
|	7	|	3	| [	synchronization/histogram_07_3rd_safe_parallel_cache_aligned.cpp	](	synchronization/histogram_07_3rd_safe_parallel_cache_aligned.cpp	) |
|	7	|	7	| [	memalloc/windows_proxy.cpp	](	memalloc/windows_proxy.cpp	) |
|	7	|	8	| [	memalloc/tbb_mem.cpp	](	memalloc/tbb_mem.cpp	) |
|	7	|	13	| [	memalloc/replace_new_n_delete.cpp	](	memalloc/replace_new_n_delete.cpp	) |
|	8	|	3	| [	synchronization/histogram_01_sequential.cpp	](	synchronization/histogram_01_sequential.cpp	) |
|	8	|	4	| [	synchronization/histogram_02_unsafe_parallel.cpp	](	synchronization/histogram_02_unsafe_parallel.cpp	) |
|	8	|	8	| [	synchronization/histogram_03_1st_safe_parallel.cpp	](	synchronization/histogram_03_1st_safe_parallel.cpp	) |
|	8	|	9	| [	synchronization/histogram_03_1st_safe_parallel.cpp	](	synchronization/histogram_03_1st_safe_parallel.cpp	) |
|	8	|	14	| [	synchronization/histogram_04_2nd_safe_parallel.cpp	](	synchronization/histogram_04_2nd_safe_parallel.cpp	) |
|	8	|	19	| [	synchronization/histogram_05_fetch_and_triple.cpp	](	synchronization/histogram_05_fetch_and_triple.cpp	) |
|	8	|	20	| [	synchronization/histogram_06_3rd_safe_parallel.cpp	](	synchronization/histogram_06_3rd_safe_parallel.cpp	) |
|	8	|	22	| [	synchronization/histogram_08_4th_safe_parallel_private.cpp	](	synchronization/histogram_08_4th_safe_parallel_private.cpp	) |
|	8	|	23	| [	synchronization/histogram_09_5th_safe_parallel_private.cpp	](	synchronization/histogram_09_5th_safe_parallel_private.cpp	) |
|	8	|	24	| [	synchronization/histogram_10_6th_safe_parallel_private.cpp	](	synchronization/histogram_10_6th_safe_parallel_private.cpp	) |
|	8	|	25	| [	synchronization/histogram_11_7th_safe_parallel_combine.cpp	](	synchronization/histogram_11_7th_safe_parallel_combine.cpp	) |
|	8	|	26	| [	synchronization/histogram_12_8th_safe_parallel_combine.cpp	](	synchronization/histogram_12_8th_safe_parallel_combine.cpp	) |
|	8	|	27	| [	synchronization/histogram_13_9th_safe_parallel_reduction.cpp	](	synchronization/histogram_13_9th_safe_parallel_reduction.cpp	) |
|	8	|	30	| [	synchronization/laurel_and_hardy.cpp	](	synchronization/laurel_and_hardy.cpp	) |
|	9	|	1	| [	cancellation/cancel_group_execution1.cpp	](	cancellation/cancel_group_execution1.cpp	) |
|	9	|	2	| [	cancellation/cancel_group_execution2.cpp	](	cancellation/cancel_group_execution2.cpp	) |
|	9	|	3	| [	cancellation/cancel_group_execution3.cpp	](	cancellation/cancel_group_execution3.cpp	) |
|	9	|	6	| [	cancellation/cancel_group_execution4.cpp	](	cancellation/cancel_group_execution4.cpp	) |
|	9	|	9	| [	exception/exception_catch1.cpp	](	exception/exception_catch1.cpp	) |
|	11	|	4	| [	performance_tuning/global_control_and_implicit_arena.cpp	](	performance_tuning/global_control_and_implicit_arena.cpp	) |
|	11	|	5	| [	performance_tuning/global_control_and_implicit_arena.cpp	](	performance_tuning/global_control_and_implicit_arena.cpp	) |
|	11	|	9	| [	performance_tuning/global_control_and_implicit_conflict.cpp	](	performance_tuning/global_control_and_implicit_conflict.cpp	) |
|	11	|	10	| [	performance_tuning/global_control_and_implicit_conflict.cpp	](	performance_tuning/global_control_and_implicit_conflict.cpp	) |
|	11	|	12	| [	performance_tuning/global_control_and_explicit_arena.cpp	](	performance_tuning/global_control_and_explicit_arena.cpp	) |
|	11	|	13	| [	performance_tuning/global_control_and_explicit_conflict.cpp	](	performance_tuning/global_control_and_explicit_conflict.cpp	) |
|	11	|	15	| [	performance_tuning/priorities_and_conflict.cpp	](	performance_tuning/priorities_and_conflict.cpp	) |
|	11	|	16	| [	performance_tuning/priorities_and_conflict.cpp	](	performance_tuning/priorities_and_conflict.cpp	) |
|	11	|	19	| [	performance_tuning/constraints.cpp	](	performance_tuning/constraints.cpp	) |
|	11	|	20	| [	performance_tuning/constraints.cpp	](	performance_tuning/constraints.cpp	) |
|	11	|	21	| [	performance_tuning/constraints.cpp	](	performance_tuning/constraints.cpp	) |
|	11	|	22	| [	performance_tuning/constraints.cpp	](	performance_tuning/constraints.cpp	) |
|	11	|	24	| [	performance_tuning/task_scheduler_observer.cpp	](	performance_tuning/task_scheduler_observer.cpp	) |
|	11	|	25 | [	performance_tuning/task_scheduler_observer.cpp	](	performance_tuning/task_scheduler_observer.cpp	) |
|	11	|	29	| [	performance_tuning/parallel_for_partitioners_timed.cpp	](	performance_tuning/parallel_for_partitioners_timed.cpp	) |
|	11	|	34	| [	performance_tuning/parallel_for_transpose_partitioners.cpp	](	performance_tuning/parallel_for_transpose_partitioners.cpp	) |
|	11	|	36	| [	performance_tuning/parallel_for_transpose_partitioners.cpp	](	performance_tuning/parallel_for_transpose_partitioners.cpp	) |
|	11	|	39	| [	performance_tuning/parallel_for_addition_partitioners.cpp	](	performance_tuning/parallel_for_addition_partitioners.cpp	) |
|	11	|	41	| [	performance_tuning/partitioners_imbalanced_loops.cpp	](	performance_tuning/partitioners_imbalanced_loops.cpp	) |
|	12	|	1	| [	migration/migrate_atomics.cpp		](	migration/migrate_atomics.cpp	) |
|	12	|	2	| [	migration/migrate_task_scheduler_init.cpp	](	migration/migrate_task_scheduler_init.cpp	) |
|	12	|	4	| [	migration/migrate_parallel_do.cpp	](	migration/migrate_parallel_do.cpp	) |
|	12	|	5	| [	migration/migrate_priorities.cpp	](	migration/migrate_priorities.cpp	) |
|	12	|	6	| [	migration/migrate_priorities.cpp	](	migration/migrate_priorities.cpp	) |
|	12	|	7	| [	migration/migrate_priorities.cpp	](	migration/migrate_priorities.cpp	) |
|	12	|	8	| [	migration/migrate_task_blocking.cpp	](	migration/migrate_task_blocking.cpp	) |
|	12	|	9	| [	migration/migrate_task_blocking.cpp	](	migration/migrate_task_blocking.cpp	) |
|	12	|	10	| [	migration/migrate_tasks_adding_work.cpp	](	migration/migrate_tasks_adding_work.cpp	) |
|	12	|	11	| [	migration/migrate_tasks_adding_work.cpp	](	migration/migrate_tasks_adding_work.cpp	) |
|	12	|	12	| [	migration/migrate_tasks_adding_work.cpp	](	migration/migrate_tasks_adding_work.cpp	) |
|	12	|	13	| [	migration/migrate_tasks_adding_work.cpp	](	migration/migrate_tasks_adding_work.cpp	) |
|	12	|	14	| [	migration/migrate_tasks_adding_work.cpp	](	migration/migrate_tasks_adding_work.cpp	) |
|	12	|	15	| [	migration/migrate_recycling_tasks.cpp	](	migration/migrate_recycling_tasks.cpp	) |
|	12	|	16	| [	migration/migrate_recycling_tasks.cpp	](	migration/migrate_recycling_tasks.cpp	) |
|	12	|	17	| [	migration/migrate_recycling_tasks.cpp	](	migration/migrate_recycling_tasks.cpp	) |
|	12	|	18	| [	migration/migrate_bypass_tasks.cpp	](	migration/migrate_bypass_tasks.cpp	) |
|	12	|	19	| [	migration/migrate_bypass_tasks.cpp	](	migration/migrate_bypass_tasks.cpp	) |
|	12	|	20	| [	migration/migrate_bypass_tasks.cpp	](	migration/migrate_bypass_tasks.cpp	) |

Feel free to leave comments/suggestions/feedback.

Mike and James

