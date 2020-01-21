// Copyright (c) 2017 Akos Kiss.
//
// Licensed under the BSD 3-Clause License
// <LICENSE.md or https://opensource.org/licenses/BSD-3-Clause>.
// This file may not be copied, modified, or distributed except
// according to those terms.

#ifndef ENIGMASERVICE_H
#define ENIGMASERVICE_H

#include "mbed.h"
#include "ble/BLE.h"
#include "main.h"
#include <math.h>
#include "Enigma.h"

// #include <string>

class EnigmaService {
public:
    EnigmaService(BLE &ble)
        : _ble(ble),
        _configCharacteristic(
            0x2D00, 
            &_config,
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY
        ), 
        _inputCharacteristic(
            0x2D01, 
            &_input,
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY
        ),
        _outputCharacteristic(
            0x2D02, 
            &_output,
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY
        )
    {
        _configCharacteristic.setReadAuthorizationCallback(this, &EnigmaService::readAuthorizationCallback_config);
        _inputCharacteristic.setReadAuthorizationCallback(this, &EnigmaService::readAuthorizationCallback_input);
        _outputCharacteristic.setReadAuthorizationCallback(this, &EnigmaService::readAuthorizationCallback_output);

        _configCharacteristic.setWriteAuthorizationCallback(this, &EnigmaService::writeAuthorizationCallback_config);
        _inputCharacteristic.setWriteAuthorizationCallback(this, &EnigmaService::writeAuthorizationCallback_input);
        _outputCharacteristic.setWriteAuthorizationCallback(this, &EnigmaService::writeAuthorizationCallback_output);

        GattCharacteristic *charTable[] = {
            &_configCharacteristic, 
            &_inputCharacteristic, 
            &_outputCharacteristic
        };
        GattService service(
            0xA000, 
            charTable, 
            sizeof(charTable) / sizeof(GattCharacteristic *)
        );

        ble.addService(service);
        ble.onDataWritten(this, &EnigmaService::onDataWritten);
    }

protected:
    void onDataWritten(const GattWriteCallbackParams *params)
    {
    }

    // read callbacks
    // config
    void readAuthorizationCallback_config(GattReadAuthCallbackParams *params)
    {
        usb.printf("read called.\r\n");
        enigma.getConfig(this->_config);
        params->data = (uint8_t*) &(this->_config);
        params->len = sizeof(char)*3;
        params->authorizationReply = AUTH_CALLBACK_REPLY_SUCCESS;
    }
    // input
    void readAuthorizationCallback_input(GattReadAuthCallbackParams *params) {
        usb.printf("read called.\r\n");
        params->data = (uint8_t*) &(this->_input);
        params->len = strlen((char*) params->data);
        params->authorizationReply = AUTH_CALLBACK_REPLY_SUCCESS;
    }
    // output
    void readAuthorizationCallback_output(GattReadAuthCallbackParams *params) {
        usb.printf("read called.\r\n");
        encrypt();

        params->data = (uint8_t*) &(this->_output);
        params->len = strlen((char*) params->data);
        params->authorizationReply = AUTH_CALLBACK_REPLY_SUCCESS;
    }

    void writeAuthorizationCallback_config(GattWriteAuthCallbackParams *params) {
        memcpy(this->_config, params->data, params->len);
        enigma.setConfig(this->_config);
        usb.printf("[enigma] got data config   : %s\n", this->_config);
    }
    void writeAuthorizationCallback_input(GattWriteAuthCallbackParams *params) {
        memcpy(this->_input, params->data, params->len);
        this->_input[params->len] = 0;
        usb.printf("[enigma] got data input   : %s\n", this->_input);
    }
    void writeAuthorizationCallback_output(GattWriteAuthCallbackParams *params) {
        memcpy(this->_output, params->data, params->len);
        this->_output[params->len] = 0;
        usb.printf("[enigma] got data output   : %s\n", this->_output);
    }

    void encrypt() {
        memcpy(this->_output, this->_input, sizeof(char)*100);
        enigma.encode(this->_output);
    }

    enum { CURRENT_TIME_DATA_SIZE = 10 };

    BLE &_ble;
    char _input[100], _config[3], _output[100];
    ReadWriteGattCharacteristic<char[100]> _inputCharacteristic, _outputCharacteristic;
    ReadWriteGattCharacteristic<char[3]> _configCharacteristic;
    Enigma enigma;
};

#endif
