// Copyright 2015 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef RCL__NODE_H_
#define RCL__NODE_H_

#if __cplusplus
extern "C"
{
#endif

#include "rcl/allocator.h"
#include "rcl/types.h"

struct rcl_node_impl_t;

/// Handle for a ROS node.
/* This handle can be in one a few stats, which are referred to below:
 * - uninitialized: memory allocated, but not initialized to anything
 * - invalid: memory allocated but not usable
 *   - either zero initialized or shutdown (rcl_fini() or rcl_node_fini())
 * - valid: memory allocated and usable
 */
typedef struct rcl_node_t {
  /// Private implementation pointer.
  struct rcl_node_impl_t * impl;
} rcl_node_t;

typedef struct rcl_node_options_t {
  // bool anonymous_name;
  // rmw_qos_profile_t parameter_qos;
  /// If true, no parameter infrastructure will be setup.
  bool no_parameters;
  /// Custom allocator used for incidental allocations, e.g. node name string.
  rcl_allocator_t allocator;
} rcl_node_options_t;

/// Return a rcl_node_t struct with members initialized to NULL.
/* Should be called to get rcl_node_t before passing to rcl_node_init().
 * It's also possible to use calloc() instead of this if the rcl_node is being
 * allocated on the heap.
 */
rcl_node_t
rcl_get_zero_initialized_node();

/// Initialize a ROS node.
/* Calling this on a rcl_node_t makes it a valid node handle until rcl_fini
 * is called or until rcl_node_fini is called on it.
 *
 * After calling the ROS node object can be used to create other middleware
 * primitives like publishers, services, parameters, and etc.
 *
 * The name of the node cannot coincide with another node of the same name.
 * If a node of the same name is already in the domain, it will be shutdown.
 *
 * A node contains infrastructure for ROS parameters, which include advertising
 * publishers and service servers.
 * So this function will create those external parameter interfaces even if
 * parameters are not used later.
 *
 * This function should be called on a rcl_node_t exactly once, and calling it
 * multiple times is undefined behavior.
 *
 * Expected usage:
 *
 *    rcl_node_t node = rcl_get_zero_initialized_node();
 *    rcl_node_options_t * node_ops = rcl_node_get_default_options();
 *    rcl_ret_t ret = rcl_node_init(&node, "node_name", node_ops);
 *    // ... error handling and then use the node, but finally deinitialize it:
 *    ret = rcl_node_fini(&node);
 *    // ... error handling for rcl_node_fini()
 *
 * This function is not thread-safe.
 *
 * \pre the node handle must be allocated, zero initialized, and invalid
 * \post the node handle is valid and can be used to in other rcl_* functions
 *
 * \param[inout] node a preallocated node structure
 * \param[in] name the name of the node
 * \param[in] options the node options; pass null to use default options
 * \return RMW_RET_OK if node was initialized successfully, otherwise RMW_RET_ERROR
 */
rcl_ret_t
rcl_node_init(rcl_node_t * node, const char * name, const rcl_node_options_t * options);

/// Deinitialize a rcl_node_t.
/* Shuts down any automatically created infrastructure and deallocates memory.
 * After calling, the rcl_node_t can be safely released.
 *
 * Any middleware primitives created by the user, e.g. publishers, services, etc.,
 * are invalid after deinitialization.
 *
 * This function is not thread-safe.
 *
 * \param[in] node handle to the node to be deinitialized
 * \return RMW_RET_OK if node was deinitialized successfully, otherwise RMW_RET_ERROR
 */
rcl_ret_t
rcl_node_fini(rcl_node_t * node);

/// Return the default node options in a rcl_node_options_t.
rcl_node_options_t
rcl_node_get_default_options();

/// Get the name of the node.
/* This function returns the node's internal name string.
 * This function can fail, and therefore return NULL, if:
 *   - node is NULL
 *   - node has not been initialized (the implementation is invalid)
 *
 * The returned string is only valid as long as the given rcl_node_t is valid.
 * The value of the string may change if the value in the rcl_node_t changes,
 * and therefore copying the string is recommended if this is a concern.
 *
 * \param[in] node pointer to the node
 * \return name string if successful, otherwise NULL
 */
const char *
rcl_node_get_name(const rcl_node_t * node);

/// Return the rcl node options.
/* This function returns the node's internal options struct.
 * This function can fail, and therefore return NULL, if:
 *   - node is NULL
 *   - node has not been initialized (the implementation is invalid)
 *
 * The returned struct is only valid as long as the given rcl_node_t is valid.
 * The values in the struct may change if the options of the rcl_node_t changes,
 * and therefore copying the struct is recommended if this is a concern.
 *
 * \param[in] node pointer to the node
 * \return options struct if successful, otherwise NULL
 */
const rcl_node_options_t *
rcl_node_get_options(const rcl_node_t * node);

/// Return the rmw node handle.
/* The handle returned is a pointer to the internally held rmw handle.
 * This function can fail, and therefore return NULL, if:
 *   - node is NULL
 *   - node has not been initialized (the implementation is invalid)
 *
 * The returned handle is only valid as long as the given node is valid.
 *
 * \TODO(wjwwood) should the return value of this be const?
 *
 * \param[in] node pointer to the rcl node
 * \return rmw node handle if successful, otherwise NULL
 */
rmw_node_t *
rcl_node_get_rmw_node_handle(const rcl_node_t * node);

#if __cplusplus
}
#endif

#endif  // RCL__NODE_H_
