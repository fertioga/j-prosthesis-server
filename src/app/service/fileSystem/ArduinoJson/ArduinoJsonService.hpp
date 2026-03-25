#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include "app/service/fileSystem/IFileSystemConfig.hpp"

template <size_t JSON_SIZE>
class ArduinoJsonService: public IFileSystemConfig 
{
    private:
        StaticJsonDocument<JSON_SIZE> doc;
        String filePath;

        bool load()
        {
            if (!LittleFS.exists(filePath)) {
                Serial.println("Arquivo não existe, criando novo...");
                return commit();
            }

            File file = LittleFS.open(filePath, "r");
            if (!file) {
                Serial.println("Erro ao abrir arquivo");
                return false;
            }

            DeserializationError error = deserializeJson(doc, file);
            file.close();

            if (error) {
                Serial.println("Erro ao ler JSON, resetando...");
                doc.clear();
                return commit();
            }

            return true;
        }

        void listFiles()
        {
            File root = LittleFS.open("/");

            File file = root.openNextFile();
            while (file) {
                Serial.print("FILE: ");
                Serial.println(file.name());
                file = root.openNextFile();
            }
        }   

        void printFile(const char* path)
        {
            File file = LittleFS.open(path, "r");

            if (!file) {
                Serial.println("Erro ao abrir arquivo");
                return;
            }

            Serial.println("----- FILE CONTENT -----");

            while (file.available()) {
                Serial.write(file.read());
            }

            Serial.println("\n------------------------");

            file.close();
        }

        template<typename T>
        bool setRaw(const String& varName, T value) {

            JsonVariant current = doc;

            int start = 0;
            while (true) {
                int dot = varName.indexOf('.', start);

                String key;
                if (dot == -1) {
                    key = varName.substring(start);
                    current[key] = value; // ← aqui seta valor final
                    break;
                } else {
                    key = varName.substring(start, dot);
                }

                if (!current[key].is<JsonObject>()) {
                    current[key].to<JsonObject>();
                }

                current = current[key];
                start = dot + 1;
            }

            return true;
        }

        template<typename T>
        T getRaw(const String& path, T defaultValue) {
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

    public:
        bool begin(const String& path = "/config.json")
        {    
            filePath = path;
            
            if (!LittleFS.begin(true)) {
                Serial.println("Erro ao montar LittleFS");
                return false;
            }

            return load();
        }

        bool setInt(const String& varName, int value)
        { 
            return this->setRaw(varName, value); 
        }

        bool setFloat(const String& varName, float value)
        { 
            return this->setRaw(varName, value); 
        }

        bool setString(const String& varName, const String& value)
        { 
            return this->setRaw(varName, value); 
        }

        bool setBool(const String& varName, const bool value)
        { 
            return this->setRaw(varName, value); 
        }
        
        bool commit()
        {
            File file = LittleFS.open(filePath, "w");
            if (!file) {
                Serial.println("Erro ao salvar arquivo");
                return false;
            }

            serializeJsonPretty(doc, file);
            file.close();

            return true;
        }

        int getInt(const String& varName, int defaultValue)
        { 
            return this->getRaw(varName, defaultValue); 
        }

        float getFloat(const String& varName, float defaultValue)
        { 
            return this->getRaw(varName, defaultValue); 
        }

        String getString(const String& varName, const String& defaultValue)
        { 
            return this->getRaw(varName, defaultValue); 
        }

        bool getBool(const String& varName, const bool defaultValue)
        { 
            return this->getRaw(varName, defaultValue); 
        }
};