#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include <vector>
#include <string>

namespace esphome {
namespace event_logger {

class EventLogger : public Component {
 public:
  // Metodi pubblici per l'interfaccia

  // Imposta il numero massimo di voci di log
  void set_max_entries(size_t max_entries) {
    max_entries_ = max_entries;
  }

  // Aggiunge una nuova voce al log
  void add_entry(const std::string& entry) {
    log_entries_.push_back(entry);
    if (log_entries_.size() > max_entries_) {
      log_entries_.erase(log_entries_.begin()); // Rimuovi l'elemento più vecchio
    }
    ESP_LOGD(TAG, "Added log entry: %s", entry.c_str());
  }

  // Restituisce il numero attuale di voci nel log (sostituisce .size())
  size_t get_size() const {
    return log_entries_.size();
  }

  // Restituisce una specifica voce del log per indice (sostituisce log_entries_[i])
  // Usiamo const std::string& per evitare copie inutili e garantire che la voce non venga modificata
  const std::string& get_entry(size_t index) const {
    // Aggiungiamo un controllo per prevenire accessi fuori limite,
    // anche se la logica di scorrimento dovrebbe già gestirlo.
    if (index < log_entries_.size()) {
      return log_entries_[index];
    }
    // In caso di indice non valido, potremmo voler restituire una stringa vuota o gestire l'errore.
    // Per semplicità e compatibilità con l'uso corrente, un crash qui indicherebbe un bug nella logica di scorrimento.
    // Potresti anche lanciare un'eccezione, ma ESPHome di solito preferisce fail-fast o logging.
    // Per ora, ci affidiamo al fatto che index sarà valido se usato con get_size().
    static const std::string EMPTY_STRING = ""; // Stringa vuota statica per sicurezza
    ESP_LOGE(TAG, "Attempted to access out-of-bounds log entry at index %zu", index);
    return EMPTY_STRING; // Restituisce una stringa vuota se l'indice non è valido
  }

  void setup() override {
    ESP_LOGCONFIG(TAG, "Setting up Event Logger...");
  }

  void dump_config() override {
    ESP_LOGCONFIG(TAG, "  Max Log Entries: %zu", max_entries_);
    ESP_LOGCONFIG(TAG, "  Current Log Size: %zu", get_size()); // Ora usa il metodo pubblico
  }

 protected: // Ora la variabile del vettore è protetta (o privata)
  const char* TAG = "EventLogger";
  std::vector<std::string> log_entries_; // Resa privata/protetta
  size_t max_entries_ = 10;
};

} // namespace event_logger
} // namespace esphome