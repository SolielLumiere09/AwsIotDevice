#pragma once

#include <ESP8266WiFi.h> //esp8266 core
#include <PubSubClient.h> //Pubsubclient dependency


class AwsIotDevice {

    private:
        BearSSL::X509List client_crt; //The device Certificate from the aws iot thing
        BearSSL::PrivateKey client_key; //The device private key from the aws iot thing
        BearSSL::X509List rootCert; // the trusted certificate emited by Amazon
        WiFiClientSecure wiFiClient; // wificlient
        PubSubClient pubSubClient; // mqtt client
        bool echo; //echo if you want logs
        void synchronizeTime(); //syncronize time in order to the certificates works

    public:
        AwsIotDevice(const char* ssid, const char* password, const char* awsEndpoint, uint16 port, MQTT_CALLBACK_SIGNATURE , const char* cert, const char* key, const char* ca, bool echo = true);
        void publish(const char* topic, const char* payload);
        bool subscribe(const char* topic);
        void setCallback(MQTT_CALLBACK_SIGNATURE);
        void update();
        ~AwsIotDevice();
};