#Filtrar y limpiar archivos residuales de un archivo csv
#Guardar cambios en un nuevo archivo csv
import pandas as pd
import sys 

file= sys.path[0] + "\dataset.csv"
#Imprimir el directorio actual
def clear_csv():
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
    df = df.drop(['type', 'id', 'key', 'uri', 'mode', 'track_href', 'analysis_url', 'time_signature', 'Unnamed: 0'], axis=1)
    #key=[0,1,2,3,4,5,6,7,8,9,10,11]
    #mode=[0,1]
    #time_signature=[1,3,4,5]
    #valores de tempo y duration_ms deben ser estandarizados
    #Se divide las 2 columnas por el valor maximo de cada una
    df['tempo'] = df['tempo']/df['tempo'].max()
    df['duration_ms'] = df['duration_ms']/df['duration_ms'].max()

    #Obtener los genres unicos del dataset
    genres = df['genre'].unique()
    #Se crea un diccionario con los genres unicos
    genres_dict = {}
    for i in range(len(genres)):
      genres_dict[genres[i]] = i
    #Se reemplaza los valores de genre por los valores numericos
    df['genre'] = df['genre'].replace(genres_dict)

    #Flotantes con n decimales
    n=4
    df['danceability'] = df['danceability'].round(n)
    df['energy'] = df['energy'].round(n)
    df['loudness'] = df['loudness'].round(n)
    df['speechiness'] = df['speechiness'].round(n)
    df['acousticness'] = df['acousticness'].round(n)
    df['instrumentalness'] = df['instrumentalness'].round(n)
    df['liveness'] = df['liveness'].round(n)
    df['valence'] = df['valence'].round(n)
    df['tempo'] = df['tempo'].round(n)
    df['duration_ms'] = df['duration_ms'].round(n)
    df = df.fillna("Null")
    #Se guarda el nuevo dataset en un archivo csv
    df.to_csv(sys.path[0] + "\cleaned.csv", index=False)

clear_csv()