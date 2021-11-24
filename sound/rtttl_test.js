rtttlParse = require("./rtttl_parser.js");

//"scale_up:d=32,o=5,b=100:c,c#,d#,e,f#,g#,a#,b"

let rtttl = process.argv[2];

if(rtttl == undefined){
    console.log("No rtttl given\n parsing example");    
    rtttl = "Future:d=16,o=5,b=200:4g.,p,4c.,p,2f#.,p,g.,p,a.,p,8g,p,8e,p,8c,p,4f#,p,g.,p,a.,p,8g.,p,8d.,p,8g.,p,8d.6,p,4d.6,p,4c#6,p,b.,p,c#.6,p,2d.6"
}

console.log(rtttlParse.parse(rtttl));

