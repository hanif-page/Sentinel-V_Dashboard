#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <string>
using namespace std;
const int DATA_SIZE_PER_DEVICE = 1; // make it a low-latency system.

#include <thread> // For sleeping the loop
#include <chrono> // for defining time units (ms)

// exercise: Add more feature to part6.cpp code, which are enabling exporting data from the generated value from takeReading() function.

class Sensor {
protected: // like private, but its child classes can access anything here
    string name;

public:
    // constructor (mandatory)
    Sensor(string n) : name(n) {}

    // 'virtual' + '= 0' makes this a Pure Virtual Function.
    // It means this class is "Abstract" (we can't create a 'Sensor' object directly)
    virtual void takeReading() = 0;

    // a Virtual Destructor is MANDATORY when using Inheritance concept
    // why? -> It ensures child classes are deleted correctly from the memory
    virtual ~Sensor() {}
};

// Derived Class
class VibrationSensor : public Sensor {
private:
    mt19937 generate;
    vector<double> freqHistory;
    vector<double> magHistory;
    const int WINDOW_SIZE = 3;
    double currentRawFreq;
    double currentSmoothFreq;
    double currentRawMag;
    double currentSmoothMag;

public:
    // pass the name up to the base class constructor
    VibrationSensor(string n) : Sensor(n) {
        // run the random generator 
        random_device rd;
        generate.seed(rd());
    }

    // implement the specific logic for this sensor
    // objective: generate a random "frequency" with range 10-500 Hz and a random "magnitude" with range 0-10 Gs
    // output: print the name of the sensor along with these simulated vibration value
    void takeReading() override {
        uniform_real_distribution<double> freqDist(10.0, 500.0);
        uniform_real_distribution<double> magDist(0.0, 10.0);

        // Update the state (by keeping the generated value)
        double rawFreq = freqDist(this->generate);
        double rawMag = magDist(this->generate);

        // adding the raw value to the history array
        this->freqHistory.push_back(rawFreq);
        this->magHistory.push_back(rawMag);

        // updating the private raw data
        this->currentRawFreq = rawFreq;
        this->currentRawMag = rawMag;

        // review the history data
        if(magHistory.size() > this->WINDOW_SIZE){
            magHistory.erase(magHistory.begin());
            freqHistory.erase(freqHistory.begin());
        }

        // updating the private smooth data
        this->currentSmoothFreq = this->calculateFrequencyHistoryAverage();
        this->currentSmoothMag = this->calculateMagnitudeHistoryAverage();
    }

    double calculateMagnitudeHistoryAverage() const {
        // check if the history < 3
        if(this->magHistory.size() == 0){
            return 0.0;
        }
        else {
            // smoothed value is the average of the history value.
            double sum = 0.0;
            for(double mag : this->magHistory){
                sum = sum + mag;
            }
            return sum / magHistory.size();
        }
    }

    double calculateFrequencyHistoryAverage() const {
        // check if the history < 3
        if(this->freqHistory.size() == 0){
            return 0.0;
        }
        else {
            // smoothed value is the average of the history value.
            double sum = 0.0;
            for(double freq : this->freqHistory){
                sum = sum + freq;
            }
            return sum / freqHistory.size();
        }
    }

    double getRawFrequency() const {
        return this->currentRawFreq;
    }

    double getRawMagnitude() const {
        return this->currentRawMag;
    }

    double getSmoothFrequency() const {
        return this->currentSmoothFreq;
    }

    double getSmoothMagnitude() const {
        return this->currentSmoothMag;
    }

    string getDeviceName() const {
        return this->name;
    }

};

int main() {

    
    // array of multiple vibration sensors
    vector<VibrationSensor*> sensors;
    
    sensors.push_back(new VibrationSensor("Main_Motor_A"));
    sensors.push_back(new VibrationSensor("Cooling_Fan_01"));
    sensors.push_back(new VibrationSensor("Hydraulic_Pump_02"));
    sensors.push_back(new VibrationSensor("Main_Motor_B"));
    sensors.push_back(new VibrationSensor("Cooling_Fan_03"));
    
    cout << "--- SENTINEL-V ENGINE STARTING (ENGINE STARTED) ---" << endl << endl;
    
    bool isRunning = true;
    int iterations = 0;

    while (isRunning) {
        // main logic
        ofstream dataFile("../data/live_stream.csv", ios::trunc);

        if(dataFile.is_open()){
            // frequency unit: Hz
            // magnitude unit: G (Gs, G-force)
            dataFile << "device_name,timestamp,raw_freq,smooth_freq,raw_mag,smooth_mag,isDanger" << endl;
    
            for(VibrationSensor* sensor : sensors){
                for(int i = 0; i < DATA_SIZE_PER_DEVICE; i++){
                    sensor->takeReading();
    
                    int isDanger = 0;
                    if(sensor->getSmoothFrequency() >= 350.0 || sensor->getSmoothMagnitude() >= 5.0){
                        isDanger = 1;
                    }
    
                    // adding 1 row for all the data
                    dataFile << sensor->getDeviceName() << ","
                            << i << ","
                            << sensor->getRawFrequency() << ","
                            << sensor->getSmoothFrequency() << ","
                            << sensor->getRawMagnitude() << ","
                            << sensor->getSmoothMagnitude() << ","
                            << isDanger << endl;
                }
    
                cout << "--- " << sensor->getDeviceName() << " DATA ACQUISITION FINISHED ---" << endl;
            }
    
            // close the file access
            dataFile.close();
        }

        cout << endl;

        // create a 10Hz update rate
        this_thread::sleep_for(chrono::milliseconds(100));

        // stop the loop when iterations
        iterations++;
        if(iterations > 600){ // approx 60 second
            isRunning = false;
        }
    }

    // loop through sensors array to delete each of the sensor
    for(VibrationSensor* sensor : sensors){
        delete sensor;
    }
    sensors.clear();

    cout << "\n--- SENTINEL-V ENGINE STOPPED ---" << endl;

    return 0;
}