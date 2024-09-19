const fs = require('fs')


function genBlockFile(freq, sr, outfile){
    let writeStream
    let pathName
    let values = []
    let rows = []
    let line = ""

    let amplitude = 0x7FFF


    let nsamples = sr / freq

    let t = (1 / freq) / nsamples     // time between samples

    console.log(`Time ${t}`)
    console.log(`Samples ${nsamples}`)

    for(let x = 0, p = 0; p < nsamples; x += t, p++)
    {
        if((p % 10) == 0 && !outfile){
            rows.push(line)
            console.log(line)
            line = ""
        }

        let val = Math.sin(2 * Math.PI * freq * x)
        //val += Math.sin(2 * Math.PI * (freq  * 2) * x)

        val = Math.floor(amplitude * val)

        let hex = (val & 0xFFFF).toString(16)
        hex = ("0000" + hex).slice(-4)
        values.push(hex)
        line += "0x" + hex  + ", "
        //line += val.toString() + ", "
        //rows.push(val)
        //rows.push(val.toFixed(5))
    }

    rows.push(line)

    if(!outfile){
        console.log(line)
    }

    /* Output for verilog memory initialization
    line = ""

    values.slice().reverse().forEach(x => {
        line += x;
        if (line.length == 64){
            console.log(line)
            line = ""
        }
    })

    console.log(line)
    */

    if(outfile) {
        writeStream = fs.createWriteStream(outfile)
        pathName = writeStream.path

        writeStream.on('finish', () => {
            console.log(`wrote all the array data to file ${pathName}`);
        })

        // handle the errors on the write process
        writeStream.on('error', (err) => {
            console.error(`There is an error writing the file ${pathName} => ${err}`)
        })

        rows.forEach(value => writeStream.write(`${value}\n`))
        writeStream.end()
    }
}

let args = process.argv.slice(2);

if(args.length < 2){
    console.log(`usage: sine_gen <freq> <sample rate> [file]`)
    console.log("Generate ONE Cycle for given sample rate")
    process.exit(0)
}

genBlockFile(args[0], args[1], args[2])
