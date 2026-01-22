from utils import load_ai_model, get_latest_data, run_inference
import time

# Load the brain
brain = load_ai_model()

print("--- Sentinel-V Bridge Test ---")
print("NOTE: Make sure your C++ engine is running in another terminal!")

try:
    while True:
        # grab the data from the C++ Loop
        raw_data = get_latest_data() # in dataframe format

        # ask the AI what it thinks
        processed_data = run_inference(brain, raw_data)

        if processed_data is not None:
            # look at just the first device (Main_Motor_A)
            row = processed_data.iloc[0]
            status = "DANGER" if row['ai_prediction'] == 1 else "SAFE"

            print(f"Device: {row['device_name']} | Mag: {row['smooth_mag']:.2f} | Status: {status}")

        time.sleep(0.1) # frequency = 10 Hz

except KeyboardInterrupt:
    print("\nBridge Test Stopped")

# test success