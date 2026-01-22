import streamlit as st
import pandas as pd
import time
from utils import load_ai_model, get_latest_data, run_inference

# Page Config
st.set_page_config(page_title="Sentinel-V Dashboard", layout="wide")
st.title("🛡️ Sentinel-V: Predictive Maintenance")

# 1. Initialization
model = load_ai_model()

# 2. Sidebar for Machine Selection
st.sidebar.header("System Controls")
device_list = ["Main_Motor_A", "Cooling_Fan_01", "Hydraulic_Pump_02", "Main_Motor_B", "Cooling_Fan_03"]
selected_device = st.sidebar.selectbox("Select Asset to Monitor", device_list)

# 3. Create placeholders for live updates
# This prevents the whole page from flickering
col1, col2 = st.columns(2)
with col1:
    gauge_placeholder = st.empty()
with col2:
    status_placeholder = st.empty()

chart_placeholder = st.empty()

# History storage for the chart
if 'history' not in st.session_state:
    st.session_state.history = pd.DataFrame(columns=['smooth_mag'])

# 4. The Live Update Loop
while True:
    df = get_latest_data()
    processed_df = run_inference(model, df)
    
    if processed_df is not None:
        # Filter for the selected device
        device_data = processed_df[processed_df['device_name'] == selected_device].iloc[0]
        
        # Update Gauge
        gauge_placeholder.metric(label="Current Vibration (Gs)", value=f"{device_data['smooth_mag']:.2f}")
        
        # Update AI Status Card
        if device_data['ai_prediction'] == 1:
            status_placeholder.error(f"DANGER DETECTED: {selected_device} requires immediate inspection.")
        else:
            status_placeholder.success(f"SYSTEM NOMINAL: {selected_device} operating normally.")

        # Update Chart (showing last 50 points)
        new_row = pd.DataFrame({'smooth_mag': [device_data['smooth_mag']]})
        st.session_state.history = pd.concat([st.session_state.history, new_row]).tail(50)
        chart_placeholder.line_chart(st.session_state.history)

    time.sleep(0.1) # Sync with your C++ 10Hz engine