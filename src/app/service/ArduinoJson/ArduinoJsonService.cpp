#include "ArduinoJsonService.hpp"

ArduinoJsonService::ArduinoJsonService(const String& path) {
    filePath = path;
}

bool ArduinoJsonService::begin() {
    if (!LittleFS.begin(true)) {
        Serial.println("Erro ao montar LittleFS");
        return false;
    }

    return load();
}

bool ArduinoJsonService::load() {
    if (!LittleFS.exists(filePath)) {
        Serial.println("Arquivo não existe, criando novo...");
        return save();
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
        return save();
    }

    return true;
}

bool ArduinoJsonService::save() {
    File file = LittleFS.open(filePath, "w");
    if (!file) {
        Serial.println("Erro ao salvar arquivo");
        return false;
    }

    serializeJsonPretty(doc, file);
    file.close();

    return true;
}