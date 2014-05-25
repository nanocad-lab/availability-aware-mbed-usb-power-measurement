/* main.cpp
 * Tested with mbed board: FRDM-KL46Z
 * Author: Mark Gottscho
 * mgottscho@ucla.edu
 */

#define NDEBUG

#include "mbed.h"
#include "util.h"
#include "SerialManager.h"
#include <string>
#include "const.h" //for various constants in global scope
#include "INA219.h" //For talking with the power sensor

using namespace std;

//The console object is global so that all objects can use its generic serial methods.
SerialManager console(USBTX, USBRX, 115200, false); //For serial console
DigitalOut green(LED1);
DigitalOut red(LED2);

typedef struct {
    string name;
    float voltage;
    float current;
    float power;
} power_state_t;

int main() {
    green = 0; //Indicate board has power

    //Turn on red LED during init
    red = 0;  
    
    __disable_irq(); //No interrupts during init
    
    //System power state
    power_state_t usb_state;
    
    //Init power state
    usb_state.name = "USB";
    usb_state.voltage = 0;
    usb_state.current = 0;
    usb_state.power = 0;

	//Simple timer
	float second = 0;
    
    //Power/battery monitoring sensors
    INA219 pwr_sens_usb(PTE0, PTE1, PWR_SENS_SUPPLY_I2C_ADDRESS);
    
    //Init power sensors
    pwr_sens_usb.selfInit();
 
    //Turn off red LED when finished init
    red = 1;

    __enable_irq(); //enable interrupts after initialization

	console.serial.printf("USB,,\r\n");
	console.serial.printf("Time(s),Voltage (V),Current (A),Power (W)\r\n");

    //The Loop
    while(1) {    
        //Sample power state
        usb_state.voltage = pwr_sens_usb.getBusVoltageFloat(true);
        usb_state.power = pwr_sens_usb.getPowerFloat(true);
        usb_state.current = pwr_sens_usb.getCurrentFloat(false);
        
        //Report power state
        console.serial.printf("%f,%f,%f,%f\r\n", second, usb_state.voltage, usb_state.current * 1000, usb_state.power * 1000);
        
		wait(0.5); // Sample at 2 Hz
		second += 0.5;
    }
}
