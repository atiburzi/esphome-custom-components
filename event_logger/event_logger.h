#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include <vector>
#include <string>

namespace esphome {
namespace event_logger {

class EventLogger : public Component {
 public:
  std::vector<std::string> log_entries_;
  size_t max_entries_ = 10; // Numero massimo di eventi da memorizzare

  void setup() override {
    ESP_LOGCONFIG(TAG, "Setting up Event Logger...");
  }

  void dump_config() override {
    ESP_LOGCONFIG(TAG, "  Max Log Entries: %zu", max_entries_);
  }

  // Modifica qui: add_entry ora accetta la stringa già formattata
  void add_entry(const std::string& entry) {
    log_entries_.push_back(entry);
    if (log_entries_.size() > max_entries_) {
      log_entries_.erase(log_entries_.begin()); // Rimuovi l'elemento più vecchio
    }
    ESP_LOGD(TAG, "Added log entry: %s", entry.c_str());
  }

  void set_max_entries(size_t max_entries) {
    max_entries_ = max_entries;
  }

 protected:
  const char* TAG = "EventLogger";
};

} // namespace event_logger
} // namespace esphome
