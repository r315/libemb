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
    if (bmpArray[0] != 66 || bmpArray[1] != 77){ //BM
        console.log("Error: image is not bmp")
        return null
    }

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
        console.log('Input pixel format unsupported')
        return null
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

        case BPP.BPP16:
		    for (let i = pixelDataOffset, pixelcount = 0; i < bmpArray.length; i += Bpp) {

	            let b = bmpArray[i + 0] >> 3
                let g = bmpArray[i + 1] >> 2
                let r = bmpArray[i + 2] >> 3

                let d = (r << 11) | (g << 5) | b // RGB565

                pixelByteArray.push(d)

                if(++pixelcount == width){
                    i += padding
                    pixelcount = 0;
                }
            }

            bpl = width * 2
            break;

        default:
	        console.log('Output pixel format unsupported')
		return null

    }

    return {
        "width": width,
        "height": height,
        "pixels": pixelByteArray,
        "bpl": bpl
    }
}

function flipBmp(bmpdata){
    let pixels = []

    for (let offset = (bmpdata.height * bmpdata.width) - bmpdata.width; offset >= 0; offset -= bmpdata.width) {        
        pixels = pixels.concat(bmpdata.pixels.slice(offset, offset + bmpdata.width))
    }
    
    bmpdata.pixels = pixels
    return bmpdata
}

function getLine(data, offset, len, suffix = ","){
    return data.slice(offset, offset + len).map((e) => {
        return (e < 16) ? "0x0" : "0x" + e.toString(16)
    }) + suffix
}

function saveToFile(bmpdata, filename, cols){
    let linecount = 0

    cols = (cols > 0) ? cols : bmpdata.width

    function nextBlock(err){
        if(err){
            console.log(err)
            return
        }
        if( (linecount += cols) < (cols * bmpdata.height))
            fs.appendFile(filename, getLine(bmpdata.pixels, linecount, cols, ',\n'), nextBlock)
    }   
    fs.writeFile(filename, getLine(bmpdata.pixels, linecount, cols, ',\n'), nextBlock)
}

function printBmpData(bmpdata, cols) {
    
    if(bmpdata.width <= 0 || bmpdata.height <= 0)
        return

    cols = (cols > 0) ? cols : bmpdata.width
      
    for (let i = 0; i < bmpdata.pixels.length; i += cols) {
        console.log(`${getLine(bmpdata.pixels, i, cols)}`)
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
        let bmpdata = convertBmp(data, opts.b)

        if(bmpdata == null)
            process.exit(-1)

        bmpdata = flipBmp(bmpdata);

        if(opts.o)
            saveToFile(bmpdata, opts.o.trim(), opts.c)
        else
            printBmpData(bmpdata, opts.c)
    })

}

start()
