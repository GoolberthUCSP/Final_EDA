#Filtrar y limpiar archivos residuales de un archivo csv
#Guardar cambios en un nuevo archivo csv
import pandas as pd

file= "..\Final_EDA\dataset.csv"

def clear_csv(file):
    df = pd.read_csv(file)
    #Imprimir los nombres de las columnas del dataset
    """
    Index(['danceability', 'energy', 'key', 'loudness', 'mode', 'speechiness',
       'acousticness', 'instrumentalness', 'liveness', 'valence', 'tempo',
       'type', 'id', 'uri', 'track_href', 'analysis_url', 'duration_ms',
       'time_signature', 'genre', 'song_name', 'Unnamed: 0', 'title'],
      dtype='object')
    """
    #El dataset tiene 22 columnas, se eliminan las columnas que no se van a utilizar
    #Columnas a eliminar: 'type', 'id', 'uri', 'track_href', 'analysis_url', 'time_signature', 'Unnamed: 0'
    #Se reemplaza las columnas sin valor por "Undefined"
    df = df.drop(['type', 'id', 'uri', 'track_href', 'analysis_url', 'time_signature', 'Unnamed: 0'], axis=1)
    df = df.fillna("Undefined")
    #Se guarda el nuevo dataset en un archivo csv
    df.to_csv('..\Final_EDA\cleaned.csv', index=False)

clear_csv(file)