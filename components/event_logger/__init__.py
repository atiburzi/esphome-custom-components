import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

# Importa i tipi necessari dal componente time di ESPHome
import esphome.components.time as time
# NUOVO IMPORTO ESSENZIALE: Per registrare le azioni (servizi)
import esphome.core.automation as automation 

# Definisce il namespace C++ e la classe per il nostro componente
event_logger_ns = cg.esphome_ns.namespace("event_logger")
EventLogger = event_logger_ns.class_("EventLogger", cg.Component)

# Schema di configurazione per l'entry 'event_logger:' nel YAML
CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(EventLogger),
        cv.Optional("max_entries", default=10): cv.All(cv.int_, cv.Range(min=1)),
        # Richiede l'ID del componente time per poter ottenere il timestamp internamente
        cv.Required("time_id"): cv.use_id(time.RealTimeClock), 
    }
).extend(cv.COMPONENT_SCHEMA)

# Funzione to_code: Genera il codice C++ dalla configurazione YAML
async def to_code(config):
    # Crea l'istanza del tuo componente EventLogger in C++
    var = cg.new_Pvariable(config[CONF_ID])
    # Registra il componente ESPHome standard
    await cg.register_component(var, config)

    # Se 'max_entries' è specificato nel YAML, chiama il metodo set_max_entries
    if "max_entries" in config:
        cg.add(var.set_max_entries(config["max_entries"]))
    
    # Ottiene l'istanza del componente time (es. sntp_time)
    # e la passa al tuo EventLogger in C++ tramite set_time_component
    time_component = await cg.get_variable(config["time_id"])
    cg.add(var.set_time_component(time_component))

    # --- Definizione del servizio per add_entry_with_timestamp ---
    # Questo servizio potrà essere chiamato come un'azione 'event_logger.add_timestamped_entry'
    # dalla tua configurazione YAML.
    
    # Aggiunge la direttiva 'using namespace esphome::event_logger;' al codice generato,
    # utile per poter usare i tipi del namespace event_logger in C++
    cg.add_global(event_logger_ns.using) 

    # Schema per i parametri che il servizio accetta
    ADD_TIMESTAMPED_ENTRY_SCHEMA = cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(EventLogger), # L'ID dell'istanza del logger (es. event_log_manager)
            cv.Required("message"): cv.string, # La stringa del messaggio da aggiungere
        }
    )
    # Registra l'azione (il servizio) in ESPHome
    # Questo è il punto in cui abbiamo corretto 'cg.register_action' in 'automation.register_action'
    automation.register_action(
        "event_logger.add_timestamped_entry", # Nome del servizio richiamabile da YAML
        ADD_TIMESTAMPED_ENTRY_SCHEMA,         # Schema dei parametri del servizio
        # Lambda che genera la chiamata al metodo C++:
        # Chiama 'add_entry_with_timestamp' sull'istanza del logger (var),
        # passando solo il 'message'. Il timestamp viene gestito internamente dalla classe C++.
        lambda config: var.add_entry_with_timestamp(config["message"])
    )