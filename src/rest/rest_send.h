#pragma once


#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include "WiFiClientFix.h"
#include <HTTPClient.h>
#include <Ticker.h>
#include <ArduinoJson.hpp>


#define FULL_CONFIG_PUSH_DELAY      (5 * 60)    // 5 minute delay on pushing a "full config" to the endpoint
// TODO - Make LCD_PUSH_DELAY configurable
#define LCD_PUSH_DELAY              (25)        // 25 second delay on pushing LCD data to the endpoint
#define REGISTER_DEVICE_DELAY       (3 * 60)    // 5 minute delay on reattempting 


namespace UpstreamAPIEndpoints {
    constexpr auto fullConfig = "/api/brewpi/device/fullconfig/";
    constexpr auto registerDevice = "/api/brewpi/device/register/";
    constexpr auto status = "/api/brewpi/device/status/";
    constexpr auto messages = "/api/brewpi/device/messages/";
}; // namespace UpstreamAPIEndpoints

/**
 * \brief Strings used for JSON keys
 * \see restMessages
 */
namespace RestMessagesKeys {
constexpr auto messages = "messages";
constexpr auto updated_cs = "updated_cs";
constexpr auto updated_cc = "updated_cc";
constexpr auto updated_mt = "updated_mt";
constexpr auto updated_es = "updated_es";
constexpr auto updated_devices = "updated_devices";
constexpr auto default_cc = "default_cc";
constexpr auto default_cs = "default_cs";
constexpr auto reset_eeprom = "reset_eeprom";
constexpr auto reset_wifi = "reset_wifi";
constexpr auto restart_device = "restart_device";
constexpr auto refresh_config = "refresh_config";
}; // namespace RestMessagesKeys


enum class sendResult {
    success,
    failure,
    retry
};

class restMessages
{
public:
    bool updated_cs = false;
    bool updated_cc = false;
    bool updated_mt = false;
    bool updated_es = false;
    bool updated_devices = false;
    bool default_cc = false;
    bool default_cs = false;
    bool reset_eeprom = false;
    bool reset_wifi = false;
    bool restart_device = false;
    bool refresh_config = false;

    bool requires_processing() {
        return updated_cs || updated_cc || updated_mt || updated_es || updated_devices || default_cc || default_cs || reset_eeprom || reset_wifi || restart_device || refresh_config;
    }

};

class restHandler
{
    enum class httpMethod {
        HTTP_PUT,
        HTTP_POST,
        HTTP_PATCH,
        HTTP_GET,
        HTTP_DELETE
    };

    constexpr const char* httpMethodToString(httpMethod method) {
        switch (method) {
            case httpMethod::HTTP_PUT:
                return "PUT";
            case httpMethod::HTTP_POST:
                return "POST";
            case httpMethod::HTTP_PATCH:
                return "PATCH";
            case httpMethod::HTTP_DELETE:
                return "DELETE";
            case httpMethod::HTTP_GET:
            default:
                return "GET";
        }
    }
public:

    // Timers and semaphores
    Ticker fullConfigTicker;
    Ticker statusTicker;
    Ticker registerDeviceTicker;

    bool send_full_config_ticker;
    bool send_status_ticker;
    bool register_device_ticker;

    bool trigger_unregister_device;

    restHandler();
    void init();

    bool send_bluetooth_crash_report();

    // Everything below this MAY no longer be in use. Need to check. 
    void process();

    bool send_lock = false;


private:

    bool messages_pending_on_server;

    bool send_full_config();
    bool register_device();
    bool send_status();
    bool unregister_device();
    bool get_messages(bool override);
    bool set_message_processed(const char* message_type_key);

    // Message processing
    restMessages messages;
    void process_messages();
    bool reset_eeprom();
    bool reset_wifi();
    bool restart_device();
    bool default_cs();
    bool default_cc();
    bool process_updated_settings();

    bool get_url(char *url, size_t size, const char *path);
    bool get_url(char *url, size_t size, const char *path, const char *device_id, const char *api_key);
    sendResult send_json_str(String &payload, const char *url, httpMethod method);
    sendResult send_json_str(String &payload, const char *url, String &response, httpMethod method);
    void get_useragent(char *ua, size_t size);

    HTTPClient http;
    WiFiClient client;
    WiFiClientSecure secureClient;
};


extern restHandler rest_handler;


