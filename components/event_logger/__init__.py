import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

# Define the C++ namespace and class for our component
# This should match the namespace and class name in event_logger.h
event_logger_ns = cg.esphome_ns.namespace("event_logger")
EventLogger = event_logger_ns.class_("EventLogger", cg.Component)

# Configuration schema for the 'event_logger:' entry in YAML
CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(EventLogger),
        cv.Optional("max_entries", default=10): cv.All(cv.int_, cv.Range(min=1)),
    }
).extend(cv.COMPONENT_SCHEMA)

# To_code function: This tells ESPHome how to generate C++ code from the YAML config
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if "max_entries" in config:
        cg.add(var.set_max_entries(config["max_entries"]))