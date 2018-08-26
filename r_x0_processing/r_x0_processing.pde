/**
 * Simple Read
 * 
 * Read data from the serial port and change the color of a rectangle
 * when a switch connected to a Wiring or Arduino board is pressed and released.
 * This example works with the Wiring / Arduino program that follows below.
 */


import processing.serial.*;

Serial myPort;  // Create object from Serial class
String val;      // Data received from the serial port

void setup() 
{
  size(200, 200);
  // I know that the first port in the serial list on my mac
  // is always my  FTDI adaptor, so I open Serial.list()[0].
  // On Windows machines, this generally opens COM1.
  // Open whatever port is the one you're using.
  String portName = Serial.list()[3];
  for (int i = 0; i < Serial.list().length; i++) {
    println(Serial.list()[i]);
  }
 
  myPort = new Serial(this, portName, 9600);
  
  String val = "START";
  
  println(val == "START");
}

void draw()
{
  if ( myPort.available() > 0) {  // If data is available,
    val = trim(myPort.readStringUntil('\n'));         // read it and store it in val
    println(val);
    if(val.equals("START")) {
       
      myPort.write(str(0));
      myPort.write(",");
      myPort.write(str(1));
      myPort.write(",");
      myPort.write(str(2));
      myPort.write(",");
      myPort.write("\r");
      
      myPort.write(str(4));
      myPort.write(",");
      myPort.write(str(5));
      myPort.write(",");
      myPort.write(str(8));
      myPort.write(",");
      myPort.write("\r");
      
      myPort.write("STOP");
      myPort.write(",");
      myPort.write(",");
      myPort.write(",");
      myPort.write("\r");
      println("did send");
    } else if(val.equals("SUCCESS")) {
      
    } else {
      println("nope");
    }
  }
  
  //background(255);             // Set background to white
  //if (val == 0) {              // If the serial value is 0,
  //  fill(0);                   // set fill to black
  //} 
  //else {                       // If the serial value is not 0,
  //  fill(204);                 // set fill to light gray
  //}
  //rect(50, 50, 100, 100);
}
