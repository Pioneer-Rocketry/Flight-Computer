// Datapoint.h
#ifndef DATAPOINT_h
#define DATAPOINT_h

#include <Arduino.h>

typedef union
{
  float value;
  byte array[4];
}
dataPointT;

const int size = 38;
class Data {
  public:
    dataPointT time;

    dataPointT accX;
    dataPointT accY;
    dataPointT accZ;
    
    dataPointT gyrX;
    dataPointT gyrY;
    dataPointT gyrZ;

    dataPointT magX;
    dataPointT magY;
    dataPointT magZ;
    dataPointT heading;

    dataPointT angleX;
    dataPointT angleY;
    dataPointT angleZ;

    dataPointT velX;
    dataPointT velY;
    dataPointT velZ;

    dataPointT temp;
    dataPointT press;
    dataPointT alt;
    
    dataPointT filted_alt;
    dataPointT starting_alt;

    dataPointT state;

    dataPointT gps_lat;
    dataPointT gps_lng;
    dataPointT gps_sat;
    dataPointT gps_alt;
    dataPointT gps_speed;
    dataPointT gps_hdop;

    dataPointT air_speed;

    dataPointT pyro_arm;

    dataPointT pyro_1_con;
    dataPointT pyro_2_con;
    dataPointT pyro_3_con;
    dataPointT pyro_4_con;

    dataPointT pyro_1_fired;
    dataPointT pyro_2_fired;
    dataPointT pyro_3_fired;
    dataPointT pyro_4_fired;

    dataPointT allowStage;

    dataPointT array[size];

    String get_data() {
      String data = "";

      data += String(Data::time.value, 4) + ",";

      data += String(Data::accX.value, 2) + ",";
      data += String(Data::accY.value, 2) + ",";
      data += String(Data::accZ.value, 2) + ",";

      data += String(Data::gyrX.value, 2) + ",";
      data += String(Data::gyrY.value, 2) + ",";
      data += String(Data::gyrZ.value, 2) + ",";

      data += String(Data::magX.value, 2) + ",";
      data += String(Data::magY.value, 2) + ",";
      data += String(Data::magZ.value, 2) + ",";
      // data += String(Data::heading.value, 2) + ",";

      // data += String(Data::angleX.value, 2) + ",";
      // data += String(Data::angleY.value, 2) + ",";
      // data += String(Data::angleZ.value, 2) + ",";

      // data += String(Data::velX.value, 2) + ",";
      // data += String(Data::velY.value, 2) + ",";
      // data += String(Data::velZ.value, 2) + ",";

      data += String(Data::temp.value, 2) + ",";
      data += String(Data::press.value, 2) + ",";
      data += String(Data::alt.value, 2) + ",";
      data += String(Data::filted_alt.value, 2) + ",";

      data += String(Data::state.value) + ",";

      data += String(Data::gps_lat.value, 8) + ",";
      data += String(Data::gps_lng.value, 8) + ",";
      data += String(Data::gps_sat.value) + ",";
      data += String(Data::gps_alt.value, 2) + ",";
      data += String(Data::gps_speed.value, 2) + ",";
      data += String(Data::gps_hdop.value, 2) + ",";

      // data += String(Data::pyro_arm.value) + ",";

      // data += String(Data::pyro_1_con.value) + ",";
      // data += String(Data::pyro_2_con.value) + ",";
      // data += String(Data::pyro_3_con.value) + ",";
      // data += String(Data::pyro_4_con.value) + ",";

      // data += String(Data::pyro_1_fired.value) + ",";
      // data += String(Data::pyro_2_fired.value) + ",";
      // data += String(Data::pyro_3_fired.value) + ",";
      // data += String(Data::pyro_4_fired.value);

      return data;
    }

    Data() {
      allowStage.value = 0;
    }
};

#endif