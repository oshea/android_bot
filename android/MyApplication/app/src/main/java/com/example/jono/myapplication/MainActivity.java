package com.example.jono.myapplication;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.*;
import android.util.Log;
import android.hardware.usb.*;
import com.felhr.usbserial.*;


public class MainActivity extends AppCompatActivity {

    UsbSerialDevice serial;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        Button mClickButton1 = (Button) findViewById(R.id.clickButton1);

        UsbDevice device;
        UsbDeviceConnection usbConnection;

        UsbManager manager = (UsbManager) getSystemService(Context.USB_SERVICE);
        List<UsbSerialDriver> availableDrivers = UsbSerialProber.getDefaultProber().findAllDrivers(manager);
        if (availableDrivers.isEmpty()) {
            Log.v("Test", "Empty");
            return;
        }


//        serial = UsbSerialDevice.createUsbSerialDevice(device, usbConnection);
//        serial.open();
//        serial.setBaudRate(9600);
//        serial.setDataBits(UsbSerialInterface.DATA_BITS_8);
//        serial.setParity(UsbSerialInterface.PARITY_ODD);
//        serial.setFlowControl(UsbSerialInterface.FLOW_CONTROL_OFF);
    }

    public void sendMessage(View view) {
//        serial.write("123");
        Log.v("Test", "Clicked");
    }
}
d