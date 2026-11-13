# Smart Home Energy Consumption Predictor

A small end-to-end IoT project: a simulated **ESP32 + DHT22** sensor sends temperature and humidity readings over Wi-Fi to a **FastAPI** server, which uses a pre-trained **scikit-learn** model to predict energy consumption (kWh) in real time.

```
DHT22 ──► ESP32 (Wokwi) ──HTTP POST /update──► localtunnel ──► FastAPI ──► ML model ──► predicted kWh
```

## Project structure

```
smart-home-energy-predictor/
├── main.py                                     # FastAPI server + model inference
├── SmartHomeEnergyConsumptionPredictor.joblib  # Trained model
├── smart_home_energy_usage_dataset.csv         # Smart home dataset (500 rows)
├── iiot_smart_grid_dataset.csv                 # IIoT smart grid dataset (~8,700 rows)
├── Dataset_source.txt                          # Dataset credits
├── wokwi-setup/
│   ├── sketch.ino                              # ESP32 firmware
│   ├── diagram.json                            # Wokwi circuit
│   ├── libraries.txt                           # Arduino libraries (DHT sensor library)
│   └── wokwi-project.txt                       # Link to the Wokwi project
└── .gitignore
```

## How it works

1. **Firmware (`sketch.ino`)**: every 5 seconds, the ESP32 reads temperature and humidity from a DHT22 (data pin on GPIO 4) and sends them as JSON via HTTP POST to the server.
2. **Server (`main.py`)**: the `/update` endpoint receives `{"temperature": float, "humidite": float}`, builds a DataFrame with the columns `Temperature_C` and `Humidity_%`, loads the model and prints the predicted consumption in kWh.
3. **Tunnel**: since Wokwi runs in the cloud, [localtunnel](https://github.com/localtunnel/localtunnel) exposes the local server to the internet.

## API

| Method | Route | Description |
|---|---|---|
| `GET` | `/` | Health check |
| `POST` | `/update` | Receives a sensor reading and prints the predicted consumption |

Example request:

```bash
curl -X POST http://localhost:8000/update \
  -H "Content-Type: application/json" \
  -d '{"temperature": 24.5, "humidite": 55.0}'
```

## Setup

### 1. Server

```bash
pip install fastapi uvicorn pandas scikit-learn joblib
uvicorn main:app --host 0.0.0.0 --port 8000
```

Use the same scikit-learn version (1.6.1) that was used to train the `.joblib` model.

### 2. Tunnel

```bash
npx localtunnel --port 8000
```

Copy the generated URL (e.g. `https://xxxx.loca.lt`).

### 3. ESP32 simulation (Wokwi)

1. Open the project from `wokwi-setup/wokwi-project.txt`, or import `diagram.json`, `sketch.ino` and `libraries.txt` into a new ESP32 project on [wokwi.com](https://wokwi.com).
2. In `sketch.ino`, set `serverUrl` to your tunnel URL followed by `/update`.
3. Start the simulation. Predictions appear in the server console.

**Wiring:** DHT22 `VCC` → 3V3, `GND` → GND, `SDA` → GPIO 4.

## Datasets

| Dataset | Content | Author |
|---|---|---|
| `smart_home_energy_usage_dataset.csv` | Per-home readings: temperature, humidity, light, appliance/HVAC/water heater usage, total energy, room | Prince Rajak (`prince7489/smart-home-energy-usage-dataset`, Kaggle) |
| `iiot_smart_grid_dataset.csv` | Hourly smart grid data: voltage, current, power factor, frequency, weather, solar/wind generation, consumption | Ziya (`ziya07/iot-enabled-smart-grid-dataset`, Kaggle) |

The model takes two inputs, `Temperature_C` and `Humidity_%`, matching what the DHT22 can measure.

## Notes

- The localtunnel URL changes on every restart, so update `serverUrl` in the sketch each time.
- The sketch sends the `Bypass-Tunnel-Reminder` header to skip localtunnel's reminder page.
- The server currently only prints predictions; it does not store them or return them to the ESP32.
