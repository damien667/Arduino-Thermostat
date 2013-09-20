package com.damien667.arduino_thermostatcontroller;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.Iterator;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.Context;
import android.content.DialogInterface;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.TextView;

import com.hoho.android.usbserial.driver.UsbSerialDriver;
import com.hoho.android.usbserial.driver.UsbSerialProber;
import com.hoho.android.usbserial.util.SerialInputOutputManager;
import com.hoho.android.usbserial.util.SerialInputOutputManager.Listener;

public class ControlActivity extends Activity {

	private static final String TAG = "ControlActivity";
	private static final int DEFAULT_READ_BUFFER_SIZE = 16 * 1024;
	protected static final boolean DEBUG = false;
	Context context;

	// shared with Arduino code
	enum THERMOSTAT_STATES {
		OFF, COOL, HEAT, FAN
	};

	// Find the first available driver.
	UsbManager manager;
	UsbSerialDriver driver;
	SerialInputOutputManager ioManager;
	// USBSerialHandler uiHandler;
	private final ExecutorService mExecutor = Executors.newSingleThreadExecutor();

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_control);
		context = this;

		Button plusButton = (Button) findViewById(R.id.buttonPlus);
		Button minusButton = (Button) findViewById(R.id.buttonMinus);
		TextView userState = (TextView) findViewById(R.id.userState);

		plusButton.setOnClickListener(plusButtonListener);

		minusButton.setOnClickListener(minusButtonListener);

		userState.setOnClickListener(userStateLabelListener);

		// Get UsbManager from Android.
		manager = (UsbManager) getSystemService(Context.USB_SERVICE);

		HashMap<String, UsbDevice> devicelist = manager.getDeviceList();
		Iterator<UsbDevice> deviceIterator = devicelist.values().iterator();

		while (deviceIterator.hasNext()) {
			UsbDevice usbDevice = deviceIterator.next();
			// manager.requestPermission(usbDevice, null);
			Log.i(TAG, "Model     : " + usbDevice.getDeviceName());
			Log.i(TAG, "Id        : " + usbDevice.getDeviceId());
		}
	} // end onCreate

	@SuppressWarnings("static-access")
	@Override
	protected void onResume() {
		super.onResume();
		// Find the first available driver.
		driver = UsbSerialProber.findFirstDevice(manager);
		if (driver != null) {
			try {
				driver.open();
				driver.setParameters(115200, driver.DATABITS_8, driver.STOPBITS_1, driver.PARITY_NONE);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		} else {
			Log.e(TAG, "driver was null!");
		}
		onDeviceStateChange();
		// ioManager.setListener();
		// runOnUiThread(ioManager);
		if (ioManager != null) {
			ioManager.writeAsync(new byte[] { 's' });
		}
	}

	@Override
	protected void onPause() {
		super.onPause();
		stopIoManager();
		if (driver != null) {
			try {
				driver.close();
			} catch (IOException e) {
				// Ignore.
			}
			driver = null;
		}
		finish();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.control, menu);
		return true;
	}

	private void onDeviceStateChange() {
		stopIoManager();
		startIoManager();
	}

	private void stopIoManager() {
		if (ioManager != null) {
			Log.i(TAG, "Stopping io manager ..");
			ioManager.stop();
			ioManager = null;
		}
	}

	private void startIoManager() {
		if (driver != null) {
			Log.i(TAG, "Starting io manager ..");
			ioManager = new SerialInputOutputManager(driver, myListener);
			mExecutor.submit(ioManager);
		}
	}

	private OnClickListener plusButtonListener = new OnClickListener() {
		@Override
		public void onClick(View v) {
			if (ioManager != null) {
				ioManager.writeAsync(new byte[] { '+' });
			}
		}
	};

	private OnClickListener minusButtonListener = new OnClickListener() {
		@Override
		public void onClick(View v) {
			if (ioManager != null) {
				ioManager.writeAsync(new byte[] { '-' });
			}
		}
	};

	private OnClickListener userStateLabelListener = new OnClickListener() {
		@Override
		public void onClick(View v) {
			AlertDialog.Builder b = new Builder(context);
			b.setTitle("Set User State");
			String[] types = { "COOL", "HEAT", "FAN", "OFF" };
			b.setItems(types, new DialogInterface.OnClickListener() {

				@Override
				public void onClick(DialogInterface dialog, int which) {

					dialog.dismiss();

					switch (which) {
					case 0:
						if (ioManager != null) {
							ioManager.writeAsync(new byte[] { 'C' });
						}
						break;
					case 1:
						if (ioManager != null) {
							ioManager.writeAsync(new byte[] { 'H' });
						}
						break;
					case 2:
						if (ioManager != null) {
							ioManager.writeAsync(new byte[] { 'F' });
						}
						break;
					case 3:
						if (ioManager != null) {
							ioManager.writeAsync(new byte[] { 'O' });
						}
						break;
					}
				}
			});
			b.show();
		}
	};

	private final Listener myListener = new Listener() {
		ByteBuffer buffer = ByteBuffer.allocate(DEFAULT_READ_BUFFER_SIZE);

		@Override
		public void onNewData(byte[] data) {
			try {
				// Log.d(TAG, "data received!" + new String(data, "ASCII"));
				buffer.put(data);
				if (buffer.position() >= 64) { // data is 64 bytes (67 bytes first packet, 64 every packet after that)
					Log.d(TAG, "Received total data: " + buffer.position());
					// parse the s response
					// it could be padded with "�\r\n", followed by "s\r\n", then the first byte of data arrives
					String received = new String(buffer.array());
					int start = received.indexOf('s') + 3; // find start of response (after "s\r\n" which is 3 bytes)
					int end = buffer.position() - 2; // find end of response (truncate the last "\r\n")
					String filtered = received.substring(start, end);
					String[] values = filtered.split(",");
					Log.d(TAG, "Received " + values.length + " values");
					ThermostatStatus status = new ThermostatStatus(values);
					Message m = new Message();
					m.what = 101;
					m.obj = status;
					USBSerialHandler.sendMessage(m);
					buffer.clear();
					Thread.sleep(50);
					ioManager.writeAsync(new byte[] { 's' });
				}
			} catch (Exception e) {
				Log.e(TAG, e.getMessage());
			}
		}

		@Override
		public void onRunError(Exception e) {
			Log.e(TAG, e.getMessage());
		}

	};

	private Handler USBSerialHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			if (msg.what == 101) {
				try {
					if (DEBUG)
						Log.d(TAG, "Handling message!");
					ThermostatStatus status = (ThermostatStatus) msg.obj;

					// get all the views
					CheckBox tooHot = (CheckBox) findViewById(R.id.tooHot);
					CheckBox tooCold = ((CheckBox) findViewById(R.id.tooCold));
					CheckBox justRight = ((CheckBox) findViewById(R.id.justRight));
					CheckBox heatingRelayState = ((CheckBox) findViewById(R.id.HeatingRelayState));
					CheckBox coolingRelayState = ((CheckBox) findViewById(R.id.CoolingRelayState));
					CheckBox fanRelayState = ((CheckBox) findViewById(R.id.FanRelayState));
					CheckBox userChanged = ((CheckBox) findViewById(R.id.UserChanged));
					CheckBox haveRestored = ((CheckBox) findViewById(R.id.HaveRestored));

					// enums
					TextView lastState = ((TextView) findViewById(R.id.lastState));
					TextView currState = ((TextView) findViewById(R.id.currState));
					TextView userState = ((TextView) findViewById(R.id.userState));

					// doubles
					TextView currentTemp = ((TextView) findViewById(R.id.currentTemp));
					TextView desiredTemp = ((TextView) findViewById(R.id.desiredTemp));

					// update textview
					// booleans
					tooHot.setChecked(status.getTooHot());
					// tooHot.postInvalidate();
					tooHot.invalidate();
					tooCold.setChecked(status.getTooCold());
					// tooCold.postInvalidate();
					tooCold.invalidate();
					justRight.setChecked(status.getJustRight());
					// justRight.postInvalidate();
					justRight.invalidate();
					heatingRelayState.setChecked(status.getRelayState(1));
					// heatingRelayState.postInvalidate();
					heatingRelayState.invalidate();
					coolingRelayState.setChecked(status.getRelayState(2));
					// coolingRelayState.postInvalidate();
					coolingRelayState.invalidate();
					fanRelayState.setChecked(status.getRelayState(3));
					// fanRelayState.postInvalidate();
					fanRelayState.invalidate();
					userChanged.setChecked(status.getUserChanged());
					// userChanged.postInvalidate();
					userChanged.invalidate();
					haveRestored.setChecked(status.getHaveRestored());
					// haveRestored.postInvalidate();
					haveRestored.invalidate();

					// enums
					lastState.setText("Last State: " + status.getLastState().name());
					// lastState.postInvalidate();
					lastState.invalidate();
					currState.setText("Current State: " + status.getCurrentState().name());
					// currState.postInvalidate();
					currState.invalidate();
					userState.setText("User State: " + status.getUserState().name());
					// userState.postInvalidate();
					userState.invalidate();

					// doubles
					currentTemp.setText("Current Temp: " + String.valueOf(status.getCurrentTemp()));
					// currentTemp.postInvalidate();
					currentTemp.invalidate();
					desiredTemp.setText("Desired Temp: " + String.valueOf(status.getDesiredTemp()));
					// desiredTemp.postInvalidate();
					desiredTemp.invalidate();

				} catch (Exception e) {
					Log.e(TAG, e.getMessage());
				}
			}
			super.handleMessage(msg);
		}
	};

	private class ThermostatStatus {
		String[] status;

		public ThermostatStatus(String[] status) {
			this.status = status;
		}

		// relays are first 16 Strings (0-15)
		public boolean getRelayState(int relayNum) {
			return (Integer.parseInt(status[relayNum - 1]) == 1);
		}

		public boolean getTooHot() {
			return (Integer.parseInt(status[19]) == 1);
		}

		public boolean getTooCold() {
			return (Integer.parseInt(status[21]) == 1);
		}

		public boolean getJustRight() {
			return (Integer.parseInt(status[20]) == 1);
		}

		public boolean getUserChanged() {
			return (Integer.parseInt(status[24]) == 1);
		}

		public boolean getHaveRestored() {
			return (Integer.parseInt(status[25]) == 1);
		}

		public THERMOSTAT_STATES getLastState() {
			return THERMOSTAT_STATES.values()[Integer.parseInt(status[17])];
		}

		public THERMOSTAT_STATES getCurrentState() {
			return THERMOSTAT_STATES.values()[Integer.parseInt(status[16])];
		}

		public THERMOSTAT_STATES getUserState() {
			return THERMOSTAT_STATES.values()[Integer.parseInt(status[18])];
		}

		public float getCurrentTemp() {
			return Float.valueOf(status[23]);
		}

		public float getDesiredTemp() {
			return Float.valueOf(status[22]);
		}
	}
}
