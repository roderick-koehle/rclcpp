/* Copyright 2014 Open Source Robotics Foundation, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RCLCPP_RCLCPP_SUBSCRIPTION_HPP_
#define RCLCPP_RCLCPP_SUBSCRIPTION_HPP_

#include <functional>
#include <memory>
#include <string>

#include <rmw/rmw.h>

#include <rclcpp/macros.hpp>

namespace rclcpp
{

// Forward declaration is for friend statement in SubscriptionBase
namespace executor {class Executor;}

namespace subscription
{

class SubscriptionBase
{
  friend class rclcpp::executor::Executor;
public:
  RCLCPP_MAKE_SHARED_DEFINITIONS(SubscriptionBase);

  SubscriptionBase(
    rmw_subscription_t * subscription_handle,
    std::string &topic_name)
    : subscription_handle_(subscription_handle), topic_name_(topic_name)
  {}

  std::string get_topic_name()
  {
    return this->topic_name_;
  }

  virtual std::shared_ptr<void> create_message() = 0;
  virtual void handle_message(std::shared_ptr<void> &message) = 0;

private:
  RCLCPP_DISABLE_COPY(SubscriptionBase);

  rmw_subscription_t * subscription_handle_;
  std::string topic_name_;

};

template<typename MessageT>
class Subscription : public SubscriptionBase
{
public:
  typedef std::function<void(const std::shared_ptr<MessageT> &)> CallbackType;
  RCLCPP_MAKE_SHARED_DEFINITIONS(Subscription);

  Subscription(rmw_subscription_t * subscription_handle,
               std::string &topic_name,
               CallbackType callback)
    : SubscriptionBase(subscription_handle, topic_name), callback_(callback)
  {}

  std::shared_ptr<void> create_message()
  {
    return std::shared_ptr<void>(new MessageT());
  }

  void handle_message(std::shared_ptr<void> &message)
  {
    auto typed_message = std::static_pointer_cast<MessageT>(message);
    callback_(typed_message);
  }

private:
  RCLCPP_DISABLE_COPY(Subscription);

  CallbackType callback_;

};

} /* namespace subscription */
} /* namespace rclcpp */

#endif /* RCLCPP_RCLCPP_SUBSCRIPTION_HPP_ */