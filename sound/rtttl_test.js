rtttlParse = require("./index.js");

//console.log(rtttlParse.parse("Future:d=16,o=5,b=200:4g.,p,4c.,p,2f#.,p,g.,p,a.,p,8g,p,8e,p,8c,p,4f#,p,g.,p,a.,p,8g.,p,8d.,p,8g.,p,8d.6,p,4d.6,p,4c#6,p,b.,p,c#.6,p,2d.6"))
//"scale_up:d=32,o=5,b=100:c,c#,d#,e,f#,g#,a#,b"
console.log(rtttlParse.parse(process.argv[2]));