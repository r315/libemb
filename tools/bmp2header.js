const fs = require('fs')
var opts = require('minimist')(process.argv.slice(2));

const BPP = {
    "BPP1": 1,
    "BPP2": 2,
    "BPP8": 8,
    "BPP16": 16
}

function printBytes(filename, cb) {
    let reader = new FileReader();
    let fileByteArray = [];
    reader.readAsArrayBuffer(filename);
    reader.onloadend = (evt) => {
        if (evt.target.readyState == FileReader.DONE) {
            let arrayBuffer = evt.target.result
            let array = new Uint8Array(arrayBuffer);
            for (var i = 0; i < array.length; i++) {
                fileByteArray.push(array[i]);
            }
            cb(fileByteArray)
        }
    }
}

function int32FromArr(a, o) { return a[o] | a[o + 1] << 8 | a[o + 2] << 16 | a[o + 3] }
function int16FromArr(a, o) { return a[o] | a[o + 1] << 8 }

/**
    
*/
function convertBmp(bmpArray, outbpp) {
    if (bmpArray[0] != 66 || bmpArray[1] != 77) //BM
        return

    let pixelByteArray = [];
    let pixelDataOffset = int32FromArr(bmpArray, 10)
    let width = int32FromArr(bmpArray, 18)
    let height = int32FromArr(bmpArray, 22)
    let bpp = int16FromArr(bmpArray, 0x1C)
    let Bpp = bpp / 8
    let padding = width & 3
    let bpl
    
    console.log(`Size: ${width}x${height}, bpp: ${bpp}`)

    if(bpp != 24){
        console.log('Unsupported pixel format')
        return {
            "width": 0,
            "height": 0,
            "data": pixelByteArray
        }
    }

    let pixelbyte = 0;
    let pixelcount = 0;

    switch (outbpp) {
        case BPP.BPP1:
            // image is vertically mirrored
            for (let i = pixelDataOffset; i < bmpArray.length; i += Bpp) {
                let b = bmpArray[i]
                let g = bmpArray[i + 1]
                let r = bmpArray[i + 2]

                if (r != 0 || g != 0 || b != 0) {
                    pixelbyte |= 0x80 >> (pixelcount & 7);
                }

                pixelcount++

                if (pixelcount % 8 == 0 || pixelcount == width) {
                    pixelByteArray.push(pixelbyte)
                    pixelbyte = 0
                    if (pixelcount == width) {
                        pixelcount = 0
                        i += padding
                    }
                }
            }
            bpl = Math.floor(width / 8) + ((width % 8) != 0 ? 1 : 0)
            break;

        case BPP.BPP2:
            // TODO: use all bits available in each line
            // currently 2 bits/byte are not used
            for (let i = pixelDataOffset, pixelcount = 0; i < bmpArray.length; i += Bpp) {
                let b = bmpArray[i] >> 6
                let g = bmpArray[i + 1] >> 6
                let r = bmpArray[i + 2] >> 6
                let d = r << 4 | g << 2 | b // 2-bit per color 
                let t = d < 10 ? `0${d}` : `${d}`
                pixelByteArray.push(t)
                if(++pixelcount == width){
                    i += padding
                    pixelcount = 0;
                }
            }
            bpl = Math.floor(width * 2 / 8) + ((width * 2 % 8) != 0 ? 1 : 0) 
            break;
    }
    return {
        "width": width,
        "height": height,
        "data": pixelByteArray,
        "bpl": bpl
    }
}

function invertBmp(bmpdata){
    let inverted = []
    for (let line = (bmpdata.height * bmpdata.bpl) - bmpdata.bpl; line >= 0; line -= bmpdata.bpl) {        
        inverted = inverted.concat(bmpdata.data.slice(line, line + bmpdata.bpl))
    }
    bmpdata.data = inverted
    return bmpdata
}

function getLine(data, offset, len, suffix){
    let linedata = []
    data.slice(offset, offset + len).forEach(e => {
        if(e < 16)
            linedata.push(`0x0${e.toString(16)}`)
        else
            linedata.push(`0x${e.toString(16)}`)
    })
    if(suffix)
        linedata.push('\n')
    return linedata
}

function saveToFile(bmpdata, filename, cols){
    let linecount = 0

    cols = (cols > 0) ? cols : bmpdata.bpl

    function nextBlock(err){
        if(err){
            console.log(err)
            return
        }
        if( (linecount += cols) < (cols*bmpdata.height))
            fs.appendFile(filename, getLine(bmpdata.data,linecount, cols, '\n'), nextBlock)
    }   
    fs.writeFile(filename, getLine(bmpdata.data,linecount, cols, '\n'), nextBlock)
}

function printMatriz(bmpdata, cols) {
    
    if(bmpdata.width <= 0 || bmpdata.height <= 0)
        return

    cols = (cols > 0) ? cols : bmpdata.bpl   
      
    for (let i = 0; i < (bmpdata.height * bmpdata.bpl); i += cols) {
        console.log(`${getLine(bmpdata.data, i, cols)}`)
    }    
}

function help(){
    console.log("Bitmap file to data array converter")
    console.log("bmp2header -f <file.bmp> [-b bpp] [-c columns] [-o file.out]")
}

function start() {
    
    if(!opts.f){
        console.log("ERROR: missing input file!")
        help()
        return
    }

    if(!(opts.b = parseInt(opts.b, 10))){
        opts.b = BPP.BPP2
    }

    opts.c = parseInt(opts.c, 10)

    fs.readFile(opts.f.trim(), (err, data) => {
        if (err) {
            console.log(err)
            return;
        }
        let bmpdata = invertBmp(convertBmp(data, opts.b))

        if(opts.o)
            saveToFile(bmpdata, opts.o.trim(), opts.c)
        else
            printMatriz(bmpdata, opts.c)
    })

}

start()