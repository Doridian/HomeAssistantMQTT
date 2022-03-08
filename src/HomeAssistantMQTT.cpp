#include "HomeAssistantMQTT.h"

#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

HomeAssistantMQTTDevice::HomeAssistantMQTTDevice(
    const String &deviceType,
    const String &name,
    const String &id
) {
    this->name = name;
    this->deviceType = deviceType;
    if (id.isEmpty()) {
        String _id = name;
        _id.toLowerCase();
        _id.replace(" ", "_");
        this->id = _id;
    } else {
        this->id = id;
    }
    this->state = "";

    this->mqttTopicBase = HA_BASE_TOPIC + this->deviceType + "/" + this->id;

    this->configDoc["~"] = this->mqttTopicBase;
    this->configDoc["name"] = this->name;
    this->configDoc["unique_id"] = this->id;
    this->configDoc["cmd_t"] = "~/" HA_COMMAND_SUFFIX;
    this->configDoc["stat_t"] = "~/" HA_STATE_SUFFIX;
    this->configDoc["json_attr_t"] = "~/" HA_ATTRIBUTES_SUFFIX;

    this->refresh();
}

HomeAssistantMQTTDevice::~HomeAssistantMQTTDevice() {
}

void HomeAssistantMQTTDevice::refresh() {
    this->configDirty = true;
    this->stateDirty = true;
    this->attributesDirty = true;
}

const String HomeAssistantMQTTDevice::getMQTTTopic(const String &topic) const {
    return this->mqttTopicBase + "/" + topic;
}

const String HomeAssistantMQTTDevice::getMQTTCommandTopic() const {
    return this->getMQTTTopic(HA_COMMAND_SUFFIX);
}

void HomeAssistantMQTTDevice::loop(PubSubClient &mqtt) {
    const unsigned long now = millis();
    char buf[HA_JSON_MAX_SIZE];

    if (this->configDirty) {
        serializeJson(this->configDoc, buf);
        mqtt.publish(this->getMQTTTopic(HA_CONFIG_SUFFIX).c_str(), buf);
        this->configDirty = false;
        this->lastConfigUpdate = now;
    }

    if (now - this->lastConfigUpdate < HA_WAIT_BETWEEN_CONFIG_AND_DATA) {
        return;
    }

    if (this->attributesDirty) {
        serializeJson(this->attributesDoc, buf);
        mqtt.publish(this->getMQTTTopic(HA_ATTRIBUTES_SUFFIX).c_str(), buf);
        this->attributesDirty = false;
    }

    if (this->stateDirty) {
        mqtt.publish(this->getMQTTTopic(HA_STATE_SUFFIX).c_str(), this->state.c_str());
        this->stateDirty = false;
    }
}

void HomeAssistantMQTTDevice::setState(const String &state) {
    this->state = state;
    this->stateDirty = true;
}
