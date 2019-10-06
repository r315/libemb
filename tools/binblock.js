const fs = require('fs')


function genBlockFile(filename, blocksize, nblocks){
let data = new Uint8Array(blocksize)
let block = 0

    data.fill(0xff, 0, blocksize)
    data[0] = 0x3D
    data[1] = `${0x30 + block}`
    data[2] = 0x3D

    /*data[3] = 0x20
    data[4] = 0x33
    data[5] = 0x12
    data[6] = 0x00
    data[7] = 0x08*/

    function nextBlock(err){
        if(err){
            console.log(err)
            return
        }
        if(++block == nblocks)
            return
        data[1] = `${0x30 + block}`
        fs.appendFile(filename, data, nextBlock)
    }
    fs.writeFile(filename, data, nextBlock)
}

let args = process.argv.slice(2);

genBlockFile('blocks.bin', 1024, args[0])