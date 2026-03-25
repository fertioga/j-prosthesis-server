#pragma once
#include "bootstrap.hpp"

class IFileSystemConfig
{
    public:
        virtual bool begin(const String& path = "/config.json") = 0;

        virtual bool setInt(const String& varName, int value) = 0;
        virtual bool setFloat(const String& varName, float value) = 0;
        virtual bool setString(const String& varName, const String& value) = 0;
        virtual bool setBool(const String& varName, const bool value) = 0;

        virtual bool commit() = 0;

        virtual int getInt(const String& varName, int defaultValue) = 0;
        virtual float getFloat(const String& varName, float defaultValue) = 0;
        virtual String getString(const String& varName, const String& defaultValue) = 0;
        virtual bool getBool(const String& varName, const bool defaultValue) = 0;

        virtual ~IFileSystemConfig() {}
};