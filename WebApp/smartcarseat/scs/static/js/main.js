
$(document).ready(function () {

  setInterval(() => {
    $.getJSON("/sendjson", function (data) {
        console.log(data[0].drivingConditions);
        $('#driver').text(data[0].driver);
        $('#safe').text(data[0].safe);
        $('#location').text(data[0].location)
        $('#temp').text(data[0].temp + "°");

      });
}, 15000);



function showNotification() {
    const notification = new Notification("Welcome")
}

console.log(Notification.permission);

if (Notification.permission === 'granted'){
console.log("PASSED")
  showNotification()
}else if(Notification.permission !== 'denied'){
  Notification.requestPermission().then(permission => {
    if(permission === 'granted'){
    console.log("ACCESS GRANTED")
       showNotification()

    }
  })
}


});

