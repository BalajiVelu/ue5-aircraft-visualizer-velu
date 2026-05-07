# ✈️ UE5 Aircraft State Visualizer

**Real-time aircraft telemetry visualization in Unreal Engine 5.3+**

In this project, a real-time aircraft simulation and 3D visualization system is developed using Unreal Engine 5.3.2, driven by live UDP telemetry data.

---

## 🚀 Key Features

- **Real-time UDP Listener:** Custom non-blocking UDP socket on port 6506 using `ws2_32.lib`.
- **Accurate 3D Visualization:** Converts geocoordinates and Euler angles into Unreal world space.
- **Three Camera Views:** Third-person chase, operator, and dynamic gimbal, switchable with keys 1, 2, 3.
- **Robust Processing:** Handles UDP restarts, packet loss, and malformed data gracefully.
- **Visual Context:** Floor plane aligned with starting altitude.
- **Stateless & Robust:** Handles UDP restarts, missing/malformed packets, and stream reinitialization.
- **Clean Codebase:** Sensitive files excluded via `.gitignore`.

---

## 🛠️ Setup & Run

### Prerequisites

- Unreal Engine 5.3+
- Visual Studio 2022
- Visual C++ Redistributables (required for `relay_win_x64.exe`)
- Optional: Python 3 (alternative UDP relay)

### Steps

1. Clone repo.  - git clone <your-private-repo-url>
2. Open Velu_Case_Study_V1.uproject in UE5.3.
3. Build C++ project in Unreal Editor or Visual Studio.
4. Run data relay:
      - Recommended: Run relay_win_x64.exe (Provided)
      - Alternative: python3 relay.py acstate_recording.bin
5. Press Play in Unreal;
      - A cube representing the aircraft moving in real-time.
      - Use keys 1, 2, 3 to switch cameras (Camera0 - Third-person, Camera1 - operator, Camera2 - Gimbal) respectively.
        
---

## 🧪 Technical Notes

- **Dynamic origin from initial geo-reference.**
- **Coordinates converted to local ENU system.**
- **Non-blocking UDP parsing loop ensures performance.**

---

## 🎥 Demo

Includes a [demo_test_videos.zip](./demo_test_videos.zip) showing real-time updates, smooth camera switching, and UDP integration reliability.

---

## 🙌 Acknowledgements
Thanks to UDP Socket framework, Unreal Engine input system, geographic conversion sources, and AI-assisted prototyping/documentation.
