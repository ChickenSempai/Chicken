(function(window){
var Greeter = {};
var speakWord = "Hello";

Greeter.Hello = function (name) {
  console.log(speakWord + " " + name);
}

window.Greeter = Greeter;

})(window);
