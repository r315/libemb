const fs = require('fs')


function genBlockFile(outfile, freq, points){
let values = []

    for(t = 0; t < points; t++)
    {
        let val = Math.sin(2*Math.PI*1000*(t/points))
        val += Math.sin(2*Math.PI*2000*(t/points)) / 2
        val += Math.sin(2*Math.PI*3000*(t/points)) / 4
        values.push(val.toFixed(5))
    }

    fs.writeFile(outfile, values, (err) => {if(err)console.log(err)})
}

let args = process.argv.slice(2);

genBlockFile('sine.c', args[0], 2048)
