# Sentinel-V: Industrial Predictive Maintenance Dashboard

This is Muhammad Ammar Hanif self-project.

## Milestones
- [x] Build the live sensor simulator CPP engine
- [x] Build the Intelligence Bridge (consume the live_stream.csv data)
- [x] Build the UI/UX Dashboard using the Python Streamlit module
- [ ] Final Polish, Final Functionality, Error/Bug Management

## Overview
Sentinel-V is an end-to-end monitoring solution designed to predict mechanical failure in industrial assets. It bridges high-performance C++ hardware simulation with a Python-based AI intelligence layer.

## Tech Stack
- **Data Generator (Engine):** C++ (OOP, Signal Processing, Moving Average Filter)
- **AI Model:** Python Scikit-Learn (Random Forest Classifier)
- **Dashboard:** Streamlit (Real-time Web UI)
- **Data Pipeline:** Real-time file-buffer synchronization between C++ and Python

## Key Features
- **Real-Time Inference:** Predicts "Safe" vs "Danger" status in under 10ms.
- **Signal Denoising:** Integrated C++ moving average filter to handle sensor noise.
- **Multi-Device Support:** Capable of tracking multiple simulated industrial motors.

## Folder Structure
- `/cpp_engine`: Contains the high-frequency vibration simulation logic.
- `/models`: Houses the serialized Random Forest model (.pkl).
- `app.py`: The entry point for the interactive web dashboard.