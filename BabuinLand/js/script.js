$(function () { // Same as document.addEventListener("DOMContentLoaded"...

  // Same as document.querySelector("#navbarToggle").addEventListener("blur",...
  $("#navbarToggle").blur(function (event) {
    var screenWidth = window.innerWidth;
    if (screenWidth < 768) {
      $("#collapsable-nav").collapse('hide');
    }
  });
});

(function (global) {

var dc = {};

var homeHtmlUrl = "snippets/home-snippet.html";
var BabuinListHtml = "snippets/Babuin-list.json";
var BabuinItemHtml = "snippets/Babuin-item.html";
var BabuinItemTitle = "snippets/Babuin-items-title.html";
var PoetHtml = "snippets/poet-snippet.html"
// Convenience function for inserting innerHTML for 'select'
var insertHtml = function (selector, html) {
  var targetElem = document.querySelector(selector);
  targetElem.innerHTML = html;
};

// Show loading icon inside element identified by 'selector'.
var showLoading = function (selector) {
  var html = "<div class='text-center'>";
  html += "<img src='images/ajax-loader.gif'></div>";
  insertHtml(selector, html);
};

// Return substitute of '{{propName}}'
// with propValue in given 'string'
var insertProperty = function (string, propName, propValue) {
  var propToReplace = "{{" + propName + "}}";
  string = string
    .replace(new RegExp(propToReplace, "g"), propValue);
  return string;
};

// Remove the class 'active' from home and switch to Menu button
var switchMenuToActive = function () {
  // Remove 'active' from home button
  var classes = document.querySelector("#navHomeButton").className;
  classes = classes.replace(new RegExp("active", "g"), "");
  document.querySelector("#navHomeButton").className = classes;

  // Add 'active' to menu button if not already there
  classes = document.querySelector("#navMenuButton").className;
  if (classes.indexOf("active") === -1) {
    classes += " active";
    document.querySelector("#navMenuButton").className = classes;
  }
};

// On page load (before images or CSS)
// On page load (before images or CSS)
document.addEventListener("DOMContentLoaded", function (event) {

// On first load, show home view
showLoading("#main-content");
$ajaxUtils.sendGetRequest(
  homeHtmlUrl,
  function (responseText) {
    document.querySelector("#main-content")
      .innerHTML = responseText;
  },
  false);
});




// 'categoryShort' is a short_name for a category
dc.loadDegradation = function () {
  showLoading("#main-content");
  $ajaxUtils.sendGetRequest(
    BabuinListHtml,
    buildBabuinListHTML);
};


// Builds HTML for the single category page based on the data
// from the server
function buildBabuinListHTML (list) {
   $ajaxUtils.sendGetRequest(
      BabuinItemTitle,
      function (BabuinItemTitle){
        $ajaxUtils.sendGetRequest(
          BabuinItemHtml,
          function (BabuinItemHtml) {
            // Switch CSS class active to menu button
            switchMenuToActive();
            var BabuinItemsViewHtml =
              buildBabuinListViewHtml(list,
                                      BabuinItemTitle,
                                      BabuinItemHtml);
            insertHtml("#main-content", BabuinItemsViewHtml);
          },
          false);
    },
    false);
}


// Using category and menu items data and snippets html
// build menu items view HTML to be inserted into page
function buildBabuinListViewHtml(list,
                                 ItemTitle,
                                 Item) {

  var finalHtml = ItemTitle;
  finalHtml += "<section class='row'>";

  // Loop over menu items
  for (var i = 0; i < list.length; i++) {
    // Insert menu item values
    var html = Item;
    html = insertProperty(html, "short_name", list[i].short_name);
    html = insertProperty(html, "hazard", list[i].hazard);
    html = insertProperty(html,"name", list[i].name);
    html = insertProperty(html, "description", list[i].description);

    // Add clearfix after every second menu item
    if (i % 2 !== 0) {
      html +=
        "<div class='clearfix d-none d-md-block'></div>";
    }

    finalHtml += html;
  }

  finalHtml += "</section>";
  return finalHtml;
}

dc.loadAgression = function () {
  showLoading("#main-content");
  $ajaxUtils.sendGetRequest(
    PoetHtml,
    function (html){
      switchMenuToActive();
      document.querySelector("#main-content")
      .innerHTML = html;
    }, false);
};







global.$dc = dc;

})(window);