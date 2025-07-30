#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include <vector>
#include <string>

namespace esphome {
namespace event_logger {

class EventLogger : public Component {
 public:
  // Imposta il numero massimo di voci di log
  void set_max_entries(size_t max_entries) {
    max_entries_ = max_entries;
  }
  
  // Aggiunge una nuova voce al log
  void add_entry(const std::string& entry) {
    log_entries_.push_back(entry);
    if (log_entries_.size() > max_entries_) {
      log_entries_.erase(log_entries_.begin()); // Rimuove l'elemento più vecchio
    }
    ESP_LOGD(TAG, "Added log entry: %s", entry.c_str());
  }

  // Restituisce il numero attuale di voci nel log
  size_t get_size() const {
    return log_entries_.size();
  }

  // Restituisce una specifica voce del log in ordine inverso (dal più recente al più vecchio)
  const std::string& get_entry(size_t index) const {
    size_t actual_index = log_entries_.size() - 1 - index;
    if (actual_index < log_entries_.size()) { 
      return log_entries_[actual_index];
    }
    static const std::string EMPTY_STRING = "";
    ESP_LOGE(TAG, "Attempted to access out-of-bounds log entry at reversed index %zu", index);
    return EMPTY_STRING;
  }
  
  // Svuota tutte le voci del log
  void clear_entries() {
    log_entries_.clear();
    ESP_LOGD(TAG, "Event log cleared.");
  }    

  void setup() override {
    ESP_LOGCONFIG(TAG, "Setting up Event Logger...");
  }

  void dump_config() override {
    ESP_LOGCONFIG(TAG, "  Max Log Entries: %zu", max_entries_);
    ESP_LOGCONFIG(TAG, "  Current Log Size: %zu", get_size());
  }

 protected:
  const char* TAG = "EventLogger";
  std::vector<std::string> log_entries_; // Mantenuta privata/protetta
  size_t max_entries_ = 10;
  time::RealTimeClock *time_component_{nullptr}; // Puntatore al componente time
};

} // namespace event_logger
} // namespace esphome