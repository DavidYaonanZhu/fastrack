/*
 * Copyright (c) 2018, The Regents of the University of California (Regents).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *
 *    3. Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Please contact the author(s) of this library if you have any questions.
 * Authors: David Fridovich-Keil   ( dfk@eecs.berkeley.edu )
 */

///////////////////////////////////////////////////////////////////////////////
//
// Base class for all planners. Planners are templated on state and dynamics
// types **of the planner** NOT **of the tracker,** as well as the tracking
// error bound type and environment. Planners take in start and goal states, and
// start time, and output a trajectory of planner states.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef FASTRACK_PLANNING_PLANNER_H
#define FASTRACK_PLANNING_PLANNER_H

#include <fastrack/environment/environment.h>
#include <fastrack/planning/trajectory.h>
#include <fastrack/utils/types.h>

#include <fastrack_msgs/ReplanRequest.h>

namespace fastrack {
namespace planning {

template< typename S, typename D, typename B, typename E >
class Planner {
public:
  virtual ~Planner() {}

  // Initialize from a ROS NodeHandle.
  bool Initialize(const ros::NodeHandle& n);

protected:
  explicit Planner(const D& dynamics, const B& bound, const E& env)
    : dynamics_(dynamics),
      bound_(bound),
      env_(env) {}

  // Load parameters and register callbacks.
  bool LoadParameters(const ros::NodeHandle& n);
  bool RegisterCallbacks(const ros::NodeHandle& n);

  // Callback to handle replanning requests.
  inline void ReplanRequestCallback(
    const fastrack_msgs::ReplanRequest::ConstPtr& msg) const {
    // Unpack start/stop states.
    const S start(msg->start);
    const S goal(msg->goal);

    // Plan and publish.
    traj_pub_.publish(Plan(start, goal, msg->start_time).ToRos());
  }

  // Plan a trajectory from the given start to goal states starting
  // at the given time.
  virtual Trajectory<S> Plan(
    const S& start, const S& goal, double start_time=0.0) const = 0;

  // Publisher and subscriber.
  ros::Subscriber replan_request_sub_;
  ros::Publisher traj_pub_;

  std::string replan_request_topic_;
  std::string traj_topic_;

  // Keep a copy of the dynamics, tracking bound, and environment.
  const D dynamics_;
  const B bound_;
  E env_;
}; //\class Planner

} //\namespace planning
} //\namespace fastrack

#endif
