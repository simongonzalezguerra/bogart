#include "bogart/service/cmd_line_args.hpp"
#include "bogart/async/message_queue.hpp"
#include "bogart/service/system.hpp"
#include "bogart/controller.hpp"
#include "bogart/log/log.hpp"
#include "bogart/view.hpp"

#include <thread>

// Call like this:
// $ cd bogart/build/bogart
// $ ./bogart -width 1920 -height 1080 -fullscreen -content-dir ../../resources
int main(int argc, char** argv) {
  // Parse command line arguments
  bogart::service::cmd_line_args args(argc, argv);

  // Set log level
  if (args.has_option("-debug")) {
    bogart::log::set_log_level(bogart::log::DEBUG);
  }

  // Initialize system
  bogart::service::system system;

  // Create message queues
  bogart::async::message_queue render_queue;
  bogart::async::message_queue logic_queue;

  // View and controller
  bogart::view view(render_queue, logic_queue, system, args);
  bogart::controller controller(logic_queue, view, args);
  controller.async_call();

  // Start the logic thread
  std::thread logic_thread(&bogart::async::message_queue::run, &logic_queue, std::chrono::seconds(1));

  // Run render loop
  render_queue.run(std::chrono::seconds(1));

  logic_thread.join();

  return 0;
}
