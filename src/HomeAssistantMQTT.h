#ifndef HOME_ASSISTANT_MQTT_H
#define HOME_ASSISTANT_MQTT_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define HA_JSON_MAX_SIZE 1024
#define HA_WAIT_BETWEEN_CONFIG_AND_DATA 5000
#define HA_BASE_TOPIC "homeassistant/"

#define HA_STATE_SUFFIX "state"
#define HA_ATTRIBUTES_SUFFIX "attr"
#define HA_CONFIG_SUFFIX "config"
#define HA_COMMAND_SUFFIX "cmd"
#define HA_STATUS_TOPIC (HA_BASE_TOPIC "status")

class HomeAssistantMQTTDevice
{
    public:
        HomeAssistantMQTTDevice(
            const String &deviceType,
            const String &name,
            const String &id = ""
        );
        ~HomeAssistantMQTTDevice();

        void loop(PubSubClient &mqtt);
        void command(const String &command);
        void refresh();

        void setState(const String &state);

        template <typename T>
        void setAttribute(const String& name, const T& src) {
            this->attributesDoc[name] = src;
            this->attributesDirty = true;
        }

        template <typename T>
        void setConfig(const String& name, const T& src) {
            this->configDoc[name] = src;
            this->configDirty = true;
        }

        const String getMQTTTopic(const String &topic) const;
        const String getMQTTCommandTopic() const;

    protected:
        bool stateDirty;
        bool attributesDirty;
        bool configDirty;

        String id;
        String name;
        String deviceType;
        String mqttTopicBase;
        String state;

        unsigned long lastConfigUpdate;
    
        StaticJsonDocument<HA_JSON_MAX_SIZE> configDoc;
        StaticJsonDocument<HA_JSON_MAX_SIZE> attributesDoc;
};

#endif
