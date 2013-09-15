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
			ioManager.writeAsync(new byte[] { 'S' });
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
			String[] types = { "COOL", "HEAT", "OFF" };
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
				if (buffer.position() >= 314) { // first packet is 314(+3), all others are 314
					// Log.d(TAG, "Received total data: " + buffer.position());
					// parse the S response
					HashMap<String, String> lines = new HashMap<String, String>();
					int responseSize = buffer.position();
					for (int i = 0; i < responseSize; i++) {
						String line = "";
						while ((char) buffer.get(i) != '\n' && i < responseSize - 1) {
							if ((char) buffer.get(i) != '\r') {
								line += (char) buffer.get(i);
							}
							i++;
						}
						int index = line.indexOf(":");
						if (index != -1) {
							String key = line.substring(0, index);
							String value = null;
							value = line.substring(index + 1, line.length()).trim();
							if (!value.isEmpty()) {
								lines.put(key, value);
							}
						}
					}
					// lines map: {last=1, userChanged=0, 15=0, 16=0, 13=0, tooCold=0, 14=0, 11=0, 12=0, Desired Temp=72.00, curr=1, Temperature=72.52, tooHot=0, haveRestored=1, 3=1, 2=1, 10=0, 1=0, justRight=1, 7=0, 6=0, 5=0, 4=0, 9=0, 8=0, user=1}
					if (lines.size() == 26) {
						ThermostatStatus status = new ThermostatStatus(lines);
						Message m = new Message();
						m.what = 101;
						m.obj = status;
						USBSerialHandler.sendMessage(m);
						buffer.clear();
						Thread.sleep(50);
						ioManager.writeAsync(new byte[] { 'S' });
					} else {
						Log.e(TAG, "missing rx data!");
						buffer.clear();
						Thread.sleep(50);
						ioManager.writeAsync(new byte[] { 'S' });
					}
				}
			} catch (InterruptedException e) {
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
		HashMap<String, String> status;

		public ThermostatStatus(HashMap<String, String> status) {
			this.status = status;
		}

		public boolean getRelayState(int relayNum) {
			return (Integer.parseInt(status.get(String.valueOf(relayNum))) == 1);
		}

		public boolean getTooHot() {
			return (Integer.parseInt(status.get("tooHot")) == 1);
		}

		public boolean getTooCold() {
			return (Integer.parseInt(status.get("tooCold")) == 1);
		}

		public boolean getJustRight() {
			return (Integer.parseInt(status.get("justRight")) == 1);
		}

		public boolean getUserChanged() {
			return (Integer.parseInt(status.get("userChanged")) == 1);
		}

		public boolean getHaveRestored() {
			return (Integer.parseInt(status.get("haveRestored")) == 1);
		}

		public THERMOSTAT_STATES getLastState() {

			return THERMOSTAT_STATES.values()[Integer.parseInt(status.get("last"))];
		}

		public THERMOSTAT_STATES getCurrentState() {
			return THERMOSTAT_STATES.values()[Integer.parseInt(status.get("curr"))];
		}

		public THERMOSTAT_STATES getUserState() {
			return THERMOSTAT_STATES.values()[Integer.parseInt(status.get("user"))];
		}

		public float getCurrentTemp() {
			return Float.valueOf(status.get("Temperature"));
		}

		public float getDesiredTemp() {
			return Float.valueOf(status.get("Desired Temp"));
		}
	}
}
