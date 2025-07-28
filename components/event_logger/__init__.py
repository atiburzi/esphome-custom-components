import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

# Aggiungiamo questo import
import esphome.components.time as time 
# Non servono le action import ancora, se vogliamo isolare

event_logger_ns = cg.esphome_ns.namespace("event_logger")
EventLogger = event_logger_ns.class_("EventLogger", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(EventLogger),
        cv.Optional("max_entries", default=10): cv.All(cv.int_, cv.Range(min=1)),
        # Aggiungiamo time_id allo schema del componente logger
        cv.Required("time_id"): cv.use_id(time.RealTimeClock), 
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if "max_entries" in config:
        cg.add(var.set_max_entries(config["max_entries"]))
    
    # NON AGGIUNGERE ANCORA la logica per set_time_component qui
    # NON AGGIUNGERE ANCORA la registrazione del servizio qui