#include "awsIotDevice.hpp"


AwsIotDevice::AwsIotDevice(const char* ssid, const char* password, const char* awsEndpoint, uint16_t port, MQTT_CALLBACK_SIGNATURE, const char* cert, const char* key, const char* ca, bool echo) : echo(echo) {


    if(this->echo){
        Serial.print(F("Connecting to ")); 
        Serial.print(ssid);
    }
    
    WiFi.begin(ssid, password);
    WiFi.waitForConnectResult();

    if(this->echo){
        Serial.print(F(", WiFi connected, IP address: ")); 
        Serial.println(WiFi.localIP());
    }

    this->pubSubClient.setClient(wiFiClient);
    this->pubSubClient.setServer(awsEndpoint, port);
    this->pubSubClient.setCallback(callback);
    
    this->client_crt.append(cert);
    this->client_key.parse(key);
    this->rootCert.append(ca);


    this->wiFiClient.setClientRSACert(&client_crt, &client_key);
    this->wiFiClient.setTrustAnchors(&rootCert);
    

    synchronizeTime();
    update();
}

void AwsIotDevice::publish(const char* topic, const char* payload){
    this->pubSubClient.publish(topic, payload);
}
bool AwsIotDevice::subscribe(const char* topic){
    return this->pubSubClient.subscribe(topic);
}
void AwsIotDevice::setCallback(MQTT_CALLBACK_SIGNATURE){
    this->pubSubClient.setCallback(callback);
}

void AwsIotDevice::update(){
    if ( ! pubSubClient.connected()) {
        if(this->echo)
            Serial.print(F("PubSubClient connecting to the aws endpoint"));
        
        String client = "client" + random(0xFFFFF);
        
        while ( ! pubSubClient.connected()) {
            if(this->echo)
                Serial.print(".");

            pubSubClient.connect(client.c_str());
        }
        if(this->echo)
            Serial.println(F(" connected"));

    }
    this->pubSubClient.loop();
}

void AwsIotDevice::synchronizeTime(){
    configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

    if(this->echo)
        Serial.print(F("Waiting for NTP time sync: "));
    
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) {
        delay(500);
        if(echo)
            Serial.print(".");
        now = time(nullptr);
    }
    if(this->echo)
        Serial.println("");
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    
    if(this->echo){
        Serial.print(F("Current time: ")); 
        Serial.print(asctime(&timeinfo));
    }
}
AwsIotDevice::~AwsIotDevice(){
    
}