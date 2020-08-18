(function(window) {

var unGreeter = {};

var speakWord = "Good Bye";


unGreeter.Bye = function (name) {
  console.log(speakWord + " " + name);
}

window.unGreeter = unGreeter;

})(window);