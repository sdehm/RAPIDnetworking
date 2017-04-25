 
 
 /** RF24Mesh_Example_Master.ino by TMRh20
  * 
  * Note: This sketch only functions on -Arduino Due-
  *
  * This example sketch shows how to manually configure a node via RF24Mesh as a master node, which
  * will receive all data from sensor nodes.
  *
  * The nodes can change physical or logical position in the network, and reconnect through different
  * routing nodes as required. The master node manages the address assignments for the individual nodes
  * in a manner similar to DHCP.
  *
  */
  
#include "RF24Mesh/RF24Mesh.h"  
#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>

#include <stdio.h>
#include <stdlib.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "nanopb_RPI.pb.h"

#include <iostream>
#include <iomanip>

using namespace std;


RF24 radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);  
RF24Network network(radio);
RF24Mesh mesh(radio,network);


int main(int argc, char** argv) {
  
  time_t timev;
  struct gm_time;
  char isotime[sizeof("2017-04-21T08:00:00Z")];
	  
  // Set the nodeID to 0 for the master node
  mesh.setNodeID(0);
  // Connect to the mesh
//  printf("start\n");
  mesh.begin();
//  radio.printDetails();

  //Shaffer
  uint8_t buffer[32];
  size_t message_length=sizeof(buffer);
  bool status;

  SensorData message = SensorData_init_zero;
  pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);

while(1)
{
  
  // Call network.update as usual to keep the network updated
  mesh.update();

  // In addition, keep the 'DHCP service' running on the master node so addresses will
  // be assigned to the sensor nodes
  mesh.DHCP();
  
  
  // Check for incoming data from the sensors
  while(network.available()){
//    printf("rcv\n");
    RF24NetworkHeader header;
    network.peek(header);
    uint8_t dat[64];
    switch(header.type){
      // Display the incoming millis() values from the sensor nodes
      case 'M': network.read(header,&dat,message_length); 
	{
                //printf("Rcv %u from 0%o\n",dat[0],header.from_node);
		SensorData message = SensorData_init_zero;
		pb_istream_t stream = pb_istream_from_buffer(dat,sizeof(dat));
		status = pb_decode(&stream, SensorData_fields, &message);
		if (!status)
		{
			printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
//			return 1;
		}
		
		time(&timev);
		strftime(isotime,80,"%FT%TZ",gmtime(&timev));
		cout << isotime;

		cout << " " << (int)message.sensorId;
		//printf("%d ", message.sensorId);
		if (message.has_value)
		{
			//printf("Value: %f ", message.value);
			cout << " value: " << fixed << setprecision(3) << (float)message.value;
		}
		if (message.has_timestamp)
		{
			//printf("count: %d ", message.timestamp);
			cout << " count: " << (int)message.timestamp;
		}
		if (message.has_temperature)
		{
			//printf("Temperature: %f ", message.temperature);
			cout << " tempr: " << (float)message.temperature;
		}
		if (message.has_humidity)
		{		
			//printf("Humidity: %f ", message.humidity);
			cout << " humid: " << (float)message.humidity;
		}
		if (message.has_pressure)
		{
			//printf("Pressure: %f ", message.pressure);
			cout << " press: " << (float)message.pressure;
		}
		if (message.has_altitude)
		{		
			//printf("Altitude: %f ", message.altitude);
			cout << " altit: " << (float)message.altitude;
		}
		//printf('\n');
		cout << endl;
                break;
	}
      default:  network.read(header,0,0); 
                printf("Rcv bad type %d from 0%o\n",header.type,header.from_node); 
                break;
    }
  }
  delay(2);
}
return 0;
}

//const std::string currentDateTime() {
	//time_t    now = time(0);
      
