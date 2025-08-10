/**
 * Basic windows bmp file to c header converter
 *
 * Usage: bmp2header -f <file.bmp> [options]
 *
 * -o <filename> output filename
 * -b <n> output bpp
 * -c <n> Number of columns on out put array
 * -k Rotate image 90º
 * -i Header information
 * -p pack data
 */
const fs = require('fs')
var opts = require('minimist')(process.argv.slice(2));

const BPP = {
    "BPP1": 1,
    "BPP2": 2,
    "BPP4": 4,
    "BPP8": 8,
    "BPP16": 16,
    "BPP24": 24
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

function printBmpHeader(bmpRawData)
{
    let bmp = getBmpHeader(bmpRawData)

    if(bmp == null){
        return
    }

    console.log("File size: " + bmp.fileSize + " Bytes")
    console.log("Data offset: " + bmp.pixelDataOffset)
    console.log("DIB size: " + bmp.dibSize)
    console.log("Width: " + bmp.width)
    console.log("Height: " + bmp.height)
    console.log("Planes: " + bmp.planes)
    console.log("Bpp: " + bmp.bpp)
    console.log("Compression: " + bmp.compression)
    console.log("Image Size: " + bmp.imgSize)
    console.log("X Res: " + bmp.xRes)
    console.log("Y Res: " + bmp.yRes)
    console.log("Pal size: " + bmp.palSize)
    console.log("Important colors: " + bmp.importantColors)
    console.log("Padding: " + bmp.padding)

    if(bmp.palSize > 0){
        bmp.palette.forEach((element, idx) => {
            console.log(`[${idx.toString().padStart(2, '0')}] ${element.toString(16).toUpperCase()}`)
        })
    }
}

function int32FromArr(a, o) { let number = a[o] | a[o + 1] << 8 | a[o + 2] << 16 | a[o + 3] << 24; return (number < 0) ? 0xFFFFFFFF + number + 1 : number; }
function int16FromArr(a, o) { return a[o] | a[o + 1] << 8 }
function isNumber(value) { return typeof value === 'number' }

function getBmpHeader(bmpRawData) {
    if (bmpRawData[0] != 66 || bmpRawData[1] != 77){ //BM
        console.log("Error: image is not bmp")
        return null
    }

    let bmp = {
        "fileSize" : int32FromArr(bmpRawData, 2),
        "dibSize" : int32FromArr(bmpRawData, 14),
        "pixelDataOffset" : int32FromArr(bmpRawData, 10),
        "width" : int32FromArr(bmpRawData, 18),
        "height" : int32FromArr(bmpRawData, 22),
        "planes" : int16FromArr(bmpRawData, 26),
        "bpp" : int16FromArr(bmpRawData, 28),
        "compression" : int32FromArr(bmpRawData, 30),
        "imgSize" : int32FromArr(bmpRawData, 34),
        "xRes" : int32FromArr(bmpRawData, 38),
        "yRes" : int32FromArr(bmpRawData, 42),
        "palSize" : int32FromArr(bmpRawData, 46),
        "importantColors" :  int32FromArr(bmpRawData, 50)
    }

    let rowSize = Math.ceil(bmp.bpp * bmp.width / 32) * 4
    let rowWidth = Math.ceil(bmp.bpp * bmp.width / 8)
    bmp.padding = rowSize - rowWidth
    bmp.pixeldata = bmpRawData.slice(bmp.pixelDataOffset)

    if(bmp.palSize > 0){
        bmp.palette = []
        for(let i = 0; i < bmp.palSize; i++){
            bmp.palette.push(int32FromArr(bmpRawData, 54 + (i * 4)))
        }
    }

    return bmp
}
/**
 * Indexed convertion for input bpp <= BPP8
 */
function convertBmpIndexed(bmpHeader, outbpp)
{
    let pixels = []
    let pixelindex = 0

    if(bmpHeader.bpp == BPP.BPP1 || bmpHeader.bpp == BPP.BPP2){
        console.log('Unsupported input pixel format (1bpp/2bpp) for indexed conversion')
        return null
    }

    if(bmpHeader.bpp == BPP.BPP8){
        // 8bpp => 1bpp, 2bpp, 4bpp, 8bpp
        const pixelMask = (1 << outbpp) - 1
        for (let i = 0; i < bmpHeader.pixeldata.length; i++) {
            pixels.push(bmpHeader.pixeldata[i] & pixelMask)
            pixelindex++
            // Check if we reached line end and add padding
            if((pixelindex % bmpHeader.width) == 0){
                i += bmpHeader.padding
            }
        }
    }else{
        if (bmpHeader.bpp == BPP.BPP24 && (outbpp == BPP.BPP2 || outbpp == BPP.BPP4)) {
            /**
             * Convert 24bpp to indexed
             */
            console.log('Creating palette..')
            // 24bpp >= xbpp (indexed)
            let palette = new Map()
            let colorIndex = 0

            for (let i = 0, pixelcount = 0; i < bmpHeader.pixeldata.length; i += 3) {
                // merge color components into color number
                let color = bmpHeader.pixeldata[i + 2] << 16 |
                            bmpHeader.pixeldata[i + 1] << 8 |
                            bmpHeader.pixeldata[i + 0];
                // Add color to palette if not present
                if(!palette.has(color)){
                    palette.set(color, colorIndex++)
                }
                // Save color index, this may be > than 255
                pixels.push(palette.get(color))
                // Add padding if needed
                if((++pixelcount) % bmpHeader.width == 0){
                    i += bmpHeader.padding
                }
            }

            bmpHeader.palette = Array.from(palette.keys())
        }else{
            // 4bpp => 1bpp, 2bpp, 4bpp, 8bpp
            const pixelMask = ((outbpp > BPP.BPP4) ? BPP.BPP4 : (1 << outbpp)) - 1
            let first = true
            for (let i = 0; i < bmpHeader.pixeldata.length; ) {
                // Write one pixel and advance rawbmpdata index
                if(first){
                    pixels.push((bmpHeader.pixeldata[i] >> 4) & pixelMask) // truncate first pixel
                    first = false
                }else{
                    pixels.push(bmpHeader.pixeldata[i] & pixelMask) // and second pixel
                    i++
                    first = true
                }
                // Check if we reached line end and add padding
                if(((++pixelindex) % bmpHeader.width) == 0){
                    // check if line ended at half byte,
                    // if so skip current byte
                    if(first == false){
                        i++
                        first = true
                    }
                    i += bmpHeader.padding
                }
            }
        }
    }

    return {
        "width": bmpHeader.width,
        "height": bmpHeader.height,
        "bpp": outbpp,
        "pixels": pixels,
        "palette" : bmpHeader.palette
    }
}
/**
 * 24bpp convertion
 *
 */
function convertBmp24(bmpHeader, outbpp)
{
    let pixelByteArray = []
    let convertFunc


    switch (outbpp) {
        case BPP.BPP1:
            // 24bpp >= 1bpp
            convertFunc = (r,g,b) => {return ((r | g | b ) == 0) ? 0 : 1}
            break;

        case BPP.BPP8:
            // 24bpp >= 8bpp (RGB332)
            convertFunc = (r,g,b) => {
                r >>= 5
                g >>= 5
                b >>= 6
                return (r << 5) | (g << 2) | b }
            break;

        case BPP.BPP16:
            // 24bpp >= 16bpp (RGB565)
            convertFunc = (r,g,b) => {
                r >>= 3
                g >>= 2
                b >>= 3
                return (r << 11) | (g << 5) | b }
            break;

        case BPP.BPP24:
            // 24bpp >= 24bpp
            convertFunc = (r,g,b) => {return (r << 16) | (g << 8) | b }
            break

        default:
	        console.log('Output pixel format unsupported')
		    return null
    }

    // 24bpp >= xbpp
    for (let i = 0, pixelcount = 0; i < bmpHeader.pixeldata.length; i += 3) {
        pixelByteArray.push(convertFunc(bmpHeader.pixeldata[i + 2], bmpHeader.pixeldata[i + 1], bmpHeader.pixeldata[i + 0]))
        if(++pixelcount == bmpHeader.width){
            i += bmpHeader.padding
            pixelcount = 0;
        }
    }

    return {
        "width": bmpHeader.width,
        "height": bmpHeader.height,
        "bpp": outbpp,
        "pixels": pixelByteArray
    }
}

/**
 * Extracts pixel data (padding is removed) from raw bmp data and
 * stores it in a pixel array in wich each pixel is represented by a single number.
 * Handles different input formats (4bpp, 8bpp, 24bpp) and the returned array is flipped
 * vertically
 *
 * When using 1bpp as output format, ensure that input umage uses color 0x000000
 * other wise the output will be only 0xFF
 */
function convertBmp(bmpRawData, outbpp) {
    let bmpHeader = getBmpHeader(bmpRawData)

    if(bmpHeader == null){
        return bmpHeader
    }
    // if no outbpp specified, use input bpp
    if(!isNumber(outbpp)){
        outbpp = bmpHeader.bpp
    }

    console.log(`Input:  ${bmpHeader.width}x${bmpHeader.height}, ${bmpHeader.bpp}bpp`)
    console.log(`Output: ${bmpHeader.width}x${bmpHeader.height}, ${outbpp}bpp`)

    if(bmpHeader.bpp <= 8 || outbpp == BPP.BPP2){
        return convertBmpIndexed(bmpHeader, outbpp)
    }else if(bmpHeader.bpp == 24){
        return convertBmp24(bmpHeader, outbpp)
    }

    console.log('Input pixel format unsupported')
    return null
}

/**
 * Pack pixels, this is used for indexed images
 * and if bpp < 8 then multiple pixel are stored
 * in one byte
 */
function packData(bmpdata){
    let pixels = []
    let pixelDataByte = 0
    let pixelIndex
    const pixelModulus = (8 / bmpdata.bpp) - 1
    const pixelMask = (1 << bmpdata.bpp) - 1

    if(bmpdata.bpp >= BPP.BPP8){
        return bmpdata
    }

    if(bmpdata.palette > (1 << bmpdata.bpp)){
        console.log("Error: palette size is greater than 2^bpp")
        return bmpdata
    }
    console.log("Packing data..")

    // Pack pixel data into bytes
    for(pixelIndex = 0; pixelIndex < bmpdata.width * bmpdata.height; pixelIndex++){
        pixelDataByte <<= bmpdata.bpp
        pixelDataByte |= bmpdata.pixels[pixelIndex] & pixelMask
        if((pixelIndex & pixelModulus) == pixelModulus){
            pixels.push(pixelDataByte)
            pixelDataByte = 0
        }
    }

    // Add final byte if it has some pixel
    let remaning = ((8 / bmpdata.bpp) - (pixelIndex & pixelModulus)) & pixelModulus
    if(remaning > 0){
        while(remaning--){
            pixelDataByte <<= bmpdata.bpp
        }
        pixels.push(pixelDataByte)
    }

    bmpdata.pixels = pixels
    return bmpdata
}
/**
 * Performs a vertical mirroring
 *
 * @param {*} bmpdata
 * @returns
 */
function flipBmp(bmpdata){
    let pixels = []

    for (let offset = (bmpdata.height * bmpdata.width) - bmpdata.width; offset >= 0; offset -= bmpdata.width) {
        pixels = pixels.concat(bmpdata.pixels.slice(offset, offset + bmpdata.width))
    }

    bmpdata.pixels = pixels
    return bmpdata
}

/**
 * Transpose pixels, this is used to rotate an
 * image 90ª for displays that have byte pages
 * Currently this only works for 1bit per pixel images
 *
 */
function transpose(bmpdata){
    let pixels = []

    for(let row = 0; row < bmpdata.width; row ++){
        for(let col = 0; col < bmpdata.height; col++){
            let offsetRow = bmpdata.width * (bmpdata.height - 1 - col)
            pixels.push(bmpdata.pixels[offsetRow + row])
        }
    }

    bmpdata.pixels = pixels
    return bmpdata
}

function dataToString(data, offset, len, nbits = 8, suffix = ","){
    return data.slice(offset, offset + len).map((e) => {
        return "0x" + e.toString(16).padStart((nbits < 8) ? 2 : nbits/4, '0')
    }) + suffix
}

function saveToFile(bmpdata, filename, cols){
    let count = bmpdata.pixels.length
    let offset = 0

    function nextBlock(err){
        if(err){
            console.log(err)
            return
        }

        count -= cols
        offset += cols

        if(count > 0){
            let line = dataToString(bmpdata.pixels, offset, cols, bmpdata.bpp, ',\n')
            fs.appendFile(filename, line, nextBlock)
        }
    }

    cols = (cols > 0) ? cols : bmpdata.width

    if(bmpdata.palette){
        let palette = "["
        bmpdata.palette.forEach(color => {
            let b = (color >> 3) & 31;
            let g = (color >> 10) & 63;
            let r = (color >> 19) & 31;
            color = (r << 11) | (g << 5) | b
            palette += `0x${color.toString(16).padStart(4, '0')},` // RGB565
        })

        palette +="]\n\n"

        fs.writeFile(filename, palette, (err)=>{
            if(err){
                console.log(err)
                return
            }
            let line = dataToString(bmpdata.pixels, offset, cols, bmpdata.bpp, ',\n')
            fs.appendFile(filename, line, nextBlock)
        })
    }else{
        let line = dataToString(bmpdata.pixels, offset, cols, bmpdata.bpp, ',\n')
        fs.writeFile(filename, line, nextBlock)
    }

}

function printBmpData(bmpdata, cols) {

    if(bmpdata.width <= 0 || bmpdata.height <= 0)
        return

    cols = (cols > 0) ? cols : bmpdata.width

    if(bmpdata.palette){
        console.log(`Palette: ${dataToString(bmpdata.palette, 0, cols)}`)
    }

    for (let i = 0; i < bmpdata.pixels.length; i += cols) {
        console.log(`${dataToString(bmpdata.pixels, i, cols,bmpdata.bpp)}`)
    }
}

function help(){
    console.log("Bitmap file to data array converter")
    console.log("bmp2header -f <file.bmp> [options]")
    console.log("\toptions :")
    console.log("\t  -o <filename>, output file name")
    console.log("\t  -c <n>,        Number of colums on output array")
    console.log("\t  -b <b>,        Output Bits per pixel")
    console.log("\t  -i,            Print bitmap header information")
    console.log("\t  -k,            Transpose data")
    console.log("\t  -p,            Pack data, when out bpp is < 8BPP")
}

function start() {

    if(!opts.f){
        console.log("ERROR: missing input file!")
        help()
        return
    }

    opts.c = parseInt(opts.c, 10)

    fs.readFile(opts.f.trim(), (err, data) => {
        if (err) {
            console.log(err)
            return;
        }

        if(opts.i){
            printBmpHeader(data)
            return;
        }

        let bmpdata = convertBmp(data, opts.b)

        if(bmpdata == null)
            process.exit(-1)

        bmpdata = flipBmp(bmpdata);

        if(opts.k){
            bmpdata = transpose(bmpdata)
        }

        if(opts.p){
            bmpdata = packData(bmpdata)
        }

        if(opts.o)
            saveToFile(bmpdata, opts.o.trim(), opts.c)
        else
            printBmpData(bmpdata, opts.c)
    })

}

start()

/*
2x2 8bit image bmp data

    BMP Header
00: 42 4D
02: 4E 00 00 00    4E = 78 = 14 + 40 + (4 * 4) + 8
06: 00 00
08: 00 00
0A: 46 00 00 00    46 = 70 = 14 + 40 + (4 * 4)

    DIB (Device-Independent bitmap)
0E: 28 00 00 00   size (40)
12: 02 00 00 00   w
16: 02 00 00 00   h
1A: 01 00         plane
1C: 08 00         bpp
1E: 00 00 00 00   compression
22: 00 00 00 00   imagesize
26: C4 0E 00 00   xres
2A: C4 0E 00 00   yres
2E: 04 00 00 00   number of colors in palette
32: 04 00 00 00   Important colors

    Palette         16
36: 00 00 FF FF     BGRA
3A: FF 00 00 FF
3E: 00 FF 00 FF
42: 00 00 00 FF

    Pixel data
46: 02 03 00 00
4A: 00 01 00 00
*/