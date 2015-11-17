# cc3200_dweet
A simple dweet.io client demo application based on TI CC3200 chip-set with Energia SDK.


**Using this demo**

This demo attempts to connect to the defined WiFi network and in case of a success then a connection to [dweet.io](https://www.dweet.io) server is estalished. In case of fail to connect the red LED is turned ON else, upon connection success, the green LED is turned ON. After that a periodic request to get the latest "thing" content is performed. A related LED (yellow on CC3200 Launchpad) is turned ON or OFF when *led_test* field is respectively *on* or *off*.
For setting the thing contest copy and paste links below in your browser:
- turning led ON: http://dweet.io/dweet/for/test_cc3200?led_test=on
- turning led OFF: http://dweet.io/dweet/for/test_cc3200?led_test=off


**Configuring the WiFi network SSID and password**

Open the cc3200_dweet.ino file and change following default values:
```
/* network name also called SSID */
char ssid[] = "ssid";
/* network password */
char password[] = "ssid2015";
```
Build and program your device (CC3200 Launchpad) with Energia SDK.


**Installing Energia SDK and setting the CC3200 launchpad hardware**

Refer to this useful links:
- http://energia.nu/guide/
- http://energia.nu/cc3200guide/
