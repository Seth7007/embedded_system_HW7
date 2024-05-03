/* Sockets Example
 * Copyright (c) 2016-2020 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed.h"
#include "wifi_helper.h"
#include "mbed-trace/mbed_trace.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"
#include "arm_math.h"
#include "math_helper.h"
#include <cstddef>
#include <cstdint>

#if MBED_CONF_APP_USE_TLS_SOCKET
#include "root_ca_cert.h"

#ifndef DEVICE_TRNG
#error "mbed-os-example-tls-socket requires a device which supports TRNG"
#endif
#endif // MBED_CONF_APP_USE_TLS_SOCKET

#define TEST_LENGTH_SAMPLES  320
#define BLOCK_SIZE            32
#define NUM_TAPS_ARRAY_SIZE   29
#define NUM_TAPS              29
static float32_t testOutput[TEST_LENGTH_SAMPLES]; // output buffer
static float32_t firStateF32[BLOCK_SIZE + NUM_TAPS - 1]; // state buffer

/*filter coefficient*/
const float32_t firCoeffs32[NUM_TAPS_ARRAY_SIZE] = {
  +0.0015711031f, +0.0019358774f, -0.0000000000f, -0.0045080042f, -0.0069614120f, +0.0000000000f, +0.0150407722f, +0.0212098924f, 
  -0.0000000000f, -0.0406688346f, -0.0568310932f, +0.0000000000f, +0.1312091027f, +0.2717720869f, +0.3324610185f, +0.2717720869f, 
  +0.1312091027f, +0.0000000000f, -0.0568310932f, -0.0406688346f, -0.0000000000f, +0.0212098924f, +0.0150407722f, +0.0000000000f, 
  -0.0069614120f, -0.0045080042f, -0.0000000000f, +0.0019358774f, +0.0015711031f
};

uint32_t blockSize = BLOCK_SIZE;
uint32_t numBlocks = TEST_LENGTH_SAMPLES/BLOCK_SIZE;

InterruptIn button(BUTTON1);
bool volatile start = false;
void button_pressed(){ 
    
    start = true;
}

int16_t pData[3] = {0};
float32_t** sensor_value = new float32_t* [3];

class SocketDemo {
    static constexpr size_t MAX_NUMBER_OF_ACCESS_POINTS = 10;
    static constexpr size_t MAX_MESSAGE_RECEIVED_LENGTH = 100;

#if MBED_CONF_APP_USE_TLS_SOCKET
    static constexpr size_t REMOTE_PORT = 443; // tls port
#else
    static constexpr size_t REMOTE_PORT = 7777; // standard HTTP port
#endif // MBED_CONF_APP_USE_TLS_SOCKET

public:
    SocketDemo() : _net(NetworkInterface::get_default_instance())
    {
    }

    ~SocketDemo()
    {
        if (_net) {
            _net->disconnect();
        }
    }

    void run()
    {
        if (!_net) {
            printf("Error! No network interface found.\r\n");
            return;
        }
        
        
        /* if we're using a wifi interface run a quick scan */
        //if (_net->wifiInterface()) {
            /* the scan is not required to connect and only serves to show visible access points */
            //wifi_scan();

            /* in this example we use credentials configured at compile time which are used by
             * NetworkInterface::connect() but it's possible to do this at runtime by using the
             * WiFiInterface::connect() which takes these parameters as arguments */
        //}

        /* connect will perform the action appropriate to the interface type to connect to the network */

        printf("Connecting to the network...\r\n");

        nsapi_size_or_error_t result = _net->connect();
        if (result != 0) {
            printf("Error! _net->connect() returned: %d\r\n", result);
            return;
        }

        print_network_info();

        /* opening the socket only allocates resources */
        result = _socket.open(_net);
        if (result != 0) {
            printf("Error! _socket.open() returned: %d\r\n", result);
            return;
        }

#if MBED_CONF_APP_USE_TLS_SOCKET
        result = _socket.set_root_ca_cert(root_ca_cert);
        if (result != NSAPI_ERROR_OK) {
            printf("Error: _socket.set_root_ca_cert() returned %d\n", result);
            return;
        }
        _socket.set_hostname(MBED_CONF_APP_HOSTNAME);
#endif // MBED_CONF_APP_USE_TLS_SOCKET

        /* now we have to find where to connect */

        SocketAddress address;

        if (!resolve_hostname(address)) {
            return;
        }

        address.set_port(REMOTE_PORT);

        /* we are connected to the network but since we're using a connection oriented
         * protocol we still need to open a connection on the socket */

        printf("Opening connection to remote port %d\r\n", REMOTE_PORT);

        result = _socket.connect(address);
       
        if (result != 0) {
            printf("Error! _socket.connect() returned: %d\r\n", result);
            return;
        }
        

        char acc_json[500];
        for(int i = 0; i < 3; i++){
            sensor_value[i] = new float32_t[TEST_LENGTH_SAMPLES];
        }
        button.fall(&button_pressed);
        while(1){
            if (start == true) break;
        }
        printf("start to collect data!\n");
        BSP_ACCELERO_Init();
        int counter = 0;
        while(1){
            if(counter >= TEST_LENGTH_SAMPLES){
                printf("data collect successful!\n");
                break;
            } 
            BSP_ACCELERO_AccGetXYZ(pData);
            for(int i = 0; i < 3; i++){
                sensor_value[i][counter] = (float32_t)pData[i]; 
            }
            counter++;
            ThisThread::sleep_for(10ms);
        }

        for(int j = 0; j < TEST_LENGTH_SAMPLES; j++){
            if(j == 1){
                ThisThread::sleep_for(100ms);
            }
            int len = std::sprintf(acc_json,"{\"x\":%f}",(float)((int)(sensor_value[0][j]*10000))/10000);
            nsapi_size_or_error_t response = _socket.send(acc_json,len);
            if (0 >= response){
                printf("Error seding: %d\n", response);
                break;
            }
            ThisThread::sleep_for(200ms);
        }

        uint32_t i;
        arm_fir_instance_f32 S;
        arm_status status;
        float32_t  *inputF32, *outputF32;

        /* Initialize input and output buffer pointers */
        inputF32 = sensor_value[0];
        outputF32 = &testOutput[0];

        /* Call FIR init function to initialize the instance structure. */
        arm_fir_init_f32(&S, NUM_TAPS, (float32_t *)&firCoeffs32[0], &firStateF32[0], blockSize);

        /* ----------------------------------------------------------------------
        ** Call the FIR process function for every blockSize samples
        ** ------------------------------------------------------------------- */

        for(i=0; i < numBlocks; i++)
        {
            arm_fir_f32(&S, inputF32 + (i * blockSize), outputF32 + (i * blockSize), blockSize);
        }

        
        for(int j = 0; j < TEST_LENGTH_SAMPLES; j++){
            int len = std::sprintf(acc_json,"{\"x\":%f}",(float)((int)(testOutput[j]*10000))/10000);
            nsapi_size_or_error_t response = _socket.send(acc_json,len);
            if (0 >= response){
                printf("Error seding: %d\n", response);
                break;
            }
            ThisThread::sleep_for(200ms);
        }
        printf("Demo concluded successfully \r\n");
    }

private:
    bool resolve_hostname(SocketAddress &address)
    {
        const char hostname[] = MBED_CONF_APP_HOSTNAME;

        /* get the host address */
        printf("\nResolve hostname %s\r\n", hostname);
        nsapi_size_or_error_t result = _net->gethostbyname(hostname, &address);
        if (result != 0) {
            printf("Error! gethostbyname(%s) returned: %d\r\n", hostname, result);
            return false;
        }

        printf("%s address is %s\r\n", hostname, (address.get_ip_address() ? address.get_ip_address() : "None") );

        return true;
    }

    void wifi_scan()
    {
        WiFiInterface *wifi = _net->wifiInterface();

        WiFiAccessPoint ap[MAX_NUMBER_OF_ACCESS_POINTS];

        /* scan call returns number of access points found */
        int result = wifi->scan(ap, MAX_NUMBER_OF_ACCESS_POINTS);

        if (result <= 0) {
            printf("WiFiInterface::scan() failed with return value: %d\r\n", result);
            return;
        }

        printf("%d networks available:\r\n", result);

        for (int i = 0; i < result; i++) {
            printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\r\n",
                   ap[i].get_ssid(), get_security_string(ap[i].get_security()),
                   ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
                   ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5],
                   ap[i].get_rssi(), ap[i].get_channel());
        }
        printf("\r\n");
    }

    void print_network_info()
    {
        /* print the network info */
        SocketAddress a;
        _net->get_ip_address(&a);
        printf("IP address: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
        _net->get_netmask(&a);
        printf("Netmask: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
        _net->get_gateway(&a);
        printf("Gateway: %s\r\n", a.get_ip_address() ? a.get_ip_address() : "None");
    }

private:
    NetworkInterface *_net;

#if MBED_CONF_APP_USE_TLS_SOCKET
    TLSSocket _socket;
#else
    TCPSocket _socket;
#endif // MBED_CONF_APP_USE_TLS_SOCKET
};

int main() {
    printf("\r\nStarting socket demo\r\n\r\n");

#ifdef MBED_CONF_MBED_TRACE_ENABLE
    mbed_trace_init();
#endif

    SocketDemo *example = new SocketDemo();
    MBED_ASSERT(example);
    example->run();

    return 0;
}
