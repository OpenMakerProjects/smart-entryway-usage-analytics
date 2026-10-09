# Architecture

Physical BME280 readings select RGB color and OLED values. Requested enable comes from USB ON/OFF or MQTT; actual enabled time only counts when sensor readings are valid. Shared Usage allocates elapsed intervals into24hour/7day ring slots with rollover tests. MQTT→Home Assistant switch→Matterbridge-hass is the explicit Matter path. ESP8266 has no native Matter stack; bridge/radio behavior is untested.
