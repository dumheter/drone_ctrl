# drone_ctrl
Reverse engineered the drone controller app to make a program that can take gamepad input to control the drone.
![one image, thousand words](https://github.com/dumheter/drone_ctrl/blob/master/resources/tldr.png?raw=true)


## Background
I have a drone. You control it with an app. It's hard without physical feedback.
So I thought, why not control it with a gamepad instead, how hard can it be?

## Demo
Drone in action

![gif of drone](https://github.com/dumheter/drone_ctrl/blob/master/resources/demo.gif?raw=true)

## Reverse Engineering
To reverse engineer the app I recoded all the network packets. Then analysed all the traffic produced by the app.
This is what I found:

The controller packet consists of 8 bytes, one static head and one static tail byte, with data in between.

Here are a few sample packets from when the drone flew up, then down.

![table](https://github.com/dumheter/drone_ctrl/blob/master/resources/table.jpg?raw=true)
