var UI = require('ui');
var ajax = require('ajax');

// Create a Card with title and subtitle
var card = new UI.Card({
  title:'STUFF IN THE SKY',
  subtitle:'LOOKIN AT THE SKY'
});

// Display the Card
card.show();

// Construct URL
var cityName = 'Tucson';
var URL = 'http://api.openweathermap.org/data/2.5/weather?q=' + cityName;

// Make the request
ajax(
  {
    url: URL,
    type: 'json'
  },
  function(data) {
    // Success!
    console.log("LOOKED AT THE WATER");

    // Extract data
    var location = data.name;
    var temperature = Math.round(data.main.temp - 273.15) + "C";

    // Always upper-case first letter of description
    var description = data.weather[0].description;
    description = description.charAt(0).toUpperCase() + description.substring(1);

    // Show to user
    card.subtitle(location + ", " + temperature);
    card.body(description);
  },
  function(error) {
    // Failure!
    console.log('COULDNT LOOK AT THE WEATHER: ' + error);
  }
);
