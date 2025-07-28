import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

# Importa i tipi necessari dal componente time di ESPHome
import esphome.components.time as time

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
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if "max_entries" in config:
        cg.add(var.set_max_entries(config["max_entries"]))
    
    # Ottiene l'istanza del componente time e la passa al logger
    time_component = await cg.get_variable(config["time_id"])
    cg.add(var.set_time_component(time_component))

    # --- Definizione del servizio per add_entry_with_timestamp ---
    # Questo servizio sarà chiamabile come un'azione 'event_logger.add_timestamped_entry'
    # dalla tua configurazione YAML.
    cg.add_global(event_logger_ns.using) # Necessario per usare i tipi del namespace event_logger in C++

    ADD_TIMESTAMPED_ENTRY_SCHEMA = cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(EventLogger), # L'ID dell'istanza del logger
            cv.Required("message"): cv.string, # La stringa del messaggio da aggiungere
        }
    )
    # Registra il servizio
    cg.register_action(
        "event_logger.add_timestamped_entry",
        ADD_TIMESTAMPED_ENTRY_SCHEMA,
        # La funzione C++ da chiamare: add_entry_with_timestamp sull'istanza 'var'
        # Passa solo il messaggio, il timestamp viene preso internamente dalla classe.
        lambda config: var.add_entry_with_timestamp(config["message"])
    )