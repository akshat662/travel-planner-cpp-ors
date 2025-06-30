# import pandas as pd
# import openrouteservice
# from itertools import permutations
# import time

# # === CONFIG ===
# API_KEY = "5b3ce3597851110001cf624884fdb1c8904345adad447df9ae4d2602"  # Replace this with your real API key
# places_file = "places_all.txt"
# output_file = "distances_all.csv"

# client = openrouteservice.Client(key=API_KEY)

# # === Load all places ===
# places_df = pd.read_csv(places_file, header=None, names=["City", "Place", "Latitude", "Longitude"])

# rows = []

# # === Process each city ===
# for city in places_df["City"].unique():
#     city_places = places_df[places_df["City"] == city].reset_index(drop=True)

#     print(f"\n📍 Processing city: {city}")
#     for i, j in permutations(range(len(city_places)), 2):
#         p1 = city_places.loc[i]
#         p2 = city_places.loc[j]

#         # Clean place names
#         name1 = p1["Place"].strip()
#         name2 = p2["Place"].strip()

#         coords = ((p1["Longitude"], p1["Latitude"]), (p2["Longitude"], p2["Latitude"]))

#         try:
#             route = client.directions(coords)
#             dist_km = round(route['routes'][0]['summary']['distance'] / 1000, 2)
#             time_min = round(route['routes'][0]['summary']['duration'] / 60, 2)

#             rows.append([city, name1, name2, dist_km, time_min])
#             print(f"✅ {name1} → {name2} = {dist_km} km, {time_min} min")
#             time.sleep(1)
#         except Exception as e:
#             print(f"❌ ERROR: {name1} → {name2}: {e}")

# # === Save output ===
# df = pd.DataFrame(rows, columns=["City", "Source", "Destination", "Distance_km", "Time_min"])
# df.to_csv(output_file, index=False)
# print(f"\n✅ All data saved to: {output_file}")

import pandas as pd
import openrouteservice
from itertools import permutations
import time

# === CONFIG ===
API_KEY = "5b3ce3597851110001cf624884fdb1c8904345adad447df9ae4d2602"
places_file = "places_all.txt"
output_file = "distances_all.csv"

client = openrouteservice.Client(key=API_KEY)

# === SNAP COORDINATES TO ROADS ===
def snap_to_road(lat, lon):
    try:
        result = client.pelias_reverse((lon, lat))
        snapped = result['features'][0]['geometry']['coordinates']  # [lon, lat]
        return (snapped[1], snapped[0])  # return (lat, lon)
    except Exception as e:
        print(f"⚠️ Snap failed for ({lat}, {lon}): {e}")
        return (lat, lon)  # fallback to original

# === LOAD PLACES ===
places_df = pd.read_csv(places_file, header=None, names=["City", "Place", "Latitude", "Longitude"])

rows = []

# === PROCESS EACH CITY ===
for city in places_df["City"].unique():
    city_places = places_df[places_df["City"] == city].reset_index(drop=True)

    print(f"\n📍 Processing city: {city}")
    for i, j in permutations(range(len(city_places)), 2):
        p1 = city_places.loc[i]
        p2 = city_places.loc[j]

        name1 = p1["Place"].strip()
        name2 = p2["Place"].strip()

        # Snap coordinates
        lat1, lon1 = snap_to_road(p1["Latitude"], p1["Longitude"])
        lat2, lon2 = snap_to_road(p2["Latitude"], p2["Longitude"])
        coords = ((lon1, lat1), (lon2, lat2))

        # Use walking mode for big cities
        profile =  "driving-car"

        try:
            route = client.directions(coords, profile=profile)
            dist_km = round(route['routes'][0]['summary']['distance'] / 1000, 2)
            time_min = round(route['routes'][0]['summary']['duration'] / 60, 2)

            rows.append([city, name1, name2, dist_km, time_min])
            print(f"✅ {name1} → {name2} = {dist_km} km, {time_min} min")
            time.sleep(1)
        except Exception as e:
            print(f"❌ ERROR: {name1} → {name2}: {e}")
            rows.append([city, name1, name2, 0.0, 0.0])  # fallback

# === SAVE OUTPUT ===
df = pd.DataFrame(rows, columns=["City", "Source", "Destination", "Distance_km", "Time_min"])
df.to_csv(output_file, index=False)
print(f"\n✅ All data saved to: {output_file}")
