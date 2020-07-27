from django.shortcuts import render,redirect
from django.http import HttpResponse
from .models import *
from django.contrib.auth.forms import UserCreationForm
from .forms import CreateUserForm
from django.contrib import messages
from django.contrib.auth import authenticate,login,logout
from django.contrib.auth.decorators import login_required
from .decorators import unauthenticated_user
import pyrebase
from django.http import JsonResponse
from django.core.mail import send_mail
import time
from geopy.geocoders import Nominatim
from .tests import *



def send_json(request):

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
    drivingConditions = 'Ok'
    temp = db.child("tempandgps/Int/TEMP").get().val()
    long = db.child("tempandgps/Int/LONG").get().val()
    lat = db.child("tempandgps/Int/LAT").get().val()
    safe = db.child("tempandgps/Int/safe").get().val()
    driver = db.child("tempandgps/Int/driving").get().val()
    breaking = db.child("/accData/Int/break").get().val()
    print(breaking)
    if breaking == 0:
        print("Driving Safe")
    elif breaking == 1:
        print('Hard break')
        send_mail('Smart Car Seat: Driving Update', "Driver made a hard break.py ma" + time.ctime(),
                  'smartcarseat2@gmail.com',
                  [request.user.email], fail_silently=False)
    elif breaking == 2:
        print("Hard Left")
        send_mail('Smart Car Seat: Driving Update', "Driver made a hard left." + time.ctime(),
                  'smartcarseat2@gmail.com',
                  [request.user.email], fail_silently=False)
    else:
        send_mail('Smart Car Seat: Driving Update', "Driver made a hard right." + time.ctime(),
                  'smartcarseat2@gmail.com',
                  [request.user.email], fail_silently=False)
        print("hard Right")
    carSeatLocation = str(lat) + "," + str(long)
    geolocator = Nominatim(user_agent="Smart Car Seat App")
    location = geolocator.reverse(carSeatLocation)
    accelerometerSafety =  seatSafety()
    #Check the position of the car seat
    if accelerometerSafety == 0:
        print('carseat is stable')
    else:
        print('carseat is NOT stable')
        send_mail('Find your child (ALERT)', "Carseat is not stable. Please adjust it immediately. -- " + time.ctime(), 'smartcarseat2@gmail.com',
                      [request.user.email], fail_silently=False)
        #time.sleep(160000)

    if (int(safe) > 0):
        send_mail('Find your child (ALERT)', "Coordinates " + str(lat) + " " + str(long) + " at " + time.ctime(), 'smartcarseat2@gmail.com',
                      [request.user.email], fail_silently=False)
        #time.sleep(60000)
        safe = "UNSAFE"
    else:
        safe = "SAFE"

    data = [{'temp': temp, 'lat': lat,
            'long': long, 'safe': safe, 'driver': driver, 'location': location.address}]


    return JsonResponse(data, safe=False)

def registerPage(request):
    form = CreateUserForm()
    if request.method == 'POST':
        form = CreateUserForm(request.POST)
        if form.is_valid():
            form.save()
            user = form.cleaned_data.get('username')
            messages .success(request, 'Account created for ' + user)
            return redirect('login')
    context = {'form': form}
    return render(request, 'carseat/register.html',context)

@unauthenticated_user
def loginPage(request):
    context = {}
    if request.method == 'POST':
        username = request.POST.get('username')
        password = request.POST.get('password')

        user = authenticate(request, username = username, password = password)
        if user is not None:
            login(request, user)
            return redirect('home')
        else:
            messages.info(request, 'Username OR Password is incorrect.')


    return  render(request, 'carseat/login.html',context)

def logoutUser(request):
    logout(request)
    return redirect('login')

@login_required(login_url='login')
def home(request):

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
    long = db.child("tempandgps/Int/LONG").get().val()
    lat = db.child("tempandgps/Int/LAT").get().val()
    carSeatLocation = str(lat) + "," + str(long)
    geolocator = Nominatim(user_agent="Smart Car Seat App")
    location = geolocator.reverse(carSeatLocation)
    lat = location.latitude
    long = location.longitude
    context = {
    'long':long,
    'lat':lat,

    }
    return render(request, 'carseat/dashboard.html',context)



