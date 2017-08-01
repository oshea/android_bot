package com.example.jono.androidserial;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.hardware.usb.*;
import android.content.Context;
import android.util.Log;
import java.io.IOException;
import android.widget.Button;
import android.view.View;
import android.widget.EditText;

import com.hoho.android.usbserial.driver.*;


public class MainActivity extends AppCompatActivity {

    UsbSerialDriver driver;
    Button myButton;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        UsbManager manager = (UsbManager) getSystemService(Context.USB_SERVICE);

        // Find the first available driver.
        driver = UsbSerialProber.acquire(manager);

        if (driver != null) {
            Log.d("TEST", "driver   ");
            try {
                driver.open();
                driver.setBaudRate(9600);

            } catch (IOException e) {
                // Deal with error.
            }
        }
    }

    public void forward(View view) throws IOException {
        byte[] buf = "Forward".getBytes();
        driver.write(buf, 1000);
    }

    public void backward(View view) throws IOException {
        byte[] buf = "Backward".getBytes();
        driver.write(buf, 1000);
    }

    public void stop(View view) throws IOException {
        byte[] buf = "Stop".getBytes();
        driver.write(buf, 1000);
    }

    public void left(View view) throws IOException {
        byte[] buf = "Left".getBytes();
        driver.write(buf, 1000);
    }

    public void right(View view) throws IOException {
        byte[] buf = "Right".getBytes();
        driver.write(buf, 1000);
    }
}
