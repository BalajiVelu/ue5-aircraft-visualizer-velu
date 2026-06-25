# UE5 Aircraft Telemetry Visualizer

![UE5](https://img.shields.io/badge/Unreal%20Engine-5.3-black?style=flat-square&logo=unrealengine&logoColor=white)
![C++](https://img.shields.io/badge/C++-17-blue?style=flat-square&logo=c%2B%2B&logoColor=white)
![Windows](https://img.shields.io/badge/Windows-10%2F11-0078D6?style=flat-square&logo=windows&logoColor=white)
![UDP](https://img.shields.io/badge/Protocol-UDP-orange?style=flat-square)
![Winsock2](https://img.shields.io/badge/Socket-Winsock2-green?style=flat-square)

<p align="center">
  <img src="docs/demo.gif" alt="Real-time aircraft telemetry visualization demo" width="800"/>
</p>

Real-time aircraft telemetry visualization in Unreal Engine 5.3.2 Ingests live UDP flight data and renders 3D aircraft position, attitude, and gimbal orientation with multi-perspective camera coverage.

---


## What It Does

- Listens for aircraft state data via UDP on port 6506 at 50 Hz
- Converts geodetic coordinates (lat/lon/alt) to local Unreal world space using the start position as origin
- Renders a UAV model with real-time position, rotation, and independent gimbal tracking
- Provides three switchable camera views: chase, fixed ground observer, and gimbal-stabilized
- Handles UDP stream restarts without application restart (stateless packet processing)
- Survives invalid inputs — bounds-checked deserialization with explicit failure modes

---

## System Architecture

```
UDP Telemetry Source (Port 6506)
    |
    v
UdpReceiverActor ............ Non-blocking UDP socket (ws2_32.lib)
    |                           Tick-based receive, packet validation
    v
AircraftState Struct ........ Binary-packed 13-double telemetry frame
    |
    v
AircraftPawn ................ Position, rotation, gimbal update
    |                           ENU conversion: start position = origin
    v
CameraManagerActor .......... Three camera systems, user-switchable
```

### Data Flow

| Stage | Component | Responsibility |
|---|---|---|
| Ingest | `UdpReceiverActor` | UDP socket init, non-blocking read, data validation |
| Parse | `AircraftState` struct | Binary deserialization, field mapping |
| Transform | `AircraftPawn` | ENU coordinate conversion, actor placement |
| Render | `CameraManagerActor` | Viewport management, camera switching |

---

## Telemetry Data Format

UDP packets on port 6506 contain a binary-packed structure of 13 `double` values (104 bytes total):

```cpp
struct AircraftState
{
    double simulation_time__s;              // Simulation timestamp

    // Start position (used as ENU origin)
    double startposition_latitude__deg;
    double startposition_longitude__deg;
    double startposition_altitude__m_amsl;

    // Current aircraft position
    double aircraft_latitude__deg;
    double aircraft_longitude__deg;
    double aircraft_altitude__m_amsl;

    // Aircraft attitude (Euler angles, radians)
    double aircraft_phi__rad;                 // Roll
    double aircraft_theta__rad;               // Pitch
    double aircraft_psi__rad;                 // Yaw

    // Gimbal attitude (world coordinates, radians)
    double gimbal_phi__rad;                   // Gimbal roll
    double gimbal_theta__rad;                 // Gimbal pitch
    double gimbal_psi__rad;                   // Gimbal yaw
};
```

### Coordinate Systems

| System | Description | Units |
|---|---|---|
| Input | WGS84 geodetic | degrees, meters AMSL |
| Intermediate | Local ENU (East-North-Up) | meters, origin at start position |
| Unreal Engine | World space | centimeters, X=East, Y=North, Z=Up |

---

## Camera System

### Camera Views

<p align="center">
  <img src="docs/camera-diagram.png" alt="Three camera perspectives: chase, operator, and gimbal" width="700"/>
</p>

| Key | Camera | Description |
|---|---|---|
| **1** | Chase | Third-person view from behind aircraft, 5 m distance, 30° elevation |
| **2** | Operator | Fixed ground position 10 m south of start, 1.7 m height, tracks aircraft |
| **3** | Gimbal | Mounted on aircraft, rotation driven by telemetry gimbal angles |

The three cameras map to three operator roles — pilot, controller, sensor operator — each with a different mental model of the same asset.

---

## Setup & Run

### Prerequisites

- Unreal Engine 5.3+
- Visual Studio 2022
- Windows SDK (for `ws2_32.lib`)
- Python 3 (for UDP relay)

### Steps

1. Clone the repository:
   ```bash
   git clone https://github.com/BalajiVelu/ue5-aircraft-visualizer-velu.git
   ```

2. Open `Velu_Case_Study_V1.uproject` in Unreal Engine 5.3

3. Build the C++ project (Unreal Editor will prompt you, or build via Visual Studio)

4. Start the UDP data relay:
   ```bash
   python relay.py
   ```
   

5. Press **Play** in the Unreal Editor

6. Press **1**, **2**, or **3** to switch camera views

---

## Project Structure

```
ue5-aircraft-visualizer-velu/
├── Source/
│   ├── Velu_Case_Study_V1/
│   │   ├── Private/
│   │   │   ├── AircraftPawn.cpp
│   │   │   ├── CameraManagerActor.cpp
│   │   │   ├── MyPlayerController.cpp
│   │   │   ├── OperatorCameraActor.cpp
│   │   │   ├── udp_socket.cpp
│   │   │   ├── UdpReceiverActor.cpp
│   │   │   ├── UdpReceiverRunnable.cpp
│   │   │   └── UdpTestActor.cpp
│   │   ├── Public/
│   │   │   ├── aircraft_state.hpp
│   │   │   ├── AircraftPawn.h
│   │   │   ├── CameraManagerActor.h
│   │   │   ├── GeoCoordinateUtils.h
│   │   │   ├── MyPlayerController.h
│   │   │   ├── OperatorCameraActor.h
│   │   │   ├── udp_socket.hpp
│   │   │   ├── UdpReceiverActor.h
│   │   │   ├── UdpReceiverRunnable.h
│   │   │   └── UdpTestActor.h
│   │   ├── Velu_Case_Study_V1.Build.cs
│   │   ├── Velu_Case_Study_V1.cpp
│   │   └── Velu_Case_Study_V1.h
│   ├── Velu_Case_Study_V1.Target.cs
│   └── Velu_Case_Study_V1Editor.Target.cs
├── Content/
│   ├── Blueprints/
│   ├── Input_Mapping/
│   ├── Materials/
│   └── Levelo.umap
├── Config/
│   ├── DefaultEngine.ini
│   ├── DefaultGame.ini
│   └── DefaultInput.ini
├── Velu_Case_Study_V1.uproject
├── relay.py             # Python UDP relay            
└── README.md
```

---

## Troubleshooting

### No aircraft movement visible

- Verify the relay is running: `python relay.py`
- Confirm UDP port 6506 is not blocked by firewall
- Check Unreal Output Log for UDP connection messages

### Build fails with missing includes

- Regenerate Visual Studio project files: right-click `.uproject` → *Generate Visual Studio project files*
- Delete `Intermediate` and `Binaries` folders, then rebuild
- Verify Visual Studio 2022 C++ toolchain is installed

### Camera keys (1, 2, 3) not responding

- Ensure the level has focus (click in viewport)
- Check Input Mapping assets exist in `Content/Input_Mapping/`
- Verify `MyPlayerController` is assigned as the active player controller

### Aircraft position jumps or warps

- Verify the first UDP packet contains valid start position coordinates
- Check that telemetry data format matches the expected `AircraftState` structure
- Review Output Log for coordinate conversion debug values

---

## Technical Notes

- **UDP Reception:** Non-blocking reads via `UdpSocket::TryGet()`, polled each Tick. No persistent state between packets. 50 Hz over UDP reflects the latency-over-loss tradeoff that visual real-time demands.
- **Coordinate Conversion:** Start position from the first received telemetry frame becomes the local ENU origin (0,0,0). Subsequent positions are offset relative to this origin.
- **Statelessness:** Restarting the UDP relay or streaming a different recording requires no application restart.
- **Robustness:** Bounds-checked deserialization with explicit failure modes — invalid packet sizes are silently discarded, null references are guarded, and the system never crashes on malformed inputs.
- **Platform:** Windows-only due to `ws2_32.lib` dependency.

---

## Demo

Video demonstration: [LinkedIn post](https://www.linkedin.com/posts/balaji-velu_unrealengine-spatialcomputing-aerospaceengineering-ugcPost-7475858174990999554-0lAt/?utm_source=share&utm_medium=member_desktop&rcm=ACoAACAzjYkB64FplBvBPrW-dyrX2pb5cLqscnI) 

---

## Built With

- Unreal Engine 5.3
- C++
- Winsock2
- Python 3 (relay)

---

## Where This Goes Next

Three directions I'm actively exploring from this foundation:

**Enterprise AR** — Taking the operator off a flat screen and into a headset, with role-based viewports switched by gesture. Unity dominates XR prototyping, but Unreal's rendering fidelity and C++ architecture make it the stronger fit for high-fidelity industrial and aerospace AR.

**VR Simulation Training** — Immersive pilot and ground-station operator training built on the same telemetry backbone.

**Synthetic Data for Sim-to-Real ML** — Procedural environments and auto-labeled datasets generated from the same coordinate and camera infrastructure.

---

## License

MIT License — see [LICENSE](LICENSE)

### 3D Model
UAV model: **"MQ-9 Reaper"** by [creadordemu](https://sketchfab.com/creadordemu)  
License: [CC Attribution](https://skfb.ly/pIY8K)

---

*Built fast. Shipped. Next: same engine, off the desktop, onto a HoloLens 2.*
