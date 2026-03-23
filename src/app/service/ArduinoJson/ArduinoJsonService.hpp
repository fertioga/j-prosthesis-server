#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

class ArduinoJsonService {
private:
    StaticJsonDocument<2048> doc;
    String filePath;

    bool load();
    bool save();

public:
    ArduinoJsonService(const String& path = "/config.json");

    bool begin();

    /***** SET *****/
    template<typename T>
    bool set(const String& path, T value) {

        JsonVariant current = doc;

        int start = 0;
        while (true) {
            int dot = path.indexOf('.', start);

            String key;
            if (dot == -1) {
                key = path.substring(start);
                current[key] = value; // ← aqui seta valor final
                break;
            } else {
                key = path.substring(start, dot);
            }

            if (!current[key].is<JsonObject>()) {
                current[key].to<JsonObject>();
            }

            current = current[key];
            start = dot + 1;
        }

        return save();

    }

    /***** GET *****/
    template<typename T>
    T get(const String& path, T defaultValue) {
        JsonVariant current = doc;

        int start = 0;
        while (true) {
            int dot = path.indexOf('.', start);

            String key;
            if (dot == -1) {
                key = path.substring(start);

                if (current.containsKey(key)) {
                    return current[key].as<T>();
                }
                
                return defaultValue;
            } else {
                key = path.substring(start, dot);
            }

            if (!current.containsKey(key)) {
                return defaultValue;
            }

            current = current[key];
            start = dot + 1;
        }
    }
};