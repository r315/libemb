const fs = require('fs')


function genBlockFile(outfile, freq, points){
    const writeStream = fs.createWriteStream(outfile)
    const pathName = writeStream.path
    let values = []
    let line = ""

    writeStream.on('finish', () => {
        console.log(`wrote all the array data to file ${pathName}`);
    })
    
    // handle the errors on the write process
    writeStream.on('error', (err) => {
        console.error(`There is an error writing the file ${pathName} => ${err}`)
    })


    console.log(`Frequency ${freq}`)
    console.log(`points ${points}`)

    let t = (1/freq) / points
    let amplitude = 0x7FFF

    for(let x = 0, p = 0; x < (1/freq) - t; x += t, p++)
    {
        if((p % 10) == 0){
            values.push(line)
            console.log(line)
            line = ""
        }
        let val = Math.sin(2 * Math.PI * freq * x)
        //val += Math.sin(2 * Math.PI * (freq  * 2) * x)
        
        val = Math.floor(amplitude * val)
        
        let hex = (val & 0xFFFF).toString(16)
        line += "0x" + ("0000" + hex).slice(-4)  + ", "
        //line += val.toString() + ", "
        //values.push(val)
        //values.push(val.toFixed(5))
    }

    values.push(line)
    console.log(line)

    //fs.writeFile(outfile, values, (err) => {if(err)console.log(err)})

    values.forEach(value => writeStream.write(`${value}\n`))
    writeStream.end()
}

let args = process.argv.slice(2);

if(args < 2){
    console.log(`usage: sine_gen <freq> <samples>`)
    process.exit(0)
}

let freq = args[0]
let points = args[1]

genBlockFile('sine.c', freq, points)
