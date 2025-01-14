#ifndef PLUGINSTRUCTS_P004_DATA_STRUCT_H
#define PLUGINSTRUCTS_P004_DATA_STRUCT_H

#include "../../_Plugin_Helper.h"
#ifdef USES_P004

# include "../Helpers/Dallas1WireHelper.h"

# ifndef P004_FEATURE_GET_CONFIG_VALUE
#  define P004_FEATURE_GET_CONFIG_VALUE   1 // Enable by default,
// adds 468 bytes on ESP8266, 944 bytes on ESP32-C6, 490 bytes on ESP32-C3
// 456 bytes on ESP32 Classic, 468 bytes on ESP32-S3 (ESP32 builds: IDF 5.1)
# endif // ifndef P004_FEATURE_GET_CONFIG_VALUE

struct P004_data_struct : public PluginTaskData_base {
  /*********************************************************************************************\
  * Task data struct to simplify taking measurements of upto 4 Dallas DS18b20 (or compatible)
  * temperature sensors at once.
  *
  * Limitations:
  * - Use the same GPIO pin
  * - Use the same resolution for all sensors of the same task
  * - Max 4 sensors queried at the same time
  *
  * The limit of 4 sensors is determined by the way the settings are stored and it
  * is a practical limit to make sure we don't spend too much time in a single call.
  *
  * Using the same resolution is to make it (a lot) simpler as all sensors then need the
  * same measurement time.
  *
  * If those limitations are not desired, use multiple tasks.
  \*********************************************************************************************/

  // @param pin  The GPIO pin used to communicate to the Dallas sensors in this task
  // @param res  The resolution of the Dallas sensor(s) used in this task
  P004_data_struct(taskIndex_t taskIndex,
                   int8_t      pin_rx,
                   int8_t      pin_tx,
                   uint8_t     res,
                   bool        scanOnInit);
  virtual ~P004_data_struct() = default;

  void init();

  bool sensorAddressSet() const;

  // Add extra sensor address
  // @param addr The address to add
  // @param index  The index (0...3) to store this address
  void add_addr(const uint8_t addr[],
                uint8_t       index);

  // Send the start measuremnt command to all set sensors which have a non-zero address
  // Their index determines the order in which the sensors receive this command.
  bool initiate_read();

  bool collect_values();

  // Read temperature from the sensor at given index.
  // May return false if the sensor is not present or address is zero.
  bool          read_temp(float & value,
                          uint8_t index = 0) const;

  String        get_formatted_address(uint8_t index) const;

  unsigned long get_timer() const {
    return _timer;
  }

  unsigned long get_measurement_start() const {
    return _measurementStart;
  }

  int8_t get_gpio_rx() const {
    return _gpio_rx;
  }

  int8_t get_gpio_tx() const {
    return _gpio_tx;
  }

  bool              measurement_active() const;
  bool              measurement_active(uint8_t index) const;
  void              set_measurement_inactive();

  Dallas_SensorData get_sensor_data(uint8_t index) const;

private:

  // Do not set the _timer to 0, since it may cause issues
  // if this object is created (settings edited or task enabled)
  // while the node is up some time between 24.9 and 49.7 days.
  unsigned long     _timer;
  unsigned long     _measurementStart;
  Dallas_SensorData _sensors[VARS_PER_TASK];
  taskIndex_t       _taskIndex;
  int8_t            _gpio_rx;
  int8_t            _gpio_tx;
  uint8_t           _res;
  bool              _scanOnInit;
};

#endif // ifdef USES_P004
#endif // ifndef PLUGINSTRUCTS_P004_DATA_STRUCT_H
