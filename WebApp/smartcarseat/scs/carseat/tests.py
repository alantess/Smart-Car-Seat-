from django.test import TestCase
import functools
import pandas as pd
import numpy as np
import tensorflow as tf
from tensorflow.keras.callbacks import ModelCheckpoint
from tensorflow.keras.models import load_model
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import  Dense,Activation,Flatten,Conv2D,MaxPooling2D,Dropout
from tensorflow.keras.optimizers import SGD, Adam
from tensorflow.keras import regularizers
from tensorflow.keras.utils import to_categorical
from tensorflow.keras.callbacks import EarlyStopping
import pyrebase
import matplotlib.pyplot as plt


def seatSafety():
    Config = {
            "apiKey": "AIzaSyAHWyf7gRdCAFExGz8vF9qIMlj1dQOWkYU",
            "authDomain": "smart-seat-f37f8.firebaseapp.com",
            "databaseURL": "https://smart-seat-f37f8.firebaseio.com",
            "projectId": "smart-seat-f37f8",
            "storageBucket": "smart-seat-f37f8.appspot.com",
            "messagingSenderId": "555682731681",
            "appId": "1:555682731681:web:bbff92ad6a7886171942e0"
        }

    firebase = pyrebase.initialize_app(Config)
    db = firebase.database()
    drivingData = []
    accData = []
    for i in range(10):
        data = db.child('accData/Int/Data' + str(i)).get().val()
        #print(data)
        drivingData.append(data)


    accData.append(drivingData)
    accelerometerarr = np.asarray(accData)
    print(accelerometerarr)



    TRAIN_DATA_URL = "https://docs.google.com/spreadsheets/d/1HBwBDxobibCzLfEBBlA8vFXY8afACi3vW9Xcw08hKhc/edit#gid=1892081344"
    TEST_DATA_URL = 'https://docs.google.com/spreadsheets/d/1LfsZT8lm7alXyyN5G0oJUfKBTBJFEK58Uc-TryEpNuk/edit#gid=1892081344'
    csv_test_url = TEST_DATA_URL.replace('/edit#gid=', '/export?format=csv&gid=')
    csv_export_url = TRAIN_DATA_URL.replace('/edit#gid=', '/export?format=csv&gid=')

    test_df = pd.read_csv(csv_export_url,sep=",")
    test_df['Label'] = pd.Categorical(test_df['Label'])
    test_df['Label'] = test_df.Label.cat.codes
    test_x = test_df.drop(columns=['Label'])
    test_df.head()


    array = pd.DataFrame(accelerometerarr,columns = ['Data1','Data2','Data3','Data4','Data5','Data6','Data7','Data8','Data9','Data10'])
    array.to_csv('../models/data.csv')
    test_x_2 = pd.read_csv('../models/data.csv', index_col=0)
    test_x_2.head()

    df = pd.read_csv(csv_export_url,sep=",")
    df['Label'] = pd.Categorical(df['Label'])
    df['Label'] = df.Label.cat.codes
    df.head()

    train_X = df.drop(columns=['Label'])
    train_X.head()

    train_y = df[['Label']]
    train_y.head()
    test_y = test_df[['Label']]
    y_test_c = to_categorical(test_y,2)
    y_Train_c = to_categorical(train_y,2)
    n_cols = train_X.shape[1]

    #
    # model = Sequential()
    # model.add(Dense(10,activation='relu', input_shape=(n_cols,)))
    # model.add(Dense(22, activation='relu'))
    # model.add(Dense(2, activation='softmax'))
    # model.compile(loss='categorical_crossentropy',optimizer=Adam(lr=0.0001),metrics=['acc'])
    # early_stopping_monitor = EarlyStopping(monitor='val_loss', mode='min', verbose=1, patience=30)
    # mc = ModelCheckpoint('../models/best_model.h5', monitor='val_loss', mode='min', save_best_only=True)
    # history = model.fit(train_X, y_Train_c, validation_split=0.2, epochs=150, callbacks=[early_stopping_monitor,mc],batch_size=10)
    #model.save('../models/CarData2.h5')
    model = load_model('../models/best_model.h5')
    #model.summary()
    predictions = model.predict_classes(test_x_2[:])
    # print(array[:1])
    # 0 means safe and 1 means unsafe
    #print('predictions:', predictions)
    #results = model.evaluate(test_x, y_test_c, batch_size=10)
    #print('Test Loss , Test Acc: ',results)
    # plt.subplot(1,2,1)
    # plt.plot(history.history['loss'])
    # plt.plot(history.history['val_loss'])
    # plt.title('Model loss')
    # plt.ylabel('Loss')
    # plt.xlabel('Epoch')
    # plt.legend(['Train', 'Val'], loc='upper right')
    # plt.subplot(1,2,2)
    # plt.plot(history.history['acc'])
    # plt.plot(history.history['val_acc'])
    # plt.title('Model Accuracy')
    # plt.ylabel('Loss')
    # plt.xlabel('Epoch')
    # plt.legend(['Train', 'Val'], loc='upper right')
    # plt.show()
    return int(predictions)

