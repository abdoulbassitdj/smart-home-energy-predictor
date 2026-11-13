from fastapi import FastAPI
from pydantic import BaseModel

import joblib
import pandas as pd

app = FastAPI()

# Modèle de données attendu de l'ESP32
class DonneesMeteo(BaseModel):
    temperature: float
    humidite: float

@app.get("/")
def home():
    return {"status": "Le serveur FastAPI est en ligne et accessible via localtunnel !"}

@app.post("/update")
def recevoir_donnees(data: DonneesMeteo):
    # Traitement des données reçues

    df = pd.DataFrame(
    [[data.temperature, data.humidite]], 
    columns=["Temperature_C", "Humidity_%"]
)

    modele_charge = joblib.load('SmartHomeEnergyConsumptionPredictor.joblib')
    predictions = modele_charge.predict(df)

    print(f"--- Nouvelle lecture reçue ---")
    print(f"Température : {data.temperature} °C")
    print(f"Humidité    : {data.humidite} %")
    print(f"Consommation predite    : {predictions.item():.2f} kWh")
    return {"message": "Données reçues avec succès !"}
