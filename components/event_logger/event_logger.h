#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/time/real_time_clock.h"
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

  // Imposta il puntatore al componente time (chiamato una volta all'avvio)
  void set_time_component(time::RealTimeClock *time_component) {
    this->time_component_ = time_component;
  }

  // Aggiunge una nuova voce al log (senza timestamp)
  void add_entry(const std::string& entry) {
    log_entries_.push_back(entry);
    if (log_entries_.size() > max_entries_) {
      log_entries_.erase(log_entries_.begin()); // Rimuove l'elemento più vecchio
    }
    ESP_LOGD(TAG, "Added log entry: %s", entry.c_str());
  }

  // Aggiunge una voce con timestamp automatico
  void add_entry_with_timestamp(const std::string& message) {
    if (this->time_component_ == nullptr) {
      add_entry("Errore: componente tempo non impostato - " + message);
      ESP_LOGE(TAG, "Time component not set for EventLogger!");
      return;
    }

    time::ESPTime now = this->time_component_->now(); // Ottiene l'ora corrente dal componente time
    if (!now.is_valid()) {
      add_entry("Timestamp non valido - " + message);
      return;
    }
    
    char timestamp_buf[20]; // Buffer per il timestamp (es. "HH:MM:SS")
    sprintf(timestamp_buf, "%02d:%02d:%02d", now.hour, now.minute, now.second);
    
    std::string formatted_message = std::string(timestamp_buf) + " - " + message;
    add_entry(formatted_message); // Chiama il metodo add_entry esistente
  }

  // Restituisce il numero attuale di voci nel log
  size_t get_size() const {
    return log_entries_.size();
  }

  // Restituisce una specifica voce del log per indice
  const std::string& get_entry(size_t index) const {
    if (index < log_entries_.size()) {
      return log_entries_[index];
    }
    static const std::string EMPTY_STRING = "";
    ESP_LOGE(TAG, "Attempted to access out-of-bounds log entry at index %zu", index);
    return EMPTY_STRING;
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